#include <linux/input-event-codes.h>
#include <stdio.h>
#include <linux/input.h>

#define LENGTH(X) sizeof X / sizeof X[0]

struct binding {
	const unsigned int mods : 8; /* this value can be changed to accommodate a different number of modifier keys */
	const int key;
	const void *cmd;
};

/* any key can be used as a modifier (e.g. caps lock), these are just the most common ones: */
static const int mod_keys[] = { KEY_LEFTSHIFT, KEY_RIGHTSHIFT, KEY_LEFTALT, KEY_RIGHTALT, KEY_LEFTMETA, KEY_RIGHTMETA, KEY_LEFTCTRL, KEY_RIGHTCTRL };

/* masks */
#define M_NONE  0b00000000
#define M_SHIFT 0b11000000
#define M_ALT   0b00110000
#define M_META  0b00001100
#define M_CTRL  0b00000011

/* commands */
static const char *shutdown[] = { "shutdown", "now", NULL };
static const char *vol_up[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "+2%", NULL };
static const char *vol_down[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-2%", NULL };
static const char *vol_toggle_mute[] = { "pactl", "set-sink-mute", "@DEFAULT_SINK@", "toggle", NULL };

/* To use envoronment variables or run scripts, add "sh -c" */
static const char *term[] = { "sh", "-c", "$TERMINAL", NULL };
/* static const char *shell_script[] = { "sh", "-c", "my_shell_script.sh", NULL }; */

/* bindings */
static const struct binding bindings[] = {
	/* modifiers            key                 command */
	{M_CTRL|M_ALT,          KEY_DELETE,         shutdown},
	{M_NONE,                KEY_VOLUMEUP,       vol_up},
	{M_NONE,                KEY_VOLUMEDOWN,     vol_down},
	{M_NONE,                KEY_MUTE,           vol_toggle_mute},
	{M_META,                KEY_ENTER,          term},
};
