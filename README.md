*WARNING*: In construction and does not work yet

# Description
A plugin for [interception-tools](https://gitlab.com/interception/linux/tools). It's [sxhkd](https://github.com/baskerville/sxhkd) without the reliance on an X server.

# Installation
TBD

# Configuration
### udevmon
Determine the keyboard name *\<NAME\>* with the command `sudo uinput -p -d /dev/input/by-id/<file>`. Then, create the following file (perhaps in the directory `$XDG_CONFIG_HOME/udevmon`):

`config.yaml`
```yaml
- JOB: "intercept -g $DEVNODE | hkd-receiver | uinput -d $DEVNODE"
  DEVICE:
    NAME: "<NAME>"
```
### $XDG_CONFIG_HOME/hkd/config
TBD

# Usage
```
hkd &
sudo udevmon -c <path-to-file>/config.yaml
```
