# Scoot Hard Wolf4SDL (v1.0) Release

Here is the source code to Scoot Hard for porting to other platforms.

I don't know if Linux porting will work out of the box though.

I have included the default Scoot Hard files (.RF6) for people to test with. This
will be found in the directory "Output". Copy these to the output directory as-is.

I cannot release the toolchain at this time that I used to
compile the RF6 files though because it involves the original Wolfenstein 3-D files.
That said, eventually I will release as much as I can.

## Compiling on Windows, Linux and OS X

You're going to need the libraries:

* libSDL 1.2.15
* libSDL-mixer compatiable with libSDL 1.2.15

on both OS X and Windows

### OS X caveats

TODO: Fix these caveats.

OS X may bicker when you clone and try to compile about not being
able to find certain files, specifically the RF6 files.
On my system, they were found in the directory as shown here. Blame XCode for being
finnicky about it.

To compile you will need XCode and the command line tools and OS X, naturally.

### Linux caveats

TODO: fix the caveats

The makefiles might need fixing to include the sound library files in 'Source/asound"

### Windows caveats

Don't do a debug build and distribute it. Playtesters had problems on Windows when I did
debug builds, saying missing libraries existed. I don't know if this is a Wolf4SDL issue or not.

## What is Scoot Hard?

Scoot Hard is a source-code mod of Wolfenstein 3D.
There are some bugs and probably some things that I can do better.