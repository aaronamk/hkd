#include <linux/input-event-codes.h>
#include <stdio.h>
#include <linux/input.h>

#define LENGTH(X) sizeof X / sizeof X[0]

struct binding {
	const unsigned int mods;
	const unsigned int key;
	const void *cmd;
};

/* any key can be used as a modifier (e.g. caps lock), these are just the most common ones: */
static const int mods[] = { KEY_LEFTSHIFT, KEY_RIGHTSHIFT, KEY_LEFTALT, KEY_RIGHTALT, KEY_LEFTMETA, KEY_RIGHTMETA, KEY_LEFTCTRL, KEY_RIGHTCTRL };

/* masks */
#define M_NONE  0
#define M_SHIFT 0b11000000
#define M_ALT   0b110000
#define M_META  0b1100
#define M_CTRL  0b11

/* commands */
static const char *shutdown[] = { "shutdown", "now", NULL };

/* requires pulseaudio */
static const char *vol_up[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "+2%", NULL };
static const char *vol_down[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-2%", NULL };
static const char *vol_toggle_mute[] = { "pactl", "set-sink-mute", "@DEFAULT_SINK@", "toggle", NULL };

/* requires playerctl */
static const char *media_next[] = { "playerctl", "--player=playerctld", "next", NULL };
static const char *media_prev[] = { "playerctl", "--player=playerctld", "previous", NULL };
static const char *media_forward[] = { "playerctl", "--player=playerctld", "position", "5+", NULL };
static const char *media_backward[] = { "playerctl", "--player=playerctld", "position", "5-", NULL };
static const char *media_toggle_pause[] = { "playerctl", "--player=playerctld", "play-pause", NULL };

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
	{M_NONE,                KEY_NEXTSONG,       media_next},
	{M_NONE,                KEY_PREVIOUSSONG,   media_prev},
	{M_SHIFT,               KEY_NEXTSONG,       media_forward},
	{M_SHIFT,               KEY_PREVIOUSSONG,   media_backward},
	{M_NONE,                KEY_PLAYPAUSE,      media_toggle_pause},
	{M_NONE,                KEY_MUTE,           vol_toggle_mute},
	{M_META,                KEY_ENTER,          term},
};
