## DESCRIPTION
A key mapper program.

Features:
* Map a key plus any number of modifiers to a command
* Map a modifier release to a command
* Configure which keys to use as modifiers
* Works in Xorg, Wayland, and the TTY (using [libevdev](https://www.freedesktop.org/software/libevdev/doc/latest/index.html))
* Usable on its own or as a plugin for [Interception Tools](https://gitlab.com/interception/linux/tools).

## INSTALLATION
### From source
Run:
```shell
git clone https://github.com/aaronamk/hkd.git
cd hkd
sudo make install
```
## CONFIGURATION
The file `config.h` is where you set your key bindings. It works similarly to suckless software. `config.h` is part of the source, so you must recompile and restart the program for any changes to take effect. Some default bindings are provided, you can add or remove them as needed.

See [linux/input-event-codes.h](https://github.com/torvalds/linux/blob/master/include/uapi/linux/input-event-codes.h) for a list of available key codes.

## USAGE
### Standalone
Run:
```shell
hkd &
sudo hkd-relay
```

### As an Interception Tools plugin
1. Install Interception Tools
2. Create the following configuration file (I recommend placing it in the directory `$XDG_CONFIG_HOME/udevmon`):

`config.yaml`
```yaml
- JOB: "intercept -g $DEVNODE | hk-relayer-plugin | uinput -d $DEVNODE"
  DEVICE:
    EVENTS:
      EV_KEY: [KEY_A] # matches all devices with an "A" key
```


3. Run:
```shell
hkd &
sudo udevmon -c <path-to-file>/config.yaml
```

## RELATED/SIMILAR SOFTWARE
* [sxhkd](https://github.com/baskerville/sxhkd)
* [Interception Tools](https://gitlab.com/interception/linux/tools)
---

*NOTICE*: Please open an issue if you find a bug
