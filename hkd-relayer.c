#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <linux/input.h>

#include "config.h"

/* https://www.kernel.org/doc/html/latest/input/event-codes.html */
#define INPUT_VAL_PRESS 1
#define INPUT_VAL_RELEASE 0
#define INPUT_VAL_REPEAT 2


int read_event(struct input_event *event) {
	return fread(event, sizeof(struct input_event), 1, stdin) == 1;
}


void write_event(const struct input_event *event) {
	if (fwrite(event, sizeof(struct input_event), 1, stdout) != 1)
		exit(EXIT_FAILURE);
}


int main(int argc, char *argv[]) {
	/* find hkd pid */
	FILE *cache = popen("pidof hkd", "r");
	char pid_str[20];
	fgets(pid_str, 20, cache);
	pid_t pid = strtoul(pid_str, NULL, 10);
	fclose(cache);

	union sigval message;
	struct input_event input;
	setbuf(stdin, NULL), setbuf(stdout, NULL);
	while (read_event(&input)) {
		/* make mouse and touchpad events consume pressed taps */
		if (input.type == EV_MSC && input.code == MSC_SCAN) continue;

		/* forward anything that is not a key event, including SYNs */
		if (input.type != EV_KEY) {
			write_event(&input);
			continue;
		}

		int bound;
		switch (input.value) {
			case INPUT_VAL_PRESS:
				bound = 0;
				for (message.sival_int = 0; message.sival_int < LENGTH(bindings); message.sival_int++) {
					if (bindings[message.sival_int].key == input.code) {
						bound = 1;
						sigqueue(pid, SIGUSR1, message);
					}
				}
				if (!bound) write_event(&input);
				break;
			case INPUT_VAL_RELEASE:
				if (input.code == KEY_BACKSLASH) {}
				else write_event(&input);
				break;
			case INPUT_VAL_REPEAT:
				/* linux console, X, wayland handles repeat */
				break;
			default:
				fprintf(stderr, "unexpected .value=%d .code=%d, doing nothing",
				        input.value,
				        input.code);
				break;
		}
	}
}
