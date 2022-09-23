/**
 * Hotkey Daemon
 *
 * Author: aaronamk
 */

#include "config.h"

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <linux/input.h>
#include <libevdev/libevdev-uinput.h>

#define VERSION "v1.0.0"
#define INPUT_VAL_PRESS 1
#define INPUT_VAL_RELEASE 0
#define INPUT_VAL_REPEAT 2
#define LENGTH(X) sizeof X / sizeof X[0]

_Atomic(unsigned int) mod_state = 0;
_Atomic(key_code) last_press = 0;
_Atomic(int) running = 1;
pthread_t threads[MAX_DEVICES];
int dev_count;


void print_usage(const char *program) {
	printf("Signal hkd based on info from libevdev device key events\n"
	       "\n"
	       "Usage: %s [options] <devices>\n"
	       "\n"
	       "Options:\n"
	       " -h        show this message and exit\n"
	       " -V        print version number and exit\n"
	       "\n"
	       "devices:   space-separated list of device paths"
	                 " (/dev/input/by-...) to be monitored\n",
	       program);
}


void handle_terminate(int signum) {
	running = 0;
	for (int i = 0; i < dev_count; i++) pthread_kill(threads[i], SIGUSR1);
}


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


unsigned int get_mod_mask(key_code key) {
	int i = 0;
	for (unsigned int bits = 1 << (LENGTH(mods) - 1); bits; bits >>= 1) {
		for (int j = 0; j < LENGTH(mods[i]); j++) {
			if (mods[i][j] == key) return bits;
		}
		i++;
	}
	return 0;
}


int try_hotkey(key_code key) {
	for (int i = 0; i < LENGTH(bindings); i++) {
		if (bindings[i].key == key
		 && bindings[i].mod_mask == mod_state) {
			spawn((char**)bindings[i].cmd);
			return 1;
		}
	}
	return 0;
}


int handle_event(struct input_event input) {
	if (input.type == EV_MSC && input.code == MSC_SCAN) return 0;

	/* forward anything that is not a key event */
	if (input.type != EV_KEY) return 0;

	/* handle key events */
	unsigned int mod_mask = get_mod_mask(input.code);
	switch (input.value) {
		case 1: /* key press event */
		case 2: /* key repeat event */
			last_press = input.code;
			mod_state |= mod_mask;
			return !mod_mask && try_hotkey(input.code);
		case 0: /* key release event */
			if (mod_mask) {
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


void nop(int signum) {}
void *handle_device(void *path) {
	struct sigaction ignore;
	ignore.sa_flags = 0;
	ignore.sa_handler = nop;
	sigaction(SIGUSR1, &ignore, NULL);
	/* open device file */
	int fd = open((char*)path, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Error: failed to open device: %s\n", (char*)path);
		return NULL;
	}

	/* open device */
	struct libevdev *dev;
	int rc = libevdev_new_from_fd(fd, &dev);
	if (rc < 0) {
		close(fd);
		fprintf(stderr, "Error: failed to init libevdev (%s)\n",
		                strerror(-rc));
		return NULL;
	}

	/* grab device */
	if (libevdev_grab(dev, LIBEVDEV_GRAB) < 0) {
		libevdev_free(dev);
		close(fd);
		fprintf(stderr, "Error: failed to grab device\n");
		return NULL;
	}

	/* create virtual uinput device */
	struct libevdev_uinput *virtual_dev;
	if (libevdev_uinput_create_from_device(dev, LIBEVDEV_UINPUT_OPEN_MANAGED,
	                                       &virtual_dev) < 0) {
		libevdev_grab(dev, LIBEVDEV_UNGRAB);
		libevdev_free(dev);
		close(fd);
		fprintf(stderr, "Error: failed to create virtual uinput device\n");
		return NULL;
	}

	/* handle key events */
	struct input_event input = {};
	do {
		rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL
		                            | LIBEVDEV_READ_FLAG_BLOCKING, &input);
		if (rc != 0) continue;

		/* continue if event was handled */
		if (handle_event(input)) continue;

		/* forward events not associated with a binding */
		if (libevdev_uinput_write_event(virtual_dev, input.type, input.code,
		                                input.value) < 0) {
			fprintf(stderr, "Error: failed to send event\n");
		}
	} while (running && (rc == 1 || rc == 0 || rc == -EAGAIN));

	/* cleanup */
	libevdev_grab(dev, LIBEVDEV_UNGRAB);
	libevdev_free(dev);
	libevdev_uinput_destroy(virtual_dev); // closes fd

	return EXIT_SUCCESS;
}


int main(int argc, char *argv[]) {
	/* parse options */
	int opt;
	while ((opt = getopt(argc, argv, ":hV")) != -1) {
		switch (opt) {
			case 'h':
				print_usage(argv[0]);
				exit(EXIT_SUCCESS);
			case 'V':
				printf("%s %s\n", argv[0], VERSION);
				exit(EXIT_SUCCESS);
			case '?':
				fprintf(stderr, "Error: invalid option: %c\n", optopt);
				print_usage(argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	/* validate number of devices */
	dev_count = argc - optind;
	if (!dev_count) {
		fprintf(stderr, "Error: device path not specified\n");
		print_usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	if (dev_count > MAX_DEVICES) {
		fprintf(stderr, "Error: Exceeded MAX_DEVICES (increase the value in "
		                "config.h)\n");
		exit(EXIT_FAILURE);
	}

	/* catch termination signals to exit gracefully */
	struct sigaction terminate;
	terminate.sa_flags = 0;
	terminate.sa_handler = handle_terminate;
	sigaction(SIGINT, &terminate, NULL);
	sigaction(SIGTERM, &terminate, NULL);
	sigaction(SIGHUP, &terminate, NULL);

	/* create thread for each device */
	for (int i = 0; i < dev_count; i++) {
		if (pthread_create(&threads[i], NULL, handle_device,
		                   (void*)argv[optind + i])) {
			fprintf(stderr, "Error: failed to create thread.");
			exit(EXIT_FAILURE);
		}
	}

	pthread_exit(EXIT_SUCCESS);
}
