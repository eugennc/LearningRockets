#ifndef __OGLDATACACHE_H__
#define __OGLDATACACHE_H__

#include <vector>
#include <string>
#include <map>
#include "proxyGl.h"
#include "dataStructures.h"
#include "objLoader.h"
#include "vector3.h"

namespace SdimpleRocket{

struct DataObject {
  GLuint vId, tId, nId, tdId; //vertex VBO id, texture coordinates VBO id, normals VBO id, texture data id; it corelates multiple disparate elements in a single visual object. This allows for re-using texture data for multiple objects, for instance.
  GLuint vpf; //verticesPerFace
  GLuint vn; //number of vertices
  bool hasUv, hasNorm; //does it have texture coordinates; does it have normals?
};

  
class OglDataCache {
 protected:
  unsigned int loadGlTexture(std::string texturePath);
  DataObject loadObject(std::string objPath, unsigned int  tdI);
  void deleteBuffers(std::vector<GLuint>::iterator begin, std::vector<GLuint>::iterator end);
  
 public:
  std::vector<GLuint> m_vbo, m_tbo, m_nbo, m_tex; //ids for: vertex buffer objects, texture buffer objects, normals buffer objects, texture data. A certain vbo, tbo, nbo and vpf index points to the same object; however, m_tex can be mixed and matched with objects, allowing to reuse textures.
  std::vector<GLuint> m_vpf; //vertices per face
  std::vector<GLuint> m_vn; //number of vertices
  std::vector<DataObject> m_objects; // we need not store DataObjects here. Was left here during development/testing.
  //these maps let us interact with the dataCache using object and texture pathnames, while allowind the code to use faster array indices
  std::map<std::string, unsigned int> m_vertexMap;
  std::map<std::string, unsigned int> m_textureMap;


  
  OglDataCache() {}
  ~OglDataCache();
  DataObject addObject(std::string objPath, std::string texturePath);
  DataObject inline addObject(const char* objPath, const char* texturePath) {
    return addObject(std::string(objPath), std::string(texturePath));
  }

  void renderObject(DataObject o);
};

}
#endif
