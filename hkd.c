/* hot key runner */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

#include "config.h"


/**
 * Execute a command in a seperate process
 */
void spawn(char *cmd[]) {
	if (fork() == 0) {
		setsid();
		execvp(cmd[0], cmd);
		exit(EXIT_SUCCESS);
	}
}


/**
 * Execute associated command from bindings array based
 * on the passed signal value
 */
void handle_sig(int signo, siginfo_t *info, void *extra) {
	spawn((char**)bindings[info->si_value.sival_int].cmd);
}


void print_usage(const char *program) {
	fprintf(stdout,
	        "Hotkey daemon plugin for interception tools:\n"
	        "        https://gitlab.com/interception/linux/tools\n"
	        "\n"
	        "usage: %s [-h]\n"
	        "\n"
	        "options:\n"
	        "    -h                   show this message and exit\n",
	        program);
}


int main(int argc, char *argv[]) {
	int opt;
	while ((opt = getopt(argc, argv, "h")) != -1) {
		switch (opt) {
			case 'h':
				return print_usage(argv[0]), EXIT_SUCCESS;
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
}
