#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "proxyGl.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <exception>
#include "types.h"
#include "objLoader.h"
#include "dataStructures.h"
#include "oglDataCache.h"
#include "scene.h"

namespace SdimpleRocket {
  
class Engine {
 protected:
  Screen m_screen1;
  OglDataCache m_cache;
  Scene m_scene;
  //simple example only
  GLuint m_texture[1];
  GLint m_startTime; //measurement start, not program start
  GLint m_currentTime;
  GLint m_frameTime;
  GLint m_frames;
  GLint m_pFrames;
  bool m_thrust;
  //hacks, mostly
  Float m_rotation;
  Float m_rotationIncrease;
  bool m_resetRotation;
  //this object will be input-controlled. It's an index for the n-th object created, starting from 0.
  unsigned int m_controlledObject;
  
  //functions for various SDL and GL-related tasks. constructors and destructors are expected to use them as appropriate.

  //general engine and SDL inits
  bool init();
  //specific OpenGL within SDL inits
  bool initGl();
  //activate lights on scene
  void initLights();
  //close SDL
  void close();
  //handle window-resize events
  void resize(SDL_Event *e);
  //handle key presses
  void handleKeys(SDL_Event *e, int x, int y);
  //render scene on screen
  void render();

  //per-frame updates
  void update();

  //switch between windowed mode, fake-fullscreen (dekstop res) and true fullscreen (at window specified resolution)
  void toggleFullscreen(bool fake = true);
  //shortcut for m_screen1.kbState(SDL_SCANCODE)
  Uint8 inline k(SDL_Scancode scancode) {
    return m_screen1.kbState[scancode];
  }

  void renderObject(DataObject d);
  void renderLight(Light l, bool active);
  void renderUi();

  //multiple scene definitions
  void scene1(); 
  void scene2();
  void scene3();

  
 public:
    Engine(unsigned int screenWidth = 800, unsigned int screenHeight = 600);
    void loadScene(std::string sceneFile);
    ~Engine();
    //run the engine
    //steps == 0 means until the window is closed. Otherwise, run the event loop a limited number of steps.
    void run(unsigned int steps = 0);
};

}
 
#endif //__ENGINE_H__
