#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <libevdev/libevdev-uinput.h>
#include <errno.h>
#include <fcntl.h>

#include "hk-relay.h"


_Atomic(unsigned int) mod_state = 0;
_Atomic(key_code) last_press = 0;


void print_usage(const char *program) {
	fprintf(stdout,
	        "\n"
	        "usage: %s [options]\n"
	        "\n"
	        "Signal hkd based on info from libevdev device key events"
	        "\n"
	        "options:\n"
	        " -h        show this message and exit\n",
	        program);
}


void *handle_device(void *path) {
	/* open device */
	struct libevdev *dev = NULL;
	int fd = open((char*)path, O_RDONLY|O_NONBLOCK);
	if (fd < 0) {
		fprintf(stderr, "Error: failed to open device: %s\n", (char*)path);
		exit(EXIT_FAILURE);
	}
	int rc = libevdev_new_from_fd(fd, &dev);
	if (rc < 0) {
		close(fd);
		fprintf(stderr, "Error: failed to init libevdev (%s)\n", strerror(-rc));
		exit(EXIT_FAILURE);
	}

	/* grab device */
	if (libevdev_grab(dev, LIBEVDEV_GRAB) < 0) {
		libevdev_free(dev);
		close(fd);
		fprintf(stderr, "Error: failed to grab device\n");
		exit(EXIT_FAILURE);
	}

	/* create virtual uinput device */
	struct libevdev_uinput *virtual_dev;
	if (libevdev_uinput_create_from_device(dev, LIBEVDEV_UINPUT_OPEN_MANAGED,
	                                       &virtual_dev) < 0) {
		libevdev_grab(dev, LIBEVDEV_UNGRAB);
		libevdev_free(dev);
		close(fd);
		fprintf(stderr, "Error: failed to create virtual uinput device\n");
		exit(EXIT_FAILURE);
	}

	/* relay events to the event handler */
	struct input_event input;
	do {
		rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &input);
		if (rc != 0) continue;

		/* continue if event was handled */
		if (handle_event(input)) continue;

		/* send event */
		if (libevdev_uinput_write_event(virtual_dev, input.type, input.code,
			                            input.value) < 0) {
			fprintf(stderr, "Error: failed to send event\n");
		}
	} while (rc == 1 || rc == 0 || rc == -EAGAIN);

	/* cleanup */
	libevdev_grab(dev, LIBEVDEV_UNGRAB);
	libevdev_free(dev);
	close(fd);

	return NULL;
}


int main(int argc, char *argv[]) {
	/* ensure program is running as root */
	if (geteuid()) {
		fprintf(stderr, "%s: must be run as root user\n", argv[0]);
		exit(EXIT_FAILURE);
	}

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

	/* create thread for each device */
	int count;
	for (count = 0; devices[count] != NULL; count++);
	pthread_t threads[count];

	int i = 0;
	for (i = 0; devices[i] != NULL; i++) {
		if (pthread_create(&threads[i], NULL, handle_device,
		                   (void*)devices[i])) {
			fprintf(stderr, "Error: failed to create thread.");
			exit(EXIT_FAILURE);
		}
	}

	void *status;
	for (i = 0; i < count; i++) {
		if (pthread_join(threads[i], &status)) {
			fprintf(stderr, "Error: failed to join thread.");
			exit(EXIT_FAILURE);
		}
	}

	return EXIT_SUCCESS;
}
