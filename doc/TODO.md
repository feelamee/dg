- [ ] minimal working app which can build/run on android + linux
  - [x] add android-project from SDL
  - [x] create activity which extends SDLActivity
  - [x] link necessary libs in cmake lists
  - [x] download opengl loader - glad
  - [x] remember how to create opengl context
  - [x] create minimal abstraction for SDL window
  - [x] test on android
  - [x] add BUILD.md with instructions of how to build on linux/android (UPD: in README.md)
  - [x] remove hardcoded android triplet from engine/CMakeLists.txt:20
  - [ ] change default sdl icons
<br>
- [ ] final test task
  - [ ] extract all mentions of SDL/glad from orbi
  - [ ] add obj loader library, or load it by yourself but fix normals
  - [ ] add something like gizmo
&nbsp;   for now we can move around by WASD and rotate camera by mouse,
&nbsp;   when ctrl is pressed, mouse will be unbinded
&nbsp;   in this state we should can choose object by click and after rotate/translate/scale using gizmo 

