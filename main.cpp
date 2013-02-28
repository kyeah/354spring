/*
  CS 354 - Fall 2012
  main.cpp

  This is simple skeleton GLUT code.
*/

// C++ library includes
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cstdio>
#include <cstring>
#include <vector>

#include "./common.h"
#include "./types.h"
#include "./bvh_defs.h"
#include "./joint.h"
#include "./loader.h"
#include "./geom.h"

using namespace std;

// window parameters
int window_width = 800, window_height = 600;
float window_aspect = window_width/static_cast<float>(window_height);

float fps = 60;
float frame_tick = 0;
float previousTime;

float zoom_speed = 5;
float orbit_speed = .2f;

// callback prototypes
void InitGL();
void Display();
void Resize(int width, int height);
void Keyboard(unsigned char key, int x, int y);
void Idle();

vector<SceneGraph> scenegraphs;
bool animate = false;
float *channels;
TreeNode joint;

#define PI 3.14159265f

Vec3f eye, center, up;
int waypoint = 1;

BoundingBox bbox = {{-100, -100, -100}, {100, 100, 100}};

// lighting
GLfloat light_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
GLfloat light_diffuse[] = { 0.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_specular[] = { 0, 1.0f, 1.0f, 1.0f };
GLfloat light_position[] = { 0, 10, 3, 0 };
GLfloat light_direction[] = { 1, 1, -1, 0 };

char filename[1000];

bool showAxis = true;
float axisLen = 1.0f;

bool showBounds = false;

void SetLighting();

void InitGL() {
  // Perform any necessary GL initialization in this function

  // enable depth testing, and set clear color to white
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);
  glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  SetLighting();

  // resize the window
  Resize(window_width, window_height);
}

void ComputeLookAt() {
  float maxDist = (bbox.max-bbox.min).max();
  center = (bbox.max+bbox.min)/2.0f;
  up = Vec3f::makeVec(0.0f, 1.0f, 0.0f);
  eye = center+Vec3f::makeVec(0.0f, 0.75f*maxDist, -1.5f*maxDist);
  if (waypoint == 1) {
    eye = center+Vec3f::makeVec(0.5f*maxDist, 0.75f*maxDist, 1.5f*maxDist);
  } else if (waypoint == 2) {
    eye = center+Vec3f::makeVec(0, 0.1f*maxDist, 1.5f*maxDist);
  } else if (waypoint == 3) {
    eye = center+Vec3f::makeVec(1.5f*maxDist, 0.1f*maxDist, 0);
  }
  axisLen = maxDist*0.05f;
}

void resetCamera() {
  Vec3f min = {-100, -100, -100}, max = {100, 100, 100};
  Vec3f offset = {0, 0, 0};
  bbox.min = min;
  bbox.max = max;
  ComputeLookAt();
}

void SetLighting() {
  glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, light_direction);
  glEnable(GL_COLOR_MATERIAL);
}

void SetCamera() {
  gluLookAt(eye[0], eye[1], eye[2],
            center[0], center[1], center[2],
            up[0], up[1], up[2]);
}

void SetProjection() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluPerspective(40.0, window_aspect, 1, 1500);
}

void SetDrawMode() {
  glPolygonMode(GL_FRONT, GL_FILL);
  glPolygonMode(GL_BACK, GL_FILL);
}

void DrawAxis() {
  Vec3f c = (bbox.min+bbox.max)*0.5f;
  float L = (bbox.max-bbox.min).max() * 0.2;

  Vec3f X = {L, 0, 0}, Y = {0, L, 0}, Z = {0, 0, L};

  glLineWidth(2.0);

  glDisable(GL_LIGHTING);
  glBegin(GL_LINES);

  glColor3f(1, 0, 0);
  glVertex3fv(c.x);
  glVertex3fv((c+X).x);

  glColor3f(0, 1, 0);
  glVertex3fv(c.x);
  glVertex3fv((c+Y).x);

  glColor3f(0, 0, 1);
  glVertex3fv(c.x);
  glVertex3fv((c+Z).x);

  glEnd();
  glEnable(GL_LIGHTING);
}

void DrawFloor(float W, float H, float w, float h) {
  float a = H/h, b = W/w;
  int M = static_cast<int>(floor(a+0.5f));
  int N = static_cast<int>(floor(b+0.5f));
  int i = 0, j = 0;
  Vec3f u = {w, 0, 0}, v = {0, 0, h}, r = {-(N/2)*w, 0, -(M/2)*h};
  Vec3f p0, p1, p2, p3;
  glDisable(GL_LIGHTING);
  glEnable(GL_POLYGON_SMOOTH);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
  glBegin(GL_QUADS);
  glColor3f(0.5, 0.5, 0.5);
  for (j = 0; j < N; j++) {
    p0 = r+u*static_cast<float>(j%2);
    for (i = j%2; i < M; i += 2) {
      p1 = p0+u;
      p2 = p1+v;
      p3 = p2-u;

      glVertex3fv(p3.x);
      glVertex3fv(p2.x);
      glVertex3fv(p1.x);
      glVertex3fv(p0.x);

      p0 += u*2.0f;
    }
    r += v;
  }
  glEnd();
  glEnable(GL_LIGHTING);
}

void DrawRect(const Vec3f & u, const Vec3f & v, const Vec3f & o) {
  glBegin(GL_LINE_STRIP);
  glColor3f(0, 0, 1);
  glVertex3fv(o.x);
  glVertex3fv((o+u).x);
  glVertex3fv((o+u+v).x);
  glVertex3fv((o+v).x);
  glVertex3fv(o.x);
  glEnd();
}

void DrawBounds() {
  Vec3f u, v, m1[] = {bbox.min, bbox.max}, m2[] = {bbox.max, bbox.min};

  for (int k = 0; k < 2; k++) {
    for (int i = 0; i < 3; i++) {
      for (int j = i+1; j < 3; j++) {
        u = m1[k];
        v = m1[k];
        u.x[i] = m2[k].x[i];
        v.x[j] = m2[k].x[j];
        u = u-m1[k];
        v = v-m1[k];
        DrawRect(u, v, m1[k]);
      }
    }
  }
}

void Joint(int i) {
  glPushMatrix();
  scenegraphs[i].connectors[joint.id].draw();
  glTranslatef(joint.offset[0], joint.offset[1], joint.offset[2]);

  if (joint.type != BVH_END_SITE) {
    for (int i = 0; i < joint.numchans; i++) {
      switch (joint.order[i]) {
      case BVH_XPOS_IDX:
        glTranslatef(channels[joint.index+i], 0, 0);
        break;
      case BVH_YPOS_IDX:
        glTranslatef(0, channels[joint.index+i], 0);
        break;
      case BVH_ZPOS_IDX:
        glTranslatef(0, 0, channels[joint.index+i]);
        break;
      case BVH_XROT_IDX:
        glRotatef(channels[joint.index+i], 1, 0, 0);
        break;
      case BVH_YROT_IDX:
        glRotatef(channels[joint.index+i], 0, 1, 0);
        break;
      case BVH_ZROT_IDX:
        glRotatef(channels[joint.index+i], 0, 0, 1);
      }
    }
  }

  joint.draw();

  vector<unsigned int> joint_children = joint.children;
  int numchildren = joint.children.size();
  for (int j = 0; j < numchildren; j++) {
    joint = scenegraphs[i].joints[joint_children[j]];
    Joint(i);
  }
  glPopMatrix();
}

void Display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  SetProjection();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  SetLighting();
  SetCamera();
  SetDrawMode();
  DrawFloor(800, 800, 80, 80);

  for (int i = 0; i < scenegraphs.size(); i++) {
    channels = scenegraphs[i].frames[scenegraphs[i].currentFrame];
    joint = scenegraphs[i].joints[0];
    Joint(i);
  }

  if (showAxis) DrawAxis();
  if (showBounds) DrawBounds();

  glFlush();          // finish the drawing commands
  glutSwapBuffers();  // and update the screen
}

// This reshape function is called whenever the user
// resizes the display window.
void Resize(int width, int height) {
  window_width = width;
  window_height = height;
  float fheight = static_cast<float>(height);
  window_aspect = width / fheight;

  // resize the window
  glViewport(0, 0, window_width, window_height);

  SetProjection();

  // setup basic orthographic projection
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  // let glut know to redraw the screen
  glutPostRedisplay();
}

void orbit(int dir) {
  float ex = eye[0], ez = eye[2];
  float r = sqrt(ex*ex + ez*ez);
  float theta = atan2(ez, ex) + 2*PI;

  theta += dir*orbit_speed;
  eye[0] = r*cos(theta);
  eye[2] = r*sin(theta);
}

// This function is called whenever the user hits letters or numbers
// on the keyboard.  The 'key' variable has the character the user hit,
// and x and y tell where the mouse was when it was hit.
void Keyboard(unsigned char key, int x, int y) {
  y = window_height - y;

  float sgn = 1.0f;
  float ex, ez;
  Vec3f v;

  switch (key) {
    case '1':
      waypoint = 1;
      resetCamera();
      ComputeLookAt();
      break;
    case '2':
      waypoint = 2;
      resetCamera();
      ComputeLookAt();
      break;
    case '3':
      waypoint = 3;
      resetCamera();
      ComputeLookAt();
      break;
    case 'z':
      v = (eye - center).unit();
      eye -= v*zoom_speed;
      break;
    case 'Z':
      v = (eye - center).unit();
      eye += v*zoom_speed;
      break;
    case 'j':
      orbit(1);
      break;
    case 'k':
      orbit(-1);
      break;
    case ' ':
      animate=!animate;
      break;
    case 'a':
      showAxis=!showAxis;
      break;
    case 'b':
      showBounds=!showBounds;
      break;
    case 'q':
    case 27:  // esc
      exit(0);
      break;
  }

  // let glut know to redraw the screen
  glutPostRedisplay();
}

void Idle() {
  // Maintains a constant FPS
  float currentTime = glutGet(GLUT_ELAPSED_TIME);
  float elapsed_time = currentTime - previousTime;
  previousTime = currentTime;

  if (animate) {
    frame_tick += fps*elapsed_time/1000;
    frame_tick = fmodf(frame_tick, scenegraphs[0].numFrames);

    for (int i = 0; i < scenegraphs.size(); i++)
      scenegraphs[i].SetCurrentFrame(static_cast<int>(frame_tick));
    glutPostRedisplay();
    }
}

void processCommandLine(int argc, char *argv[]) {
  if (argc>1) {
    for (int i = 0; i < 1/*argc - 1*/; i++) {
      SceneGraph sg = SceneGraph();
      snprintf(&(filename[0]), strlen(argv[i+1])+1, "%s", argv[i+1]);
      BVHLoader::loadBVH(filename, &sg);
      scenegraphs.push_back(sg);
      BVHLoader::bci = bvh_cb_info();
    }
    ComputeLookAt();
  } else {
    printf("Filename argument required.\n");
    exit(0);
  }
}

void showMenu() {
  cout << "------------ menu ------------" << endl;
  cout << "q - quit" << endl;
  cout << "a - show/hide axis" << endl;
  cout << "b - show/hide bounds" << endl;
  cout << "[1-3] - move to waypoint" << endl;
  cout << "z - zoom in" << endl;
  cout << "Z - zoom out" << endl;
  cout << "j - rotate left" << endl;
  cout << "k - rotate right" << endl;
  cout << "[SPACE] - start/stop" << endl;
}

int main(int argc, char *argv[]) {
  // Initialize GLUT
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(window_width, window_height);
  glutInitWindowPosition(100, 100);
  // Do not change the name of the window -- it is needed for the screenshot
  glutCreateWindow("Mocap");
  glutDisplayFunc(Display);
  glutReshapeFunc(Resize);
  glutKeyboardFunc(Keyboard);
  glutIdleFunc(Idle);

  processCommandLine(argc, argv);

  showMenu();

  InitGL();

  glutMainLoop();

  return 0;
}
