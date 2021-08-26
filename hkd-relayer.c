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


/**
 * Get an event from stdin
 */
int read_event(struct input_event *event) {
	return fread(event, sizeof(struct input_event), 1, stdin) == 1;
}


/**
 * Write an event to stdout
 */
void write_event(const struct input_event *event) {
	if (fwrite(event, sizeof(struct input_event), 1, stdout) != 1)
		exit(EXIT_FAILURE);
}


/**
 * Update the mod state and test if modifier
 *
 * @return whether the value is a modifier
 */
int try_modifier(int key, unsigned int *mod_state) {
	switch (key) {
		case KEY_LEFTSHIFT:
		case KEY_RIGHTSHIFT:
			*mod_state ^= 0b11000000;
			return 1;
		case KEY_LEFTALT:
		case KEY_RIGHTALT:
			*mod_state ^= 0b00110000;
			return 1;
		case KEY_LEFTMETA:
		case KEY_RIGHTMETA:
			*mod_state ^= 0b00001100;
			return 1;
		case KEY_LEFTCTRL:
		case KEY_RIGHTCTRL:
			*mod_state ^= 0b00000011;
			return 1;
		default:
			return 0;
	}
}


/**
 * Attempt to run associated command
 *
 * @return whether the key combination is a hotkey
 */
int try_hotkey(unsigned int key, unsigned int mod_state, pid_t pid) {
	union sigval msg;
	for (msg.sival_int = 0; msg.sival_int < LENGTH(bindings); msg.sival_int++) {
		if (bindings[msg.sival_int].key  == key
		 && bindings[msg.sival_int].mods == mod_state) {
			sigqueue(pid, SIGUSR1, msg);
			return 1;
		}
	}
	return 0;
}


int main(int argc, char *argv[]) {
	/* find hkd pid */
	FILE *cache = popen("pidof hkd", "r");
	char pid_str[20];
	fgets(pid_str, 20, cache);
	pid_t pid = strtoul(pid_str, NULL, 10);
	fclose(cache);

	/* process events */
	struct input_event input;
	unsigned int mod_state = 0;
	setbuf(stdin, NULL), setbuf(stdout, NULL);
	while (read_event(&input)) {
		/* make mouse and touchpad events consume pressed taps */
		if (input.type == EV_MSC && input.code == MSC_SCAN) continue;

		/* forward anything that is not a key event, including SYNs */
		if (input.type != EV_KEY) {
			write_event(&input);
			continue;
		}

		/* process key */
		switch (input.value) {
			case INPUT_VAL_PRESS:
				if (try_modifier(input.code, &mod_state)
				 || !try_hotkey(input.code, mod_state, pid)) {
					write_event(&input);
				}
				break;
			case INPUT_VAL_RELEASE:
				if (try_modifier(input.code, &mod_state)) {
					try_hotkey(input.code, mod_state, pid);
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

	return 0;
}
