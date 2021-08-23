/* hot key runner */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
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
	switch (info->si_value.sival_int) {
		case 1:
			run("$TERMINAL");
			break;
	}
}

void
print_usage(const char *program) {
	fprintf(stdout,
			"Hotkey daemon plugin for interception tools:\n"
			"        https://gitlab.com/interception/linux/tools\n"
			"\n"
			"usage: %s [-h | -c CONFIG]\n"
			"\n"
			"options:\n"
			"    -h                   show this message and exit\n"
			"    -c CONFIG            use custom config location\n",
			program);
}


int main(int argc, char *argv[]) {
	int opt;
	while ((opt = getopt(argc, argv, "hc:")) != -1) {
		switch (opt) {
			case 'h':
				return print_usage( argv[0]), EXIT_SUCCESS;
			case 'c':
				/* deal with this later */
				continue;
		}
	}

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
