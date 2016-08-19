#define RETRO_NO_MAIN
#include "../retro.h"

int main(int argc, char *argv[])
{
  Retro_Context_LoadFromLibrary("LibEditor", 0);
  Retro_Shutdown();
  return 0;
}