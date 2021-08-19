/* hot key runner */

#include <stdio.h>
#include <unistd.h>
#include <signal.h>


void handle_sig() {
	puts("handled");
}


int main() {
	struct sigaction new;
	sigset_t mask;

	while (1) {
		new.sa_handler = handle_sig;
		sigaction(SIGUSR1, &new, NULL);
		sigsuspend(&mask);
	}
}
