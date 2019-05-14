#include "sceneObject.h"

using namespace std;
using namespace SdimpleRocket;

SceneObject::SceneObject(DataObject object, Vector3 position = Vector3(0, 0, 0), Float size = 1, Float mass = 1, Vector3 velocity = Vector3(0, 0, 0), Float rotation = 0, Float angularVelocity = 0, Vector3 thrust = Vector3()) {
  m_object = object;
  m_size = size;
  m_m = mass;
  m_state.r = position;
  m_state.v = velocity;
  m_h = rotation;
  m_w = angularVelocity;
  m_derivative.dr = 0;
  m_derivative.dv = 0;
  
  m_glCenterOffset = Vector3(0, 0, 0);
  m_glScale = Vector3(1, 1, 1);
  m_glRotationAxis = Vector3(0, 0, 0);
  m_glRotationAngle = 0;

  m_thrust = thrust;
  m_thrustActive = false;
  if(m_thrust.getNorm())
    m_thrustActive = true;
  update();
}

void SceneObject::updateMass() {
  m_p = m_state.v * m_m;
  m_im = pow(m_m, -1);
}

void SceneObject::update() {
  updateMass();
}

Vector3 SceneObject::linearAcceleration(Vector3 force) const {
  // F = m * a; a = F / m
  return force / m_m;
}

Derivative SceneObject::eval(State s, Derivative d, Float dt, Vector3 force) const {
  State n;
  Derivative out;
  //n.r = s.r + d.dr * dt; //update position: old position + velocity * time. Not needed for this computation.
  n.v = s.v + d.dv * dt; //update velocity: old vel + accel * time

  out.dr = n.v; //obviously, since dr/t is the velocity
  out.dv = linearAcceleration(force); //the instant total acceleration is equal to the instant total force divided by mass
  return out;
}

Derivative SceneObject::evalAccel(State s, Derivative d, Float dt) const {
  State n;
  Derivative out;
  //n.r = s.r + d.dr * dt; //update position: old position + velocity * time. Not needed for this computation.
  n.v = s.v + d.dv * dt; //update velocity: old vel + accel * time

  out.dr = n.v; //obviously, since dr/t is the velocity
  out.dv = m_derivative.dv;
  return out;
}

void SceneObject::integrateRk4(Float dt, Vector3 force) {
  Derivative a, b, c, d;
  a = eval(m_state, m_derivative, 0, force);
  b = eval(m_state, a, dt * 0.5, force);
  c = eval(m_state, b, dt * 0.5, force);
  d = eval(m_state, c, dt, force);
  //weighted average of multiple points across the time interval [0, dt]
  Vector3 drdt = (a.dr + Float(2.0) * (b.dr + c.dr) + d.dr) / Float(6.0);
  Vector3 dvdt = (a.dv + Float(2.0) * (b.dv + c.dv) + d.dv) / Float(6.0);
  //now that we have a good estimate of velocity and acceleration, use them to update position and velocity.
  m_state.r += drdt * dt;
  m_state.v += dvdt * dt;
}

void SceneObject::integrateRk4Accel(Float dt) {
  Derivative a, b, c, d;
  a = evalAccel(m_state, m_derivative, 0);
  b = evalAccel(m_state, a, dt * 0.5);
  c = evalAccel(m_state, b, dt * 0.5);
  d = evalAccel(m_state, c, dt);
  //weighted average of multiple points across the time interval [0, dt]
  Vector3 drdt = (a.dr + Float(2.0) * (b.dr + c.dr) + d.dr) / Float(6.0);
  Vector3 dvdt = (a.dv + Float(2.0) * (b.dv + c.dv) + d.dv) / Float(6.0);
  //now that we have a good estimate of velocity and acceleration, use them to update position and velocity.
  m_state.r += drdt * dt;
  m_state.v += dvdt * dt;
}


void SceneObject::applyThrust() {
  if(isThrustActive()) {
    //we apply only z-axis rotation:
    Vector3 t = m_thrust;
    t.setX(m_thrust.getX() * cos(m_h) - m_thrust.getY() * sin(m_h));
    t.setY(m_thrust.getX() * sin(m_h) + m_thrust.getY() * cos(m_h));
    /*    cout<<"Thrust: ";
    t.print();
    cout<<"thrust magnitude: "<<t.getNorm()<<endl;
    */

    addForce(t);
  }
}
