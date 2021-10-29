#include <signal.h>
#include <linux/input.h>
#include <stdlib.h>

#include "config.h"

#define LENGTH(X) sizeof X / sizeof X[0]
#define INPUT_VAL_PRESS 1
#define INPUT_VAL_RELEASE 0
#define INPUT_VAL_REPEAT 2


extern _Atomic(unsigned int) mod_state;
extern _Atomic(key_code) last_press;
static pid_t pid;


/**
 * Set hkd pid
 *
 * @return the pid of hkd or 0 if not found
 */
int find_hkd() {
	FILE *cache = popen("pidof hkd", "r");
	if (cache == NULL) return 0;

	char pid_str[20];
	fgets(pid_str, 20, cache);
	fclose(cache);
	pid = strtoul(pid_str, NULL, 10);
	return pid;
}


/**
 * Attempt to run associated command
 *
 * @return whether the key combination is a hotkey
 */
int try_hotkey(key_code key) {
	int i;
	union sigval msg;
	for (i = 0; i < LENGTH(bindings); i++) {
		if (bindings[i].key == key
		 && bindings[i].mod_mask == mod_state) {
			msg.sival_int = i;
			sigqueue(pid, SIGUSR1, msg);
			return 1;
		}
	}
	return 0;
}


/**
 * @return the modifier mask of the given key
 */
unsigned int get_mod_mask(key_code key) {
	int i = 0, j;
	unsigned int bits;
	for (bits = 1 << (LENGTH(mods) - 1); bits; bits >>= 1) {
		for (j = 0; j < LENGTH(mods[i]); j++) {
			if (mods[i][j] == key) return bits;
		}
		i++;
	}
	return 0;
}


/**
 * React to a key event
 *
 * @return whether the event was a hotkey (and should be ignored)
 */
int handle_event(struct input_event input) {
	if (input.type == EV_MSC && input.code == MSC_SCAN) return 0;

	/* forward anything that is not a key event, including SYNs */
	if (input.type != EV_KEY) return 0;

	/* handle keys */
	unsigned int mod_mask;
	switch (input.value) {
		case INPUT_VAL_PRESS:
			last_press = input.code;
		case INPUT_VAL_REPEAT:
			if ((mod_mask = get_mod_mask(input.code))
			 || !try_hotkey(input.code)) {
				mod_state |= mod_mask;
				return 0;
			}
			return 1;
		case INPUT_VAL_RELEASE:
			if ((mod_mask = get_mod_mask(input.code))) {
				mod_state ^= mod_mask;
				if (last_press == input.code) try_hotkey(input.code);
			}
			return 0;
		default:
			fprintf(stderr, "unexpected .value=%d .code=%d, doing nothing",
					input.value, input.code);
			return 0;
	}
}
