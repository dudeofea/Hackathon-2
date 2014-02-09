/*
 * Program that plays and controls the BGM of the game.
 */
#include "BMG.h"

void BMG_player()
{
  int pstat;
  pid_t pid;
  if(!system(NULL))
    	exit(EXIT_FAILURE); //.. processor is not available.
  // Make it a daemon
  pstat = system("make play");
/*
	pid  = 1;
	if(pid == 0)
	{
		FILE *fpipe = (FILE*)popen("make play","r");
		static char line[256];
		while ( fgets( line, sizeof(line), fpipe))
		{
			printf("%s", line);
		}
		pclose(fpipe);
	}else { }
		// do nothing
 */	
}
