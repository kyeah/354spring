#include "./connector.h"
#include "./common.h"
#include "./vec.h"

const float PI = 3.14159265f;

// Orients the Z-axis with the origin-offset vector
void Connector::orientAxis() {
  Vec3f target_dir = offset.unit();
  float rot_angle = acos( target_dir.x[2] ); 
 
  Vec3f axis = {-1*target_dir.x[1], target_dir.x[0], 0};  // Cross product of target_dir and neg z-axis
  Vec3f rot_axis = axis.unit();

  glRotatef( rot_angle*180.0/PI, rot_axis.x[0], rot_axis.x[1], rot_axis.x[2] );
}

void Connector::draw() {
  glPushMatrix();
  glColor3fv(color);

  orientAxis();
  GLUquadric *obj = gluNewQuadric();
  gluCylinder(obj, r1, r2, height, 10, 10);

  glPopMatrix();
}
