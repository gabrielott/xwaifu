# xwaifu

xwaifu draws an image overlay above all other windows,
making it visible at all times.
It's pretty much just like [Xteddy](http://weber.itn.liu.se/~stegu/xteddy/xteddy_info.html),
but with a few additional features.
Useful to decorate your desktop or
when you need to reference an image while working.

## Features
* Set image location and size
* Set image translucency
* Option to make image disappear when hovered over
* Image can be clicked through, so that windows below the image can still be interacted with
* Support for Extended Window Manager Hints

## Screenshots
![xwaifu](https://gabrielott.github.io/xwaifu/img/gif1.gif)

## Usage
    usage: xwaifu [-fhrR] [-a ALPHA] [-g GEOMETRY] image_file
    -a ALPHA    set image translucency
    -f          hide image when hovered over
    -g GEOMETRY set window position and/or size
    -h          print this message
    -r          set image width automatically
    -R          set image height automatically

For a detailed description of each option and a couple examples,
please read the [manual](https://gabrielott.github.io/xwaifu/xwaifu.1.html).

## Dependencies
* X11
* Imlib2

## Installation
1. Make sure you have X11 and Imlib2 installed.
2. Clone the repository with `git clone https://github.com/gabrielott/xwaifu`
3. Compile and install with `make install`.

Installing requires root privileges,
so you may have to use `sudo`.
The executable will be located at /usr/local/bin/xwaifu;
if you wish to do so,
this can be changed by setting the PREFIX variable to something else when calling `make`.

xwaifu can be uninstalled with `make uninstall`.
