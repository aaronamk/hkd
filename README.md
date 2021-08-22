*WARNING*: In construction and does not work yet

## DESCRIPTION
A plugin for [interception-tools](https://gitlab.com/interception/linux/tools). It's [sxhkd](https://github.com/baskerville/sxhkd) without the reliance on an X server.

## INSTALLATION
TBD

## CONFIGURATION
You must create the following two configuration files:
#### `config.yaml`
```yaml
- JOB: "intercept -g $DEVNODE | hkd-receiver | uinput -d $DEVNODE"
  DEVICE:
    NAME: "<NAME>"
```
Determine the keyboard name *\<NAME\>* with the command `sudo uinput -p -d /dev/input/by-id/<file>`. I keep this file in the directory `$XDG_CONFIG_HOME/udevmon`.

See [interception-tools](https://gitlab.com/interception/linux/tools) for more information on this configuration file.
#### `$XDG_CONFIG_HOME/hkd/config`
```
TBD
```
## USAGE
```
hkd &
sudo udevmon -c <path-to-file>/config.yaml
```
