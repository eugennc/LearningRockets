#ifndef __SCENEOBJECT_H__
#define __SCENEOBJECT_H__

#include "dataStructures.h"
#include "oglDataCache.h"
#include <string>
#include <vector>


namespace SdimpleRocket {
  //variable names may depart from being long and precise in the traditional sense; this is not in order to shorten them, but in order to make them _even more precise_: the short names used here are long-standing physics conventions, with precise meanings. Still, outside of physics computations (like get/set), long names will be used


struct Light {
  GLuint lightId;
  GLfloat amb[4], dif[4], spe[4];
  Vector3 offset;//where the object bounding sphere has radius 1
};

struct State {
  Vector3 r; //position vector to the center of the object, meters
  Vector3 v; //velocity, meters / second
};

struct Derivative { //the state derivative
  Vector3 dr; // dr / dt  velocity
  Vector3 dv; // dv / dt  acceleration m/s^2
};

class SceneObject {
 protected:
  DataObject m_object; //the actual 3d model and textures
  //3d model-related
  Vector3 m_glCenterOffset; // Center offset or object translation. OpenGL units!!! Should generally be (0, 0, 0), and used only if the (0, 0, 0) point of the 3d model is not the center of mass
  Vector3 m_glScale; // object scale; not actual vector, but scale factors on each dimension. Usually, a 3d model should have its longest dimension length (along an axis) as 1 (inscribed in a radius 1 sphere).
  Vector3 m_glRotationAxis; // rotation axis for rotation offset
  Float m_glRotationAngle;  // object rotation around the rotation axis. The rotation is not stored as a vector having a direction and a vector length (for rotational angle) for clarity. Measured in pi-radians. Describes the initial orientation of the model

  //physics related
  Float m_size; // The dimension / length multiplier for the object. This, multiplied with the dimensions of the 3d model, yields the 'real-size' object. meters.
  Float m_m; //object mass, kg
  Float m_h; //rotation around object origin, in pi-radians
  Float m_w; //object angular velocity, pi-radians/s, only for the depth axis (z)
  State m_state; //this object's position and current velocity

  //ideally, we would be working with the rotational momentum quaternion. For convenience, we skip this, and, instead, use a very simple angular velocity scalar; computing rotation will, likewise, be simple fiat, not accurate simulation.
  //as we compute now, these are secondary properties, entirely computed from primary properties
  Derivative m_derivative; //m_state derivatives, in respect to time
  Vector3 m_p; //momentum vector kg * m / s
  Float m_im; //object inverse mass, kg^{-1}

  Vector3 m_force;//the resultant of all forces acting upon this object

  //engine
  Vector3 m_thrust; //direction and magnitude of the force produced by the engine upon the object center of mass
  bool m_thrustActive;

  void update();
  void updateMass();

 public:
  //light related
  bool m_hasLight; //does this body have a lightsource attached?
  Light m_light; // not enough time to properly implement accessor functions, thus public

  SceneObject(DataObject object, Vector3 position, Float size, Float mass, Vector3 velocity, Float rotation, Float angularVelocity, Vector3 thrust);

  DataObject getDataObject() { return m_object; }
  void setDataObject(DataObject object) { m_object = object; }


  Vector3 getGlCenterOffset() const { return m_glCenterOffset; }
  void setGlCenterOffset(Vector3 glCenterOffset) { m_glCenterOffset = glCenterOffset; }
  Vector3 getGlScale() const { return m_glScale; }
  void setGlScale(Float glScale) { m_glScale = Vector3(glScale , glScale, glScale); }
  Vector3 getGlRotationAxis() const { return m_glRotationAxis; }
  void setGlRotationAxis(Vector3 glRotationalAxis) { m_glRotationAxis = glRotationalAxis; }
  Float getGlRotationAngle() const { return m_glRotationAngle; }
  void setGlRotationAngle(Float glRotationalAngle) { m_glRotationAngle = glRotationalAngle; }
  Float getSize() const { return m_size; }
  void setSize(Float size) { m_size = size; }
  Vector3 getPosition() { return m_state.r; }
  void setPosition(Vector3 position) { m_state.r = position; }
  void translate(Vector3 deltaR) { m_state.r += deltaR; }
  Float getMass() const { return m_m; }
  void setMass(Float mass) { m_m = mass; updateMass(); }
  Vector3 getVelocity() const { return m_state.v; }
  void getVelocity(Vector3 velocity) { m_state.v = velocity; }
  Float getRotation() { return m_h; }
  void setRotation(Float rotation) { m_h = rotation; }
  Float getAngularVelocity() const { return m_w; }
  void setAngularVelocity(Float angularVelocity) { m_w = angularVelocity; }
  Vector3 getDPosition() const { return m_derivative.dr; }
  void setDPosition(Vector3 dPosition) { m_derivative.dr = dPosition; }
  Vector3 getDVelocity() const { return m_derivative.dv; }
  void setDVelocity(Vector3 dVelocity) { m_derivative.dv = dVelocity; }
  State getState() const { return m_state; }
  void setState(State state) { m_state = state; }
  Derivative getDerivative() const { return m_derivative; }
  void setDerivative(Derivative Derivative) { m_derivative = Derivative; }
  Vector3 getForce() const { return m_force; }
  void setForce(const Vector3 &force = Vector3()) { m_force = force; }
  void addForce(const Vector3 &force) { m_force += force; }
  void setAcceleration(const Vector3 &acceleration = Vector3()) { m_derivative.dv = acceleration; }
  void addAcceleration(const Vector3 &acceleration) { m_derivative.dv += acceleration; }


  //physics
  Vector3 linearAcceleration(Vector3 force) const;
  Derivative eval(State s, const Derivative d, Float dt, Vector3 force) const ;
  Derivative evalAccel(State s, const Derivative d, Float dt) const ;
  //move the object affected by a force during a certain time
  void integrateRk4Accel(Float dt);
  void integrateRk4(Float dt, Vector3 force);
  inline void integrate(Float dt, Vector3 force) { integrateRk4(dt, force); }
  inline void integrate(Float dt) { integrate(dt, m_force); }
  inline void integrateAccel(Float dt) { integrateRk4Accel(dt); }
  inline void forceToAccel() { m_derivative.dv += m_force / m_m; }

  bool isThrustActive() const { return m_thrustActive; }
  void applyThrust(); //compute thrust according to object rotation, and add it to the forces acting upon the object. Do not integrate yet.
};


}
#endif
