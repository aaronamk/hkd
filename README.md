## DESCRIPTION
A key mapper program.

Features:
* Map a key plus any number of modifiers to a command
* Map a modifier release to a command
* Configure which keys to use as modifiers
* Works in Xorg, Wayland, and the TTY (using [libevdev](https://www.freedesktop.org/software/libevdev/doc/latest/index.html))

## INSTALLATION
### From source
1. Run:
```
git clone https://github.com/aaronamk/hkd.git
cd hkd
sudo make install
```
2. Add yourself to the `input` group: `sudo usermod -a -G input <username>`. This allows hkd to access/modify input devices in the filesystem without root access.
3. Reboot for the change to take affect

## CONFIGURATION
The file `config.h` is where you set your key bindings. This is similar to [suckless](https://suckless.org/philosophy)'s software practices. `config.h` is part of the source, so you must recompile and restart the program for any changes to take effect. Some default bindings are provided; you can add or remove them as needed.

See [linux/input-event-codes.h](https://github.com/torvalds/linux/blob/master/include/uapi/linux/input-event-codes.h) for a list of available key codes or use the [evtest](https://gitlab.freedesktop.org/libevdev/evtest) command to see device events printed to the terminal.

## USAGE
```
hkd /dev/input/by-id/<device-id-1> /dev/input/by-id/<device-id-2> ...
```

## RELATED/SIMILAR SOFTWARE
* [swhkd](https://github.com/waycrate/swhkd)
* [Interception Tools](https://gitlab.com/interception/linux/tools)
---

*NOTICE*: Please check the issues board if you find a bug or would like a feature added. Open a new issue if it is not listed.
