#include "oglDataCache.h"
#include "proxyGl.h"
#include <iostream>
using namespace SdimpleRocket;
using namespace std;

unsigned int OglDataCache::loadGlTexture(std::string texturePath) {
  SDL_Surface *texSdl = NULL;
  std::map<std::string, unsigned int>::iterator it;
  unsigned int i = 0;
  //we load all the resources at the start, and do not allow on-line modifications
  if(m_textureMap.count(texturePath) > 0) {//have we loaded this texture already?
    it = m_textureMap.find(texturePath);
    i = it->second;
  }
  //if SDL loads the texture
  else if((texSdl = SDL_LoadBMP(texturePath.c_str()))) {
    GLuint texId = 0;
    //create space for the texture, let OpenGL return its 'pointer', or Id
    glGenTextures(1, &texId);
    //set the created texture as the active texture
    glBindTexture(GL_TEXTURE_2D, texId);
    //fill the active texture from the sdl data
    glTexImage2D(GL_TEXTURE_2D, 0, 3, texSdl->w, texSdl->h, 0, GL_BGR, GL_UNSIGNED_BYTE, texSdl->pixels);
    //TexParameter are set for each texture. Do not set them globally, such as in init()
    //how should a pixel be computed when the on-screen rendered texture is smaller than the initial bitmap?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //how should a pixel be computed when the on-screen rendered texture is bigger
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //we have finished generating the OpenGL texture; we can now delete the SDL texture;
    if(texSdl) {
      SDL_FreeSurface(texSdl);
      texSdl = NULL;
    }
    i = m_tex.size();
    m_tex.push_back(texId);
    //we record the position of the texture index in its vector, not its value
    m_textureMap.insert(std::pair<std::string, unsigned int>(texturePath, i));
  }
  return i;
}

DataObject OglDataCache::loadObject(std::string objPath, unsigned int tdI) {
  DataObject d;
  //add texture data index now
  d.tdId = m_tex[tdI];
  //we only deal with textured objects that have normals
  d.hasUv = true;
  d.hasNorm = true;
  unsigned int i;
  if(m_vertexMap.count(objPath) > 0) {//have we loaded this object before?
    i = m_vertexMap.find(objPath)->second;
    d.vId = m_vbo[i];
    d.tId = m_tbo[i];
    d.nId = m_nbo[i];
    d.vpf = m_vpf[i];
    d.vn = m_vn[i];
  }
  else {
    //we'll discard this object after converting the information inside it, so it's a local static variable
    ObjLoader o(objPath);
    //now we convert the loaded object into VBOs
    GLuint vId = 0, nId = 0, tId = 0, vn;
    unsigned int vpf = o.m_verticesPerFace;//we assume a constant number of vertices per face (3 or 4)
    unsigned int target = GL_ARRAY_BUFFER, usage = GL_STATIC_DRAW;//our VBO is stored as an array. Also, it's not modified after load, merely drawn a large number of times. As such, OpenGL will likely decide to uploaded to VRAM.
    GLint vArraySize, vBufferSize = 0, nArraySize, nBufferSize = 0, tArraySize, tBufferSize = 0;//checking buffer sizes after upload, to ensure no transfer/memory problems occured.
    std::vector<GLfloat> vert, tex, norm;
    //face vector iterators
    std::vector<GLuint>::iterator fIt, fEnd;
    fIt = o.m_faces.begin();
    fEnd = o.m_faces.end();
    // since we render using texture as well, we can't use Elem array rendering. instead, we have to 'unpack' each face - if a vertex is part of multiple faces, it will appear in our arrays multiple times
    while(fIt != fEnd) {
      //a vertex in out face vector is, in fact, 3 numbers: vertexIndex, textureIndex, normalIndex
      //however, here we mul by 3 not because of that, but since the vertices have 3 coordinates, x, y, z
      i = *fIt * 3;
      vert.push_back(o.m_vertices[i]);
      vert.push_back(o.m_vertices[i + 1]);
      vert.push_back(o.m_vertices[i + 2]);

      //texture only has U, V coordintes
      ++fIt; i = *fIt * 2;
      tex.push_back(o.m_texture[i]);
      tex.push_back(o.m_texture[i + 1]);

      ++fIt; i = *fIt * 3;
      norm.push_back(o.m_normals[i]);
      norm.push_back(o.m_normals[i + 1]);
      norm.push_back(o.m_normals[i + 2]);
      ++fIt;
    }
    vn = vert.size() / 3; // each vertex has 3 coordinates
    vArraySize = sizeof(GLfloat) * vert.size();
    tArraySize = sizeof(GLfloat) * tex.size();
    nArraySize = sizeof(GLfloat) * norm.size();
    //VBO: create the vertex buffer object
    glGenBuffers(1, &vId);
    if(!vId) {
      cerr << "Error generating vertex buffer for " << objPath << endl;
      exit(1);
    }
    //VBO: specify the buffer type, intended usage and start and length; std::vector is guaranteed to store its information contiguously
    glBindBuffer(target, vId);
    glBufferData(target, vArraySize, &(vert[0]), usage);
    //VBO: get information about a buffer
    glGetBufferParameteriv(target, GL_BUFFER_SIZE, &vBufferSize);
    if(vArraySize != vBufferSize) {
      //VBO: always! delete buffers. They reside on the video card, and the kernel cannot clean up memory there.
      //VBO: delete a buffer identified by its Id
      glDeleteBuffers(1, &vId);
      vId = 0;
      cerr << "Error. vertex coord VBO buffer size mismatched vs. initial size. File: " << objPath << endl;
      exit(1);
    }

    //VBO: create the texture coord buffer object
    glGenBuffers(1, &tId);
    if(!tId) {
      cerr << "Error generating texture coord buffer for " << objPath << endl;
      exit(1);
    }
    //VBO: upload texture coordinates the same way, like vertices
    glBindBuffer(target, tId);
    glBufferData(target, tArraySize, &(tex[0]), usage);
    glGetBufferParameteriv(target, GL_BUFFER_SIZE, &tBufferSize);
    if(tArraySize != tBufferSize) {
      glDeleteBuffers(1, &tId);
      tId = 0;
      cerr << "Error. texture coord VBO buffer size mismatched vs. initial size. File: " << objPath << endl;
      exit(1);
    }

    //VBO: create the normals buffer object
    glGenBuffers(1, &nId);
    if(!nId) {
      cerr << "Error generating normals buffer for " << objPath << endl;
      exit(1);
    }
    //VBO: upload normals the same way
    glBindBuffer(target, nId);
    glBufferData(target, nArraySize, &(norm[0]), usage);
    glGetBufferParameteriv(target, GL_BUFFER_SIZE, &nBufferSize);
    if(nArraySize != nBufferSize) {
      glDeleteBuffers(1, &nId);
      nId = 0;
      cerr << "Error. texture coord VBO buffer size mismatched vs. initial size. File: " << objPath << endl;
      exit(1);
    }
    //load the buffer IDs in our OglDataCache
    m_vbo.push_back(vId);
    m_tbo.push_back(tId);
    m_nbo.push_back(nId);
    m_vpf.push_back(vpf);
    m_vn.push_back(vn);
    d.vId = vId;
    d.tId = tId;
    d.nId = nId;
    d.vpf = vpf;
    d.vn = vn;
  }
  return d;
}

DataObject OglDataCache::addObject(std::string objPath, std::string texturePath) {
  GLuint texId;
  //first we load the texture
  texId = loadGlTexture(texturePath);
  //then we load the object. together with the textureID, we assemble the DataObject
  DataObject d = loadObject(objPath, texId);
  //store the object in the OglDataCache
  m_objects.push_back(d);
  //and return it to the caller
  return d;
}

void inline OglDataCache::deleteBuffers(std::vector<GLuint>::iterator begin, std::vector<GLuint>::iterator end) {
  std::vector<GLuint>::iterator it = begin;
  GLuint id;
  for(; it != end; ++it) {
    id = *it;
    if(id) {
      //VBO: free a buffer associated to an ID
      glDeleteBuffers(1, &id);
      *it = id;
    }
  }
}

//VBO: we need to free the VBOs, since they reside in the vram. The kernel cannot clean up after our program in vram as it does in ram.
OglDataCache::~OglDataCache() {
  deleteBuffers(m_vbo.begin(), m_vbo.end());
  deleteBuffers(m_tbo.begin(), m_tbo.end());
  deleteBuffers(m_nbo.begin(), m_nbo.end());
}

void OglDataCache::renderObject(DataObject d) {
  //VBO: having the IDs of all the needed buffers, draw the object
  GLuint target = GL_ARRAY_BUFFER;
  GLuint primitiveType;
  //we decide what primitive to draw, depending on the number of vertices per face
  if(d.vpf == 3)
    primitiveType = GL_TRIANGLES;
  else if(d.vpf == 4)
    primitiveType = GL_QUADS;
  else
    primitiveType = GL_LINE_LOOP; //fallback. strange case, and we decide to just draw wireframe
  //VBO: signal tha we will be using vertex information.
  glEnableClientState(GL_VERTEX_ARRAY);
  //VBO: bind the appropriate vertex buffer; this means that pointer operations in OpenGL will be relative to the start of this buffer
  glBindBuffer(target, d.vId);
  //VBO: each vertex has 3 coordinates, is of the type GLfloat; the vertex position-related values have 0 bytes of non-related data between them (often used to interleave position, texture and normals data in the same vicinity), and the vertex position-related data starts at the very start of the buffer (offset NULL)
  glVertexPointer(3, GL_FLOAT, 0, NULL);

  //VBO: bind the texture coordinate array
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  //VBO: also bind the appropriate texture, where the actual image resides
  glBindTexture(GL_TEXTURE_2D, d.tdId);
  glBindBuffer(target, d.tId);
  //VBO: a vertex has 2 texture coordinates 
  glTexCoordPointer(2, GL_FLOAT, 0, NULL);

  glEnableClientState(GL_NORMAL_ARRAY);
  glBindBuffer(target, d.nId);
  glNormalPointer(GL_FLOAT, 0, NULL);
  //VBO: how to draw objects where vertices are NOT shared between faces. It draws the chosen primitive type, starts with the first vertex, and ends after drawing vn vertices
  glDrawArrays(primitiveType, 0, d.vn);

  //VBO: disable the capabilities we have used
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  //and unbind the buffer
  glBindBuffer(target, 0);
}
