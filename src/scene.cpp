#include "scene.h"

using namespace std;
using namespace SdimpleRocket;

//moving constant init in constructor, to be visible
//const Float Scene::k_G = 0.000000000066740831;

Scene::Scene(Float maxFrameTime) {
  resetTime();
  m_maxFrameTime = maxFrameTime * 1000; //millisecond conversion
  m_timeMul = 1;
  m_scale = 1;
  m_offset = Vector3(0, 0, 0);
}

void Scene::resetTime(GLint time) {
  m_time = Float(time) / 1000.0;
  m_dt = 0;
}

void Scene::updateTime(GLint dt) {
  m_time += m_dt;
  m_dt = Float(dt) / 1000.0;
}

void Scene::updateRotation() {
  vector< SceneObject >::iterator begin, end, ii;
  begin = m_objects.begin();
  end = m_objects.end();
  for(ii = begin; ii != end; ++ii) {
    ii->setRotation(ii->getRotation() + ii->getAngularVelocity() * m_dt);
  }
}

void Scene::updatePhysics(bool thrusting) {
  //proper way to name interators, I was lazy the rest of the program
  vector< SceneObject >::iterator begin, end, ii, jj;
  begin = m_objects.begin();
  end = m_objects.end();
  //we compute all the resultants, then accelerate and move the objects, so that objects stay still while we compute their forces
  for(ii = begin; ii != end; ++ii) {
    //thanks to offloading our computations in a function, we can afford to split this for and get rid of a comparison on every iteration
    ii->setForce();//null resultant force
    for(jj = begin; jj != ii; ++jj) {
      ii->addForce(gravity(*ii, *jj));
    }
    for(jj = ii + 1; jj != end; ++jj) {
      ii->addForce(gravity(*ii, *jj));
    }
    //since we're iterating over out objects, we can thrust now. Ideally, we'd have a vector<bool> indicating which object is thrusting and which isn't. in the case of multiple thrusters, with multiple settings, it would be a vector< vector<Float> >. Still, this simplest case is proof of concept enough. 
    if(thrusting)
      ii->applyThrust();
  }
  //compute now, _after_ we have gotten the forces for all objects involved
  for(ii = begin; ii!= end; ++ii) {
    ii->integrate(m_dt);
  }
  updateRotation();
}

Vector3 Scene::gravity(SceneObject local, SceneObject distant) {
  Vector3 force, r;
  //get the position difference between the two bodies. the order of the operations matters. distant = local + r, thus r = distant - local
  r = distant.getPosition() - local.getPosition();
  if (r.getNorm() == 0) return force;
  //the force has the same direction vector (versor) as the distance 'vector'
  force = r; force.normalise();
  //we multiply the force direction with its magnitude
  force *= k_G * local.getMass() * distant.getMass() / r.getNorm2();
  return force;
}

int Scene::nextFrame(GLint dt, bool thrusting) {
  //fixing the time step. We compute each physics step after, at most, a maximum amount of time. 
  GLint frames = 0;
  Float mdt = Float(dt) * m_timeMul; //time-multiplied delta-t
  while(mdt > m_maxFrameTime) { //if our time is greater than the max time a step should take, we break it up into multiple successive steps.
    updateTime(m_maxFrameTime);
    updatePhysics(thrusting);
    mdt -= m_maxFrameTime;
    ++frames;
  }
  
  if(mdt > 0) { //the left-over time is also computed here.
    updateTime(mdt);
    updatePhysics(thrusting);
    mdt = 0;
    ++frames;
  }
  //we have a horizontal FOV of 90deg, and the camera is positioned at (0, 0, 5). This means that the horizontal length/aperture of the view volume at z = 0 (where we center out scene) is ~around~ 10. In practice, since we apply minimum visual object size, it should be a bit smaller, at least by our minimumSize / 2.
  updateView(9.75);
  return(frames);
}


void Scene::updateView(Float maxLength) {
  Vector3 tv, max, min, center;
  Float len, tf;
  if(m_objects.size() == 0) return;
  std::vector< SceneObject >::iterator it, end;
  it = m_objects.begin();
  end = m_objects.end();
  tv = it->getPosition() + it->getSize() * 0.5;
  max = tv;
  min = tv;
  while(it != end) {
    //position of object center
    tv = it->getPosition();
    //spherical extent of the object
    tf = it->getSize() * 0.5;
    //position + max spherical extent
    max.perAxisMax(tv + tf);
    //position + min spherical extent = pos - max extent
    min.perAxisMin(tv - tf);
    ++it;
  }
  center = (max + min) / 2.0;
  m_offset = Vector3() - center;
    
  len = (max - min).getNorm();
  m_scale = maxLength / len;

    
  /*Not really good, no.
  m_scale = maxLength / len;
  center *= m_scale;
  m_offset = center;
  */
  
  //now, visually enlarge objects that are too small to be seen under the current scale
  it = m_objects.begin();
  Float minSize = 0.5;
  while(it != end) {
    it->setGlScale(1);
    tf = it->getSize() * m_scale;
    if(tf < minSize && tf != 0 ) {
      tf = minSize / tf;
      it->setGlScale(tf);
    }
    ++it;
  }
}

void Scene::addObject(SceneObject sceneObject) {
  m_objects.push_back(sceneObject);
}

void Scene::addObject(DataObject object, Vector3 position, Float size, Float mass, Vector3 velocity, Float rotation, Float angularVelocity, Vector3 thrust) {
  addObject(SceneObject(object, position, size, mass, velocity, rotation, angularVelocity, thrust));
}

std::vector< SceneObject >::iterator Scene::begin() {
  return m_objects.begin();
}
std::vector< SceneObject >::iterator Scene::end() {
  return m_objects.end();
}

pair<Float, Float> Scene::getApPe(unsigned int object) {
  if(object >= getObjectCount() - 1)
    return(make_pair<Float, Float>(0, 0));
  //E = v^2/2 - mu/r
  //a = - mu/(2E)
  //e = (|v|^2 * r)/mu - (r \dot v)v/mu - r/|r|
  //r_{Pe} = a(1 - |e|)
  //r_{Ap} = a(1 + |e|)

  vector<SceneObject>::iterator begin, end, ii, centerMass, self;
  begin = m_objects.begin(); end = m_objects.end();
  Float maxForce = 0, force = 0;
  Float ownMass, mass, distance;
  Vector3 ownPos, pos;
  self = begin + object;
  ownPos = self->getPosition();
  ownMass = self->getMass();
  //search for the greaters gravitational influence upon the body. the following computation will be less accurate the more this deviates from the standard two-body problem
  for(ii = begin; ii != end; ii++) {
    pos = ii->getPosition();
    if(pos == ownPos) continue;
    mass = ii->getMass();
    //force = mass / pow(distance(ownPos, pos), 2);
    if(force > maxForce) {
      maxForce = force;
      centerMass = ii;
    }
  }
  Float mu, r, E, e, v, a, ap, pe;
  
  return(make_pair<Float, Float>(0, 0));
}
