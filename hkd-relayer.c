#include <stdio.h>
#include <stdbool.h>
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
	unsigned int mod_state = 0;
	bool bound;
	bool is_mod;
	while (read_event(&input)) {
		/* make mouse and touchpad events consume pressed taps */
		if (input.type == EV_MSC && input.code == MSC_SCAN) continue;

		/* forward anything that is not a key event, including SYNs */
		if (input.type != EV_KEY) {
			write_event(&input);
			continue;
		}

		is_mod = false;
		switch (input.value) {
			case INPUT_VAL_PRESS:
				bound = false;
				switch (input.code) {
					case KEY_LEFTSHIFT:
					case KEY_RIGHTSHIFT:
						is_mod = true;
						mod_state ^= 0b11000000;
						break;
					case KEY_LEFTALT:
					case KEY_RIGHTALT:
						is_mod = true;
						mod_state ^= 0b00110000;
						break;
					case KEY_LEFTMETA:
					case KEY_RIGHTMETA:
						is_mod = true;
						mod_state ^= 0b00001100;
						break;
					case KEY_LEFTCTRL:
					case KEY_RIGHTCTRL:
						is_mod = true;
						mod_state ^= 0b00000011;
						break;
				}
				if (!is_mod) {
					for (message.sival_int = 0; message.sival_int < LENGTH(bindings); message.sival_int++) {
						if (bindings[message.sival_int].key == input.code && mod_state == bindings[message.sival_int].mods) {
							bound = true;
							sigqueue(pid, SIGUSR1, message);
						}
					}
				}
				if (!bound) write_event(&input);
				break;
			case INPUT_VAL_RELEASE:
				switch (input.code) {
					case KEY_LEFTSHIFT:
					case KEY_RIGHTSHIFT:
						is_mod = true;
						mod_state ^= 0b11000000;
						break;
					case KEY_LEFTALT:
					case KEY_RIGHTALT:
						is_mod = true;
						mod_state ^= 0b00110000;
						break;
					case KEY_LEFTMETA:
					case KEY_RIGHTMETA:
						is_mod = true;
						mod_state ^= 0b00001100;
						break;
					case KEY_LEFTCTRL:
					case KEY_RIGHTCTRL:
						is_mod = true;
						mod_state ^= 0b00000011;
						break;
				}
				if (is_mod) {
					for (message.sival_int = 0; message.sival_int < LENGTH(bindings); message.sival_int++) {
						if (bindings[message.sival_int].key == input.code && mod_state == bindings[message.sival_int].mods) {
							bound = true;
							sigqueue(pid, SIGUSR1, message);
						}
					}
				}
				write_event(&input);
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
