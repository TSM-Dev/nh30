Written by Contsantine "NanoCat" Shablya.
I hereby place this project and its code in the public domain.

Special thanks to
Leystryku
styles
Willox
Paradoxxx
kelse
LuaStoned
blue kirby
everybody else i forgot to put here

FYI: you will not be able to compile the cheat due to lack of compile
scripts, so the code only solves as help to copypaste/learn/whatever.

Check INDEX.txt to find out what files of cheat mean.

Videos:
http://youtu.be/gPmfBLijMrM
http://youtu.be/xi_4UAxdOo8
http://youtu.be/IYHW1cTmZhw
http://youtu.be/XB9x3lNkwB4
http://youtu.be/zs5TCdsl_AY

Hi! You are looking at the README for NanoHack version 3.0 source. If you
expect to see some cool functionality please check NanoHack 2.1 source
instead. If not, play enjoy your stay. This is what i've began to work at
in winter 2014 but never finished. This was supposed to be a complete
rework of my cheat with use of skills i gained over time. Here is a list of
features you probably wouldn't find in many cheats out there (until the
date this was released) and what makes this cheat special:
 Cheat:
  . Fixed prediction errors on speedhack
    This one wasn't quite worked out, because recoil apparently is still
    fucked up, so don't expect much from this.

  . a new, unique way to draw stuff on screen
    cheat hooks into EngineVGui::Paint and draws stuff during the call
    This was found by styles

 Internals:
  . use of GCC to compile cheat with special parameters
    this would let me bring size of cheat down to 27 kilobytes while still
    not having to do anything special.

  . compile script compiles separate DLL for each game unlike it was single
    DLL for all games in my older cheat, v2.1

  . cheat could easily be made universal for various games and/or engine
    versions.

  . custom compiler script with custom preprocessor
    which allowed me to implement small JIT generator in my cheat in a more
    easy way, would let me make my SDK look nice and slim, let me use IDA
    signatures very easily and compress signatures and do a lot of other
    things.
    Parts of compile script are lmake.lua and build.txt

  . small JIT generator
    for interfaces would improve performance of calls to engine interfaces
    i.e. virtual IEngine::GetScreenSize() call  gets turned into more
    simple CALL [] instruction. JIT info is pregenerated at compile time
    by compile script.
    Files JIT "eats" are src/interfaces.txt and src/datatable.txt
    After precompiler script is done, code is generated at the end of
    src/sdk.cpp (lmake/sdk.cpp) and src/sdk.hpp (lmake/sdk.hpp)

  . use of special format for signatures
    cheat uses format for signatures you probably know as "IDA Style", where
    bytes are written down as hex and separated with spaces. Other than
    that, you can also put a . (dot) before a byte and that will make
    sig finder return offseted address.
    Source code for sigfinder can be found in src/util.cpp:3
    Source code for sig processor can be found in build.txt:202

  . compilation/compression of signatures
    is performed by preprocessor and allows to decrease size of sig,
    simplify sigfinder and improve its runtime performance. Compiled sig
    consists of: 1st byte is offset (dot). If there was no offset in sig,
    it's set to 0. 4 bytes of mask (written in little endian). Instead of
    string mask (i.e. "x????xxx") my sigs use bit mask which other than
    minimizing size usage also determines length of a sig. Though this
    also implies limitation of sig length up to 32 bytes. After offset
    and mask, there go known bytes of sig.
    Source code for sigfinder can be found in src/util.cpp:3
    Source code for sig processor can be found in build.txt:202

  . statically linked various imported engine functions.

 Other common things and notes:
  . use of so-called "engine prediction"
  . use of "engine prediction" doesn't fuck up when stepping into
    water in Left4Dead2 - Thanks to styles for this

  . well optimized aimbot lookup algorithm
  . nicely looking dynamic ESP boxes
