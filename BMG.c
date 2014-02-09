/*
 * Program that plays and controls the BGM of the game.
 */

#include <stdio.h>
#include <stdlib.h>

int main()
{
  int pstat;
  if(!system(NULL))
    exit(EXIT_FAILURE); //.. processor is not available.
  // Make it a daemon
  pstat = system("mpg321 -q -z -B ~/Hackathon/music/ &");
  printf("value returned was %d\n", pstat);
  exit(0);
}