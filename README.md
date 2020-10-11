Larry the Dinosaur 3 SDL
------------------------

This is a (pretty basic) port of the unfinished game Larry the Dinosaur 3 to
Linux via [SDL](https://libsdl.org/).

Is it playable?
---------------

Yes, but…

Larry the Dinosaur 3 was never finished, and this version was not polished for
release: there are a number of bugs and some missing files (e.g., sounds).

That being said, it's possible to play through most of the levels, and it's
still quite a bit of fun!


Is this the same as the 'ld3beta2' version?
-------------------------------------------

No, this version contains some content that wasn't in the 'ld3beta2' version,
and vice-versa. The ld3beta2 version is overall more polished, but has fewer
levels.

Also note that there are some changes to the levels, and to the music.

What changed during the port to SDL/Linux?
------------------------------------------

First, a number of non-C++ standards-compliant things were changed to get this
to compile on non-MSVC6 compilers. Old Microsoft compilers, for example, did
not scope the loop variable to a for-loop properly.

Most of the basic Windows API calls were replaced with equivalent SDL calls.
Win32 API types are defined in ``slx_win32types.h``.

In particular, the DirectDraw surfaces were replaces with ``SDL_Surfaces``. A
special blit function (``SLX_BlitFlipSurface()``) is used to work around SDL's
lack of support for flipping surfaces horizontally and vertically. Note also
that ``SDL_BlitSurface()`` is aware of surface palettes, whereas DirectDraw
(and the ``SLX_BlitFlipSurface()`` function) are not, and just preserve
palette indices.

Instead of using page flipping, the SDL version always renders to a
"backbuffer" ``SDL_Surface``, and uploads this to an ``SDL_Texture`` as a
frontbuffer.

For input, Joystick support has been stubbed out (I play with a keyboard). The
DirectInput code was totally removed, and ``SDL_GetKeyboardState()`` is used
for keyboard access. This isn't ideal, but works well-enough for the moment.
SDL Scancodes are used (rather than keycodes) to make this easier, and replace
the ``DIK_`` values throughout the code.

For sound, FMOD has been replaced with ``SDL_Mixer``. It should still be
possible to compile the game against FMOD, if you can find an old enough
version, but such old versions are pretty buggy on modern Linux (and quite
tricky to get hold of).

Finally, all of the files were renamed, as Linux uses a case-sensitive
filesystem.

What's broken?
--------------

There are a quite a few known bugs in LD3SDL.
Here are some highlights (lowlights?):

* The "no move" key is broken, and hence disabled.
* There are no sound effects (the files are missing)
* Level names in saved games may be off-by-one?
* There's no Joystick/Gamepad support.
* There are quite a few hangs/crashes.

How do I compile it?
--------------------

This port is built with CMake, and should be reasonably straightforward to build.

```
mkdir build
cd build
cmake ..
make
cp ld3.x86_64 ..
cd ..
./ld3.x86_64
```

You'll need ``SDL2`` and ``SDL2_mixer`` installed, including the development headers.

(On Debian/Ubuntu: ``sudo apt install libsdl2-dev libsdl2-mixer-dev``

Who owns Larry the Dinosaur 3?
------------------------------

Larry the Dinosaur 3 (and all characters, names, indicia, etc) are owned by
[Joe King](https://github.com/grasshoppersunshine).

The SDL port is by [David Gow](https://davidgow.net/).

