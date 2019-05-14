#include "vector3.h"

using namespace SdimpleRocket;

Vector3::Vector3(const Vector3 &src) {
  setX(src.getX());
  setY(src.getY());
  setZ(src.getZ());
  update();
}

void Vector3::update() {
  m_x2 = pow(m_x, 2);
  m_y2 = pow(m_y, 2);
  m_z2 = pow(m_z, 2);
  updateNorm();
}

void Vector3::updateNorm() {
  m_norm2 = m_x2 + m_y2 + m_z2;
  m_norm = sqrt(m_norm2);
}

void Vector3::normalise() {
  if(m_norm != 0) {
    m_x /= m_norm;
    m_y /= m_norm;
    m_z /= m_norm;
    update();
  }
}


void Vector3::perAxisMax(const Vector3 &rhs) {
  if(m_x < rhs.getX())
    m_x = rhs.getX();
  if(m_y < rhs.getY())
    m_y = rhs.getY();
  if(m_z < rhs.getZ())
    m_z = rhs.getZ();
}

void Vector3::perAxisMin(const Vector3 &rhs) {
  if(m_x > rhs.getX())
    m_x = rhs.getX();
  if(m_y > rhs.getY())
    m_y = rhs.getY();
  if(m_z > rhs.getZ())
    m_z = rhs.getZ();
}

Vector3& Vector3::operator=(const Vector3 &rhs) {
  m_x = rhs.getX();
  m_y = rhs.getY();
  m_z = rhs.getZ();
  update();
  return *this;
}

Vector3& Vector3::operator*=(const Float &rhs) {
  m_x *= rhs;
  m_y *= rhs;
  m_z *= rhs;
  update();
  return *this;
}

Vector3& Vector3::operator/=(const Float &rhs) {
  m_x /= rhs;
  m_y /= rhs;
  m_z /= rhs;
  update();
  return *this;
}
  
Vector3& Vector3::operator+=(const Vector3 &rhs) {
  m_x += rhs.m_x;
  m_y += rhs.m_y;
  m_z += rhs.m_z;
  update();
  return *this;
}

Vector3 & Vector3::operator-=(const Vector3 &rhs) {
  m_x -= rhs.m_x;
  m_y -= rhs.m_y;
  m_z -= rhs.m_z;
  update();
  return *this;
}

Vector3& Vector3::operator+=(const Float &rhs) {
  m_x += rhs;
  m_y += rhs;
  m_z += rhs;
  update();
  return *this;
}

Vector3 & Vector3::operator-=(const Float &rhs) {
  m_x -= rhs;
  m_y -= rhs;
  m_z -= rhs;
  update();
  return *this;
}

bool Vector3::operator==(const Vector3 &rhs) const {
  if(getX() == rhs.getX() && getY() == rhs.getY() && getZ() == rhs.getZ())
    return true;
  else
    return false;
}

bool Vector3::operator!=(const Vector3 &rhs) const {
  return !(*this == rhs);
}

Vector3 Vector3::operator+(const Vector3 &rhs) {
  return Vector3(*this) += rhs;
}

Vector3 Vector3::operator-(const Vector3 &rhs) {
  return Vector3(*this) -= rhs;
}

Vector3 Vector3::operator-() {
  return Vector3(-m_x, -m_y, -m_z);
}



Vector3 Vector3::operator+(const Float &rhs) {
  return Vector3(*this) += rhs;
}

Vector3 Vector3::operator-(const Float &rhs) {
  return Vector3(*this) -= rhs;
}


Vector3 Vector3::operator*(const Float &rhs) {
  return Vector3(*this) *= rhs;
}
 
Float Vector3::operator*(const Vector3 &rhs) {
  return (getX() * rhs.getX() + getY() * rhs.getY() + getZ() * rhs.getZ());
}
 
Vector3 Vector3::operator/(const Float &rhs) {
  return Vector3(*this) /= rhs;
}

