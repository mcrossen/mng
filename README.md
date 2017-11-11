# MNG
*The Offical MNG Repository*

MNG stands for Mng's Not Gnu. It is a simple kernel I developed during a 6 hour
hackathon.

## Building
1. get the right cross compiler.
1. type the following:

```
make
```

## Installation
1. Setup your sd card. The easiest way to do this is install raspbian as normal.
Once it is all setup, you should see a
[few files](https://github.com/raspberrypi/firmware/tree/master/boot) in the
'boot' partition

1. Copy kernel.img from the output directory to the 'boot' partition. Rename it
'kernel8.img'

## Debugging
After building, type the following to launch MNG in QEMU and attach a debugger:
```
make debug
```
