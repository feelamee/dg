DG - Domini Games test task


# Overview

This project is a small game engine developed using SDL and OpenGL.
It features basic movement controls with the WASD keys, allowing players to navigate the environment.
The left shift key enables sprinting for faster movement.
The camera can be rotated using the mouse, providing an immersive experience.
Additionally, the engine includes a lighting system, though it may have some inaccuracies.
Overall, this test assignment showcases fundamental game mechanics and graphics rendering capabilities.

# BUILD


### Linux

install dependencies:
```sh
pacman -S cmake mesa ninja clang
```
NOTE: you need C++ compiler which support C++20

from root project folder:
```sh
  cmake -S orbi/ -B build -G Ninja
  ninja -C build/
```

additional options: `-DDG_ENGINE_TEST=ON/OFF`,  `-DDG_ENGINE_SANITIZER=ON/OFF`, `-DDG_ORBI_SANITIZER=ON/OFF`, `-DDG_ENGINE_PEDANTIC=ON/OFF`, `-DDG_ORBI_PEDANTIC=ON/OFF`

run with:
```sh
./build/orbi
```


### Android

move to android-project:
```sh
  cd orbi/android-project
```

firstly you need to install NDK.
I recommend to install cmdline-tools and gradle will do the rest for us.
[Here](https://developer.android.com/tools/sdkmanager) is official documentation

Also add `local.properties` file in android-project with path to android-sdk.
Something like that for my pc.
```sh
  sdk.dir=/home/missed/code/dg/orbi/android-project/android-sdk
````

Carefully read and accept licenses..
Or just `yes | ./android-sdk/cmdline-tools/latest/bin/sdkmanager --licenses` if you dont like all this lawyer nonsense.

After connect you phone and just run:
```sh
  ./gradlew installDebug
```


### Windows

Isn't tested, but also should work with minor fixes.
How to configure environment on I will leave to Windows lovers :)


# Known issues

- ambient and diffuse lightning implemented, but works with glitches
  in `orbi` due to incorrect loading of normals from .obj file
- memory leak
  on my machine this is always 3730 bytes. It reproduces even on simple example with `SDL_Init` and `SDL_Quit`.
  So, maybe SDL just leave memory unfreed, because up althrough will exit.
  On other SDL backend behaviour may differ.
