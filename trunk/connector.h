#ifndef __CONNECTOR_H__
#define __CONNECTOR_H__

#include "./common.h"
#include "./vec.h"

struct Color {
  Color() {}
  Color(GLfloat _r, GLfloat _g, GLfloat _b) {
    c[0] = _r;
    c[1] = _g;
    c[2] = _b;
  }
  operator GLfloat*() { return c; }
  GLfloat c[3];
};

struct Connector {
Connector(float _r1=1, float _r2=.5, Color c=Color(0, 0, 1)) : r1(_r1), r2(_r2), color(c) {}
  Vec3f offset;
  float height, r1, r2;
  Color color;
  
  void orientAxis();
  void draw();
};

#endif