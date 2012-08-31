FuseFM Arduino Firmware
=======================

This repo uses submodules. either use the `--recursive` flag for `git clone`, or run:

```
git submodule update --init
```

after cloning.

Building
--------

You will need:

- Scons
- The very latest arduino software and compiler. The `arduino-git` AUR package in Arch is known to work.

Run something like the following to build and upload in one go:

```
scons upload
```

Omit the `upload` to just build.

Hacking
-------

`arduino/arduino.cpp` is the file you want.

Use https://github.com/18sg/SHETSource/blob/master/arduino/client/client_example.cpp for reference.

Running
-------

Assuming SHET is set up properly, run:

```
./SHETSource/router/directrouter.py /dev/whatever
```
