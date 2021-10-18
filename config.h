#include <linux/input-event-codes.h>
#include <stdio.h>


typedef unsigned short key_code;

struct binding {
	const unsigned int mod_mask;
	const key_code key;
	const void *cmd;
};

/* list of devices to respond to */
static const char *devices[] = {
	"/dev/input/by-id/usb-SEMITEK_USB-HID_Gaming_Keyboard_SN0000000001-event-kbd",
	"/dev/input/by-id/usb-SEMITEK_USB-HID_Gaming_Keyboard_SN0000000001-if02-event-mouse",
	NULL
};

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
static const char terminal[] = "st";
static const char *term[] = { "sh", "-c", "$TERMINAL", NULL };
static const char *editor[] = { terminal, "-e", "nvim", NULL };
static const char *files[] = { terminal, "-e", "vifm", NULL };
static const char *launcher[] = { terminal, "-t", "\"launch\"", "-e", "fzf-launch.sh", NULL };
static const char *fzf_open[] = { terminal, "-e", "fzf-open.sh", NULL };
static const char *fzf_open_meta[] = { terminal, "-e", "fzf-open-meta.sh", NULL };
static const char *session[] = { terminal, "-g", "30x15", "-t", "\"session_options\"", "-e", "fzf-session-options.sh", NULL };

static const char *browser[] = { "sh", "-c", "$BROWSER", NULL };
static const char *browser_private[] = { "firefox", "-private-window", NULL };

static const char *vol_mixer[] = { terminal, "-g", "130x30", "-t", "\"audio_mixer\"", "-e", "pulsemixer", NULL };
static const char *vol_up[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "+2%", NULL };
static const char *vol_down[] = { "pactl", "set-sink-volume", "@DEFAULT_SINK@", "-2%", NULL };
static const char *vol_toggle_mute[] = { "pactl", "set-sink-mute", "@DEFAULT_SINK@", "toggle", NULL };

static const char *media_next[] = { "playerctl", "--player=playerctld", "next", NULL };
static const char *media_prev[] = { "playerctl", "--player=playerctld", "previous", NULL };
static const char *media_forward[] = { "playerctl", "--player=playerctld", "position", "5+", NULL };
static const char *media_backward[] = { "playerctl", "--player=playerctld", "position", "5-", NULL };
static const char *media_toggle_pause[] = { "playerctl", "--player=playerctld", "play-pause", NULL };
static const char *music_toggle_pause[] = { "mpc", "toggle", NULL };

static const char *brightness_up[] = { "brightness-ctrl.sh", "+", NULL };
static const char *brightness_down[] = { "brightness-ctrl.sh", "-", NULL };

static const char *screenshot[] = { "screenshot.sh", NULL };


/* bindings */
/* https://www.kernel.org/doc/html/latest/input/event-codes.html */
static const struct binding bindings[] = {
	/* modifier mask         key code             command */
	{ M_NONE,                KEY_HOME,            term },
	{ M_META,                KEY_ENTER,           term },
	{ M_META,                KEY_SEMICOLON,       editor },
	{ M_META,                KEY_E,               files },
	{ M_META|M_SHIFT,        KEY_ENTER,           launcher },
	{ M_META,                KEY_SLASH,           fzf_open },
	{ M_META|M_SHIFT,        KEY_SLASH,           fzf_open_meta },
	{ M_META,                KEY_BACKSPACE,       session },

	{ M_META,                KEY_BACKSLASH,       browser },
	{ M_META|M_SHIFT,        KEY_BACKSLASH,       browser_private },

	{ M_META,                KEY_MUTE,            vol_mixer },
	{ M_NONE,                KEY_VOLUMEUP,        vol_up },
	{ M_NONE,                KEY_VOLUMEDOWN,      vol_down },
	{ M_NONE,                KEY_MUTE,            vol_toggle_mute },

	{ M_NONE,                KEY_NEXTSONG,        media_next },
	{ M_NONE,                KEY_PREVIOUSSONG,    media_prev },
	{ M_SHIFT,               KEY_NEXTSONG,        media_forward },
	{ M_SHIFT,               KEY_PREVIOUSSONG,    media_backward },
	{ M_NONE,                KEY_PLAYPAUSE,       media_toggle_pause },
	{ M_SHIFT,               KEY_PLAYPAUSE,       music_toggle_pause },

	{ M_NONE,                KEY_KPPLUS,          brightness_up },
	{ M_NONE,                KEY_KPMINUS,         brightness_down },
	{ M_NONE,                KEY_BRIGHTNESSUP,    brightness_up },
	{ M_NONE,                KEY_BRIGHTNESSDOWN,  brightness_down },

	{ M_NONE,                KEY_SYSRQ,           screenshot }
};
