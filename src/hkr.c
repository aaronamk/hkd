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
	if (info->si_value.sival_int == 1) {
		run("$TERMINAL");
	}
}


int main(int argc, char *argv[]) {
	/* wait for signal */
	struct sigaction action;
	sigset_t mask;
	while (1) {
		action.sa_flags = SA_SIGINFO;
		action.sa_sigaction = &handle_sig;
		sigaction(SIGUSR1, &action, NULL);
		sigsuspend(&mask);
	}
}
