#ifndef __VECTOR3_H__
#define __VECTOR3_H__

#include <cmath>
#include <iostream>
#include "types.h"
namespace SdimpleRocket {
//mathematics / physics vector, element of a 3-dimensional Vector Space, with Euclidean (L_p, p = 2) metric

class Vector3 {
 protected:
  Float m_x, m_y, m_z;
  Float m_x2, m_y2, m_z2; //squared values, for caching
  Float m_norm; //the vector's euclidean norm
  Float m_norm2;
  
  void update();
  void updateNorm();
  
 public:
 Vector3(Float x = 0, Float y = 0, Float z = 0):
  m_x(x), m_y(y), m_z(z) {
    update();
  }
  
  Vector3(const Vector3 &);

  Float getX() const { return m_x; } 
  void setX(const Float x) { m_x = x; m_x2 = pow(x, 2); updateNorm();}
  Float getY() const { return m_y; }
  void setY(const Float y) { m_y = y; m_y2 = pow(y, 2); updateNorm();}
  Float getZ() const { return m_z; }
  void setZ(const Float z) { m_z = z; m_z2 = pow(z, 2); updateNorm();}
  Float getNorm() const { return m_norm; }
  Float getNorm2() const { return m_norm2; }
  void normalise();

  void perAxisMax(const Vector3 &);
  void perAxisMin(const Vector3 &);
  
  Vector3& operator=(const Vector3 &);
  Vector3& operator*=(const Float &);
  Vector3& operator/=(const Float &);
  Vector3& operator+=(const Vector3 &);
  Vector3& operator-=(const Vector3 &);
  //for convenience
  Vector3 & operator+=(const Float &);
  Vector3 & operator-=(const Float &);

  
  bool operator==(const Vector3 &) const ; 
  bool operator!=(const Vector3 &) const ;
  Vector3 operator+(const Vector3 &);
  Vector3 operator-(const Vector3 &);
  Vector3 operator-();

  Vector3 operator+(const Float &);
  Vector3 operator-(const Float &);

  Vector3 operator*(const Float &);
  Float operator*(const Vector3 &);
  Vector3 operator/(const Float &);
  void print() const { std::cout << "(" << m_x << ", " << m_y << ", " << m_z << ") "; }
  //friend Vector3 operator*(const Float& lhs, const Vector3 &rhs);


};

//outside-class operator for scalar * vector operation
inline Vector3 operator*(const Float &lhs, const Vector3 &rhs) {
  Vector3 ret(rhs);
  return ret * lhs;
}

/*Float distance(Vector3 lhs, Vector3 rhs) {
  return (lhs - rhs).getNorm();
  }*/
 
}
#endif
