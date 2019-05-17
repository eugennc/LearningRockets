#ifndef __PROXYGL_H__
#define __PROXYGL_H__

//this should be the only place the followind declarations take place; this is mainly to allow modifiying a single place, conflicts and errors may or may not occur
//glew is the first GL-related header to include
//try commenting out glu.h and glext.h if you get symbol redefinition errors
#define GLEW_STATIC
#include <GL/glew.h>
//probably redundant, thanks to glew.h
#include <GL/glu.h>
//seemingly not needed
//#define GL_GLEXT_PROTOTYPES
#include <GL/glext.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#endif
