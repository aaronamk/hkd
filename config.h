/**
 * Hotkey Daemon sample config.h
 *
 * Author: aaronamk
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <linux/input-event-codes.h>
#include <stdio.h>

typedef unsigned short key_code;

struct binding {
	const unsigned int mod_mask;
	const key_code key;
	const void *cmd;
};

/* Maximum number of keyboards to allow via the command line */
#define MAX_DEVICES 32

/* any key can be used as a modifier (e.g. caps lock), these are just the most common ones: */
static const key_code mods[][2] = { { KEY_LEFTSHIFT, KEY_RIGHTSHIFT },
                                    { KEY_LEFTALT,   KEY_RIGHTALT },
                                    { KEY_LEFTMETA,  KEY_RIGHTMETA },
                                    { KEY_LEFTCTRL,  KEY_RIGHTCTRL } };

/* masks */
#define M_NONE  0
#define M_SHIFT 0b1000
#define M_ALT   0b0100
#define M_META  0b0010
#define M_CTRL  0b0001


/* commands */
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

/* requires a $TERMINAL environment variable set to your preferred terminal */
static const char *term[] = { "sh", "-c", "$TERMINAL", NULL };

/* requires dmenu */
static const char *launcher[] = { "dmenu_run", NULL };

static const char *shutdown[] = { "shutdown", "now", NULL };

/* bindings */
/* https://www.kernel.org/doc/html/latest/input/event-codes.html */
static const struct binding bindings[] = {
	/* modifier mask         key code             command */
	{ M_NONE,                KEY_VOLUMEUP,        vol_up },
	{ M_NONE,                KEY_VOLUMEDOWN,      vol_down },
	{ M_NONE,                KEY_MUTE,            vol_toggle_mute },

	{ M_NONE,                KEY_NEXTSONG,        media_next },
	{ M_NONE,                KEY_PREVIOUSSONG,    media_prev },
	{ M_SHIFT,               KEY_NEXTSONG,        media_forward },
	{ M_SHIFT,               KEY_PREVIOUSSONG,    media_backward },
	{ M_NONE,                KEY_PLAYPAUSE,       media_toggle_pause },

	{ M_META,                KEY_ENTER,           term },
	{ M_NONE,                KEY_LEFTMETA,        launcher },
	{ M_NONE,                KEY_RIGHTMETA,       launcher },
	{ M_CTRL|M_ALT,          KEY_DELETE,          shutdown }
};

#endif
