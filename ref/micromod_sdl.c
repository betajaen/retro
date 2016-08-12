// Modified for Retro

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include "SDL.h"

#include "micromod.h"

/*
  Simple command-line test player for micromod using SDL.
*/

#define SAMPLING_FREQ  48000  /* 48khz. */
#define REVERB_BUF_LEN 4800   /* 50ms. */
#define OVERSAMPLE     2      /* 2x oversampling. */
#define NUM_CHANNELS   2      /* Stereo. */
#define BUFFER_SAMPLES 16384  /* 64k buffer. */

// static SDL_sem *semaphore;

typedef struct {
  long samples_remaining, length;
  short reverb_buffer[ REVERB_BUF_LEN ];
  short mix_buffer[ BUFFER_SAMPLES * NUM_CHANNELS * OVERSAMPLE ];
  long reverb_len, reverb_idx, filt_l, filt_r;
} micromod_sdl_context;

/*
  2:1 downsampling with simple but effective anti-aliasing.
  Count is the number of stereo samples to process, and must be even.
  input may point to the same buffer as output.
*/
static void micromod_sdl_downsample(micromod_sdl_context* ctx, short *input, short *output, long count ) {
  long in_idx, out_idx, out_l, out_r;
  in_idx = out_idx = 0;
  while( out_idx < count ) {	
    out_l = ctx->filt_l + ( input[ in_idx++ ] >> 1 );
    out_r = ctx->filt_r + ( input[ in_idx++ ] >> 1 );
    ctx->filt_l = input[ in_idx++ ] >> 2;
    ctx->filt_r = input[ in_idx++ ] >> 2;
    output[ out_idx++ ] = out_l + ctx->filt_l;
    output[ out_idx++ ] = out_r + ctx->filt_r;
  }
}


inline float micromod_sdl_s16_to_float(short i)
{
  float f = ((float) i) / (float) 32768;
  if( f > 1 ) f = 1;
  else if( f < -1 ) f = -1;
  return f;
}

/*
2:1 downsampling with simple but effective anti-aliasing.
Count is the number of stereo samples to process, and must be even.
input may point to the same buffer as output.
*/
static void micromod_sdl_downsample_float(micromod_sdl_context* ctx, short *input, float *output, long count ) {
  long in_idx, out_idx, out_l, out_r;
  in_idx = out_idx = 0;
  while( out_idx < count ) {	
    out_l = ctx->filt_l + ( input[ in_idx++ ] >> 1 );
    out_r = ctx->filt_r + ( input[ in_idx++ ] >> 1 );
    ctx->filt_l = input[ in_idx++ ] >> 2;
    ctx->filt_r = input[ in_idx++ ] >> 2;
    short l = out_l + ctx->filt_l;
    short r = out_r + ctx->filt_r;
    
    output[ out_idx++ ] = micromod_sdl_s16_to_float(l);
    output[ out_idx++ ] = micromod_sdl_s16_to_float(r);

  }
}

/* Simple stereo cross delay with feedback. */
static void micromod_sdl_reverb(micromod_sdl_context* ctx, short *buffer, long count ) {
  long buffer_idx, buffer_end;
  if(ctx->reverb_len > 2 ) {
    buffer_idx = 0;
    buffer_end = buffer_idx + ( count << 1 );
    while( buffer_idx < buffer_end ) {
      buffer[ buffer_idx ] = ( buffer[ buffer_idx ] * 3 + ctx->reverb_buffer[ctx->reverb_idx + 1 ] ) >> 2;
      buffer[ buffer_idx + 1 ] = ( buffer[ buffer_idx + 1 ] * 3 + ctx->reverb_buffer[ctx->reverb_idx ] ) >> 2;
      ctx->reverb_buffer[ctx->reverb_idx ] = buffer[ buffer_idx ];
      ctx->reverb_buffer[ctx->reverb_idx + 1 ] = buffer[ buffer_idx + 1 ];
      ctx->reverb_idx += 2;
      if(ctx->reverb_idx >= ctx->reverb_len ) {
        ctx->reverb_idx = 0;
      }
      buffer_idx += 2;
    }
  }
}

#if 0
static void micromod_sdl_audio_callback( void *udata, Uint8 *stream, int len ) {
  long count;
  count = len * OVERSAMPLE / 4;
  if( samples_remaining < count ) {
    /* Clear output.*/
    memset( stream, 0, len );
    count = samples_remaining;
  }
  if( count > 0 ) {
    /* Get audio from replay.*/
    memset( mix_buffer, 0, count * NUM_CHANNELS * sizeof( short ) );
    micromod_get_audio( mix_buffer, count );
    downsample( mix_buffer, ( short * ) stream, count );
    reverb( ( short * ) stream, count / OVERSAMPLE );
    samples_remaining -= count;
  } else {
    /* Notify the main thread to stop playback.*/
    SDL_SemPost( semaphore );
  }
}
#endif

#if 0
static void termination_handler( int signum ) {
  /* Notify the main thread to stop playback. */
  SDL_SemPost( semaphore );
  fprintf( stderr, "\nTerminated!\n" );
}
#endif

#if 0
static long read_file( char *filename, void *buffer, long length ) {
  FILE *file;
  long count;
  count = -1;
  file = fopen( filename, "rb" );
  if( file != NULL ) {
    count = fread( buffer, 1, length, file );
    if( count < length && !feof( file ) ) {
      fprintf( stderr, "Unable to read file '%s'.\n", filename );
      count = -1;
    }
    if( fclose( file ) != 0 ) {
      fprintf( stderr, "Unable to close file '%s'.\n", filename );
    }
  }
  return count;
}
#endif

static void print_module_info() {
  int inst;
  char string[ 23 ];
  for( inst = 0; inst < 16; inst++ ) {
    micromod_get_string( inst, string );
    printf( "%02i - %-22s", inst, string );
    micromod_get_string( inst + 16, string );
    printf( "%02i - %-22s\n", inst + 16, string );
  }
}

#if 0
static long read_module_length( char *filename ) {
  long length;
  signed char header[ 1084 ];
  length = read_file( filename, header, 1084 );
  if( length == 1084 ) {
    length = micromod_calculate_mod_file_len( header );
    if( length < 0 ) {
      fprintf( stderr, "Module file type not recognised.\n");
    }
  } else {
    fprintf( stderr, "Unable to read module file '%s'.\n", filename );
    length = -1;
  }
  return length;
}
#endif

#if 0
static long play_module( signed char *module ) {
  long result;
  SDL_AudioSpec audiospec;
  /* Initialise replay.*/
  result = micromod_initialise( module, SAMPLING_FREQ * OVERSAMPLE );
  if( result == 0 ) {
    print_module_info();
    /* Calculate song length. */
    samples_remaining = micromod_calculate_song_duration();
    printf( "Song Duration: %li seconds.\n", samples_remaining / ( SAMPLING_FREQ * OVERSAMPLE ) );
    fflush( NULL );
    /* Initialise SDL_AudioSpec Structure. */
    memset( &audiospec, 0, sizeof( SDL_AudioSpec ) );
    audiospec.freq = SAMPLING_FREQ;
    audiospec.format = AUDIO_S16SYS;
    audiospec.channels = NUM_CHANNELS;
    audiospec.samples = BUFFER_SAMPLES;
    audiospec.callback = audio_callback;
    audiospec.userdata = NULL;
    /* Initialise audio subsystem. */
    result = SDL_Init( SDL_INIT_AUDIO );
    if( result == 0 ) {
      /* Open the audio device. */
      result = SDL_OpenAudio( &audiospec, NULL );
      if( result == 0 ) {
        /* Begin playback. */
        SDL_PauseAudio( 0 );
        /* Wait for playback to finish. */
        semaphore = SDL_CreateSemaphore( 0 );
        result = SDL_SemWait( semaphore );
        if( result != 0 ) {
          fprintf( stderr, "SDL_SemWait() failed.\n" );
        }
        /* Close audio device and shut down SDL. */
        SDL_CloseAudio();
        SDL_Quit();
      } else {
        fprintf( stderr, "Unable to open audio device: %s\n", SDL_GetError() );
      }
    } else {
      fprintf( stderr, "Unable to initialise SDL: %s\n", SDL_GetError() );
    }
  } else {
    fprintf( stderr, "Unable to initialise replay.\n" );
  }
  return result;
}
#endif

#if 0
int main( int argc, char **argv ) {
  int arg, result;
  long count, length;
  char *filename;
  signed char *module;
  filename = NULL;
  for( arg = 1; arg < argc; arg++ ) {
    /* Parse arguments.*/
    if( strcmp( argv[ arg ], "-reverb" ) == 0 ) {
      reverb_len = REVERB_BUF_LEN;
    } else {
      filename = argv[ arg ];
    }
  }
  result = EXIT_FAILURE;
  if( filename == NULL ) {
    fprintf( stderr, "Usage: %s [-reverb] filename\n", argv[ 0 ] );
  } else {
    /* Read module file.*/
    length = read_module_length( filename );
    if( length > 0 ) {
      printf( "Module Data Length: %li bytes.\n", length );
      module = calloc( length, 1 );
      if( module != NULL ) {
        count = read_file( filename, module, length );
        if( count < length ) {
          fprintf( stderr, "Module file is truncated. %li bytes missing.\n", length - count );
        }
        /* Install signal handlers.*/
        signal( SIGTERM, termination_handler );
        signal( SIGINT,  termination_handler );
        /* Play.*/
        if( play_module( module ) == 0 ) {
          result = EXIT_SUCCESS;
        }
        free( module );
      }
    }
  }
  return result;
}
#endif