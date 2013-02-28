#ifndef __COLOR_H__
#define __COLOR_H__

#include "./common.h"

struct Color {
  Color() {}
  Color(GLfloat _r, GLfloat _g, GLfloat _b) {
    c[0] = _r;
    c[1] = _g;
    c[2] = _b;
  }
  operator GLfloat*() { return c; }
  GLfloat c[3];

  static Color RandColor();
};

Color Color::RandColor() {
  return Color((float)rand()/(float)RAND_MAX,
               (float)rand()/(float)RAND_MAX,
               (float)rand()/(float)RAND_MAX);
}

#endif
