*WARNING*: Early in development, please open an issue if you run into a bug

A key mapper program. It's [sxhkd](https://github.com/baskerville/sxhkd) without the reliance on an X server.

## INSTALLATION
#### From source
```shell
git clone https://github.com/aaronamk/hkd.git
cd hkd
sudo make install
```
## CONFIGURATION
#### `config.h`
This configuration file is where you set your key bindings. It works similarly to suckless software. `config.h` is part of the source, so you must recompile and re-run the program for any changes to take effect. Some default bindings are provided, you can add or remove them as needed.

See [https://github.com/torvalds/linux/blob/master/include/uapi/linux/input-event-codes.h](https://github.com/torvalds/linux/blob/master/include/uapi/linux/input-event-codes.h) for a list of available key codes.

## USAGE
```
hkd &
sudo hkd-relayer
```
Note: hkd must be run before hkd-relayer
