# xkbmon - command-line keyboard layout monitor for X11

[![Build Status](https://travis-ci.org/xkbmon/xkbmon.svg)](https://travis-ci.org/xkbmon/xkbmon)

## Table of Contents

<!-- vim-markdown-toc GFM -->

* [Features](#features)
* [Examples](#examples)
  - [tmux](#tmux)
  - [Tint2](#tint2)
  - [dwm](#dwm)
  - [i3 with i3blocks](#i3-with-i3blocks)
* [Build Instructions](#build-instructions)
  - [DEB-based distro](#deb-based-distro)
  - [RPM-based distro](#rpm-based-distro)
  - [Build & Run](#build--run)
* [License](#license)

<!-- vim-markdown-toc -->

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

### dwm

![dwm status](/screenshots/dwm.png?raw=true)

Add to `.xsession`

```
xkbmon -u | while read -r line; do xsetroot -name $line; done &
```

### i3 with i3blocks

![i3 i3blocks](/screenshots/i3.png?raw=true)

Add to `i3blocks.conf`:

```
[xkbmon]
command=xkbmon -u
interval=persist
color=#2E9EF4
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
