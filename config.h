#include <linux/input-event-codes.h>
#include <linux/input.h>
#include <stdio.h>

#define LENGTH(X) sizeof X / sizeof X[0]

struct binding {
	const int key;
	const void *cmd;
};

static const char *term[] = { "sh", "-c", "$TERMINAL", NULL };
static const char *vol_up[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "+2%", NULL };
static const char *vol_down[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-2%", NULL };

static const struct binding bindings[] = {
	{KEY_BACKSLASH, term},
	{KEY_VOLUMEDOWN, vol_down},
	{KEY_VOLUMEUP, vol_up}
};
