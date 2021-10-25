#include <unistd.h>

#include "hk-relay.h"


_Atomic(unsigned int) mod_state = 0;
_Atomic(key_code) last_press = 0;


void print_usage(const char *program) {
	fprintf(stdout,
	        "\n"
	        "usage: %s [options]\n"
	        "\n"
	        "Signal hkd based on info from Interception Tools"
	        "\n"
	        "options:\n"
	        " -h        show this message and exit\n",
	        program);
}


/**
 * Receive keyboard event from stdin
 */
int receive_event(struct input_event *event) {
	return fread(event, sizeof(struct input_event), 1, stdin) == 1;
}


int main(int argc, char *argv[]) {
	if (find_hkd() == 0) {
		fprintf(stderr, "%s: could not find hkd process\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	/* parse options */
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

	/* relay events to the event handler */
	struct input_event input;
	setbuf(stdin, NULL), setbuf(stdout, NULL);
	while (receive_event(&input)) {
		/* done if event was handled */
		if (handle_event(input)) continue;

		/* send event */
		if (fwrite(&input, sizeof(struct input_event), 1, stdout) != 1)
			exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
}
