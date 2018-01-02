# xkbmon - command-line keyboard layout monitor for X11

[![Build Status](https://travis-ci.org/xkbmon/xkbmon.svg)](https://travis-ci.org/xkbmon/xkbmon)

## Features

* real-time monitoring with zero CPU usage
* output layout in lower, upper or camel case

## Examples

### tmux

![tmux status panel](/screenshots/tmux.png?raw=true)

Add to `.tmux.conf`:

```
set -g status-right '#[fg=white]#[bg=blue] #(xkbmon -u) '
```

### Tint2

![tint2 executor](/screenshots/tint2.png?raw=true)

Add to `tint2rc`:

```
-------------------------------------
# Executor 1
execp = new
execp_command = xkbmon -u
execp_interval = 1
execp_has_icon = 0
execp_cache_icon = 1
execp_continuous = 1
execp_markup = 0
execp_font = Sans Bold 9
execp_font_color = #dcdcdc 100
execp_padding = 0 0
execp_background_id = 0
execp_centered = 0
```

## Build Instructions

### DEB-based distro

```sh
apt install libx11-dev
```

### RPM-based distro

```sh
yum install libX11-devel
```

### Build & Run

```sh
make
./xkbmon
```

## License
[GPL3](/LICENSE.GPL3)
