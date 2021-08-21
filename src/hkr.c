/* hot key runner */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>


/* run a command from calling process */
void run(char *cmd) {
	char *full_cmd[] = {"sh", "-c", cmd, NULL};
	if (fork() == 0) {
		if (fork() == 0) {
			setsid();
			execvp(full_cmd[0], full_cmd);
		}
		exit(EXIT_SUCCESS);
	}
	wait(NULL);
}


/* catch signal and run associated command from signal number */
void handle_sig(int signo, siginfo_t *info, void *extra) {
	run("$TERMINAL");
}


int main(int argc, char *argv[]) {
	/* save pid to cache file */
	FILE *cache = fopen(strcat(getenv("XDG_CACHE_HOME"), "/hkd_cache"), "w");
	char pid[20];
	sprintf(pid, "%d\n", getpid());
	fputs(pid, cache);
	fclose(cache);

	/* wait for signal */
	struct sigaction new;
	sigset_t mask;
	while (1) {
		new.sa_sigaction = handle_sig;
		sigaction(SIGUSR1, &new, NULL);
		sigsuspend(&mask);
	}
}
