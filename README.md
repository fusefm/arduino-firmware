FuseFM Arduino Firmware
=======================

This repo uses submodules. either use the `--recursive` flag for `git clone`, or run:

```
git submodule update --init
```

after cloning. You may need to run

```
git submodule update
```

after pulling to keep them up to date.

Building
--------

You will need:

- Scons
- The very latest arduino software and compiler. The `arduino-git` AUR package in Arch is known to work.

Run something like the following to build and upload in one go:

```
scons ARDUINO_BOARD=mega2560 ARDUINO_HOME=/usr/share/arduino-1.0/ ARDUINO_PORT=/dev/ttyACM0 upload
```

Omit the `upload` to just build.

Hacking
-------

`arduino/arduino.ino` is the file you want.

Use https://github.com/18sg/SHETSource/blob/master/arduino/client/client_example.cpp for reference.

Running
-------

Assuming SHET is set up properly, run:

```
./SHETSource/router/directrouter.py /dev/whatever
```
