#ifndef __OBJLOADER_H__
#define __OBJLOADER_H__

#include <fstream>
#include <vector>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string>
#include "types.h"
#include "vector3.h"

namespace SdimpleRocket {

class ObjLoader {
 public:
  //we use these vectors for their auto-allocation feature; they treat different coordinates the same way - i.e. array of values, not mathematical vectors.
  //3-dimensional: x, y, z
  std::vector<GLfloat> m_vertices;
  //2-dimensional: u, v
  std::vector<GLfloat> m_texture;
  std::vector<GLfloat> m_normals;
  Vector3 m_min, m_max;
  // integer indices: a face can be made up by the 0-th, 2-nd and 5-th vertex.
  std::vector<GLuint> m_faces;
  bool m_hasUv;
  bool m_hasNormals;
  //Limitation: we only deal with faces of a constant number of vertices. This values partitions the continuous m_faces vector into separate faces
  unsigned int m_verticesPerFace;

  //i encoutered the stupidest bug or huuge lapse in my C++ constructor and object copy knowledge in C++99 here. fix'd-ish
  ObjLoader() {
    m_hasUv = false;
    m_hasNormals = false;
    m_verticesPerFace = 3;
  }


  ObjLoader(std::string fileName) {
    ObjLoader();
    load(fileName.c_str());
  }
  
  ObjLoader(std::string *fileName) {
    ObjLoader();
    load(fileName->c_str());
  }
  
  ObjLoader(const char* fileName) {
    ObjLoader();
    load(fileName);
  }

  void load(const char* fileName);
  void parse(std::ifstream* file);
  void parseVertices(StringVect tokens);
  void parseTexture(StringVect tokens);
  void parseNormals(StringVect tokens);
  void parseFaces(StringVect tokens);
  void computeMinMax();
  void normaliseAndCenter();
  
  std::string dumpVect(std::vector<GLfloat> v, int groupSize = 3);
  
  std::string dump();
  
};

}

#endif //__OBJLOADER_H__
