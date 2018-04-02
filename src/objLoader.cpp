#include <iostream>
#include "objLoader.h"
#include "tokeniser.h"
#include <cstdlib>
#include <sstream>

namespace SdimpleRocket {
using namespace std;
  void ObjLoader::load(const char *fileName) {
    ifstream file;
    file.open(fileName, ios::in);
    if(file == NULL) {
      //      throw(string("Could not open .obj file: ") + fileName);
      cerr << "Could not open .obj file: " << fileName << endl;
      exit(1);
    }
    parse(&file);
    file.close();
    normaliseAndCenter();
  }
  
  void ObjLoader::parse(ifstream* file) {
    string line, tok;
    StringVect tokens;
    while(*file != NULL) {
      getline(*file, line);
      tokens = tokenise(line);
      if(tokens.size() > 0) {
	if(tokens[0] == "v") {
	  parseVertices(tokens);
	}
	else if(tokens[0] == "vt") {
	  parseTexture(tokens);
	}
	else if(tokens[0] == "vn") {
	  parseNormals(tokens);
	}
	else if(tokens[0] == "f") {
	  parseFaces(tokens);
	} 
      }
    }
    //std::cout<<m_vertices.size()<<" "<<m_texture.size()<<std::endl;
  }


  void ObjLoader::parseVertices(StringVect tokens) {
    //v 1 0.5 -1
    //  x   y  z
    if(tokens.size() != 4) {
      throw(string("Error: a vertex definition should contain 3 coordinates."));
    }
    for(int i = 1; i < 4; ++i)
      m_vertices.push_back(::atof(tokens[i].c_str()));
  }

  void ObjLoader::parseTexture(StringVect tokens) {
    //vt 1 -0.2
    //   u    v
    if(tokens.size() != 3) {
      throw(string("Error: a texture definition should contain 2 coordinates."));
    }
    for(int i = 1; i < 3; ++i) {
      m_texture.push_back(::atof(tokens[i].c_str()));
    }
    m_hasUv = true;
  }

  void ObjLoader::parseNormals(StringVect tokens) {
    //vn 1 0.5 0.2
    if(tokens.size() != 4) {
      throw(string("Error: a normals definition should contain 3 coordinates."));
    }
    for(int i = 1; i < 4; ++i)
      m_normals.push_back(::atof(tokens[i].c_str()));
    m_hasNormals = true;
  }

  void ObjLoader::parseFaces(StringVect tokens) {
    //we assume a very, very simple, regular object. Such object has faces consisting of a constant number of vertices.
    StringVect::iterator it, end;
    StringVect results;
    m_verticesPerFace = tokens.size() - 1;
    it = tokens.begin() + 1; //skip "f"
    end = tokens.end();
    if(m_hasUv) {
      if(m_hasNormals) {
	// f 1/1/1 2/2/1 3/3/1 4/4/1
	//   v1/vt1/vn1 ... so on
	// we change this to start at 0, such as: 1/1/1->0/0/0
	for(; it != end; ++it) {
	  results = tokenise(*it, "/");
	  for(int i = 0; i < 3; ++i)
	    m_faces.push_back(::atoi(results[i].c_str()) - 1);
	}
      }
      else {
	// f 1/1 2/2 3/3 4/4
	//   v1/vt1 v2/vt2 ... so on
	for(; it != end; ++it) {
	  results = tokenise(*it, "/");
	  for(int i = 0; i < 2; ++i)
	    m_faces.push_back(::atoi(results[i].c_str()) - 1);
	}
      }
    }
    else {
      //f 1 2 3 4
      for(; it != end; ++it)
	m_faces.push_back(::atof(it->c_str()));
    }
  }
  
  string ObjLoader::dumpVect(std::vector<GLfloat> v, int groupSize) {
    unsigned int i = 1;
    stringstream result;
    std::vector<GLfloat>::iterator it = v.begin(), end = v.end();
    for(; it != end; ++it, ++i) {
      result << *it << " ";
      if(i % groupSize == 0)
	result << endl;
    }
    return result.str();
  }
  
  string ObjLoader::dump() {
    string result;
    char a = '0' + m_verticesPerFace;
    result += dumpVect(m_vertices, 3) + "\n";
    result += dumpVect(m_texture, 2) + "\n";
    result += dumpVect(m_normals, 3);
    result += "verticesPerFace: " + a;
    return result;
  }

  void ObjLoader::computeMinMax() {
    //we assume we have at least a vertex in the object
    m_max.setX(m_vertices[0]);
    m_max.setY(m_vertices[1]);
    m_max.setZ(m_vertices[2]);
    m_min.setX(m_vertices[0]);
    m_min.setY(m_vertices[1]);
    m_min.setZ(m_vertices[2]);
    vector<GLfloat>::iterator it, end;
    it = m_vertices.begin();
    end = m_vertices.end();
    while(it != end) {
      if(*it < m_min.getX()) {
	m_min.setX(*it);
      }
      if(*it > m_max.getX()) {
	m_max.setX(*it);
      }
      ++it;

      if(*it < m_min.getY()) {
	m_min.setY(*it);
      }
      if(*it > m_max.getY()) {
	m_max.setY(*it);
      }
      ++it;

      if(*it < m_min.getZ()) {
	m_min.setZ(*it);
      }
      if(*it > m_max.getZ()) {
	m_max.setZ(*it);
      }
      ++it;
    }
  }
  void ObjLoader::normaliseAndCenter() {
    computeMinMax();
    if(m_max == m_min) return;
    Vector3 span, center;
    span = m_max - m_min;
    Float length = span.getNorm();
    center = (m_min + m_max) / 2.0;
    vector<GLfloat>::iterator it, end;
    it = m_vertices.begin();
    end = m_vertices.end();
    while(it != end) {
      *it = (*it - center.getX()) / length;
      ++it;
      *it = (*it - center.getY()) / length;
      ++it;
      *it = (*it - center.getZ()) / length;
      ++it;
    }
  }
}

