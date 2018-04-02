#ifndef __DATASTRUCTURES_H__
#define __DATASTRUCTURES_H__

#include <vector>
#include "proxyGl.h"
#include <cmath>
#include "vector3.h"
#include "types.h"

namespace SdimpleRocket {

//information related to the drawable screen and its outer window
struct Screen {
  GLsizei w, h;
  GLfloat ratio;
  SDL_Window *window;
  SDL_GLContext context;
  //this will point to a certain address, updated by SDL_PollEvent
  const Uint8 *kbState;
  bool active;
  
  inline void updateRatio() {
    if (h < 1) h = 1;
    ratio = (GLfloat) w / (GLfloat) h;
  }
};
 
 

 
}
#endif
