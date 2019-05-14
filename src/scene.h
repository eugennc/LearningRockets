#ifndef __SCENE_H__
#define __SCENE_H__
#include "dataStructures.h"
#include <iostream>
#include <string>
#include <vector>
#include "sceneObject.h"

namespace SdimpleRocket{


class Scene {
 protected:
  std::vector< SceneObject > m_objects;
  Float m_time, m_dt; //unit: second; old time and time difference to get in the 'present'
  Float m_mulDt; //time difference, multiplied
  Float m_timeMul; //time multiplier, for faster / slower simulation
  Float m_frameTime; //the length of the simulation time-step
  //virtual camera-related
  Float m_scale;
  Vector3 m_offset;
  //gravitational constant, N * m^2 * kg^-2
  static const Float k_G = 0.000000000066740831;

  //center and scale out objects so all the scene has a certain length, so it fits in the view volume
  void updateView(Float maxLength = 2);
  void updateRotation();
  //compute the gravitational force acting upon the local object from a distant object. The force point of application is the center of the local object, and is directed towards the distant. This assumes point masses.
  Vector3 gravity(SceneObject local, SceneObject distant);
  //updatePhysics looks up current m_dt, no need for an argument
  void updatePhysics(bool thrusting);

 public:
  Scene(Float frameTime = 0.1); //maxFrameTime given in seconds here.

  void addObject(SceneObject sceneObject);
  void addObject(DataObject object, Vector3 position = Vector3(0, 0, 0), Float size = 1, Float mass = 1, Vector3 velocity = Vector3(0, 0, 0), Float rotation = 0, Float angularVelocity = 0, Vector3 thrust = Vector3());

  std::vector< SceneObject >::iterator begin();
  std::vector< SceneObject >::iterator end();

  //use this before every frame
  int nextFrame(Float dt, bool thrusting = 0);

  //concievably usable by the engine, thus public. Questionable decision. TODO: remove or provide usecase.
  void updateTime(Float dt);
  void resetTime(Float time = 0);

  size_t getObjectCount() { return m_objects.size(); }
  
  Float getTimeMul() { return m_timeMul; }
  void setTimeMul(Float timeMul) { m_timeMul = timeMul; }
  Float getScale() { return m_scale; }
  void setScale(Float scale) { m_scale = scale; }
  Vector3 getOffset() { return m_offset; }
  void setOffset(Vector3 offset) { m_offset = offset; }

  std::pair<Float, Float> getApPe(unsigned int object);
  
};

}
#endif
