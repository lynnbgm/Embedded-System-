#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

/******************************************************
  Prepared by Maha Ashour
  
  Usage:
    ./ktimer [flag] [args...]
	 
	-s n message (set timer for n secs with message)
	-l (list timer message and remaining time)
	-r (delete timer)
	
  Examples:

	./ktimer -s 30 "This is a 30 sec timer"
		New timer set

	./ktimer -l
		30secTimer 30
	
	./ktimer -r
		Timer deleted

******************************************************/

void printManPage(void);
char* concat(char*, char*);
void sighandler(int);
void printmsg(char*);
char* msg;

int main(int argc, char **argv) {
	int pFile, oflags;
	struct sigaction action, oa;
	int ii, count = 0;
	char line[256];

	// Opens to device file
	pFile = open("/dev/mytimer", O_RDWR);
	if (pFile < 0) {
		fprintf (stderr, "mytimer module isn't loaded\n");
		return 1;
	}

	// Read mode - list current timer
	if (argc == 2 && strcmp(argv[1], "-l") == 0) {
		// Read from file.
		if(read(pFile, line, 256) > 0)
			printf("%s\n", line);
	}

	// Write mode - delete timer
	else if (argc == 2 && strcmp(argv[1], "-r") == 0) {
		// Write to file.
		write(pFile, "", 0);
	}
		
	// Write mode - set timer
	else if (argc == 4 && strcmp(argv[1], "-s") == 0) {

		// Store msg for printing
		msg = argv[3];
		int time = atoi(argv[2]);
		char *arg = concat(argv[2], argv[3]);
		
		// Setup signal handler
		memset(&action, 0, sizeof(action));
		action.sa_handler = sighandler;
		action.sa_flags = SA_SIGINFO;
		sigemptyset(&action.sa_mask);
		sigaction(SIGIO, &action, NULL);
		fcntl(pFile, F_SETOWN, getpid());
		oflags = fcntl(pFile, F_GETFL);
		fcntl(pFile, F_SETFL, oflags | FASYNC);

		// Write to file.
		int result = write(pFile, arg, 256);
		if (result == 0) {
			printf("A timer exists already!\n");
		} else if (result == 1) {		
			// Wait
			pause();	
		} else if (result == 2) {
			// Reset current timer
			printf("Timer %s has now been reset to %d seconds!\n",msg,time); 							
			pause();
		}
	}
	
	// Otherwise invalid
	else {
		printManPage();
	}

	// Closes.
	close(pFile);
	return 0;
}

/* Man page for ktimer */
void printManPage() {
	printf("Error: invalid use.\n");
	printf(" timerset [-flag] [args...]\n");
	printf(" -l: list timer and remaining time\n");	
	printf(" -s n message: set new timer for n seconds & display message\n");
	printf(" -r : remove existing timer\n");
}

void printmsg(char* arg) {
	printf("%s\n",arg);
}

// SIGIO handler
void sighandler(int signo)
{
	// Print message on awake signal
	printmsg(msg);
}

/* concatenate two char* and return char* */
char* concat(char* first, char* second) {
	char* new = first;
	while(*first)
		first++;
	*first = ' ';
	first++;
	while(*second) {
		*first = *second;
		first++;
		second++;
	}
	*first = '\0';
	return new;
}

