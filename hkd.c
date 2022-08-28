/* hot key runner */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

#include "config.h"


void print_usage(const char *program) {
	printf("Run commands based on signals\n"
	       "\n"
	       "Usage: %s [options]\n"
	       "\n"
	       "Options:\n"
	       " -h  show this message and exit\n",
	       program);
}


/**
 * Execute a command in a seperate process
 */
void spawn(char *cmd[]) {
	if (fork() == 0) {
		if (fork() == 0) {
			setsid();
			execvp(cmd[0], cmd);
		}
		exit(EXIT_SUCCESS);
	}
	wait(NULL);
}


/**
 * Execute associated command from bindings array based on the passed signal
 * value
 */
void handle_sig(int signo, siginfo_t *info, void *extra) {
	spawn((char**)bindings[info->si_value.sival_int].cmd);
}


int main(int argc, char *argv[]) {
	int opt;
	while ((opt = getopt(argc, argv, "h")) != -1) {
		switch (opt) {
			case 'h':
				print_usage(argv[0]);
				exit(EXIT_SUCCESS);
			case '?':
				fprintf(stderr, "%s: invalid option: %c\n", argv[0], opt);
				print_usage(argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	/* wait for signal */
	struct sigaction action;
	sigset_t mask = {0};
	while (1) {
		action.sa_flags = SA_SIGINFO;
		action.sa_sigaction = &handle_sig;
		sigaction(SIGUSR1, &action, NULL);
		sigsuspend(&mask);
	}

	return EXIT_SUCCESS;
}
