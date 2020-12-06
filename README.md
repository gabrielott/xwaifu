# xwaifu

xwaifu draws an image overlay above all other windows,
making it visible at all times.
It's pretty much just like [Xteddy](http://weber.itn.liu.se/~stegu/xteddy/xteddy_info.html),
but with a few additional features.

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

## Dependencies
* X11
* Imlib2
