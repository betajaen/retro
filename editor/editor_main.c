#define RETRO_NO_MAIN
#include "../retro.h"

int main(int argc, char *argv[])
{
  Retro_StartFromLibrary("LibEditor", 0);
  Retro_Shutdown();
  return 0;
}