#ifndef __JOINT_H__
#define __JOINT_H__

#include <vector>
#include <cstring>
#include <cmath>

#include "./bvh_defs.h"
#include "./common.h"
#include "./connector.h"
#include "./vec.h"

using namespace std;

struct TreeNode {
  TreeNode() {}
  TreeNode(unsigned char _type, char *_name, unsigned int _id, 
           float conn_radius=1) : type(_type), name(_name), id(_id), connector_radius(conn_radius) {}

  unsigned char type; // 0=root, 1=joint, 2=end site                                               
  unsigned int id;
  unsigned int parent;
  vector<unsigned int> children;
  char * name;
  unsigned int numchans;
  unsigned short chanflags;
  unsigned int index;
  int order[6];
  float offset[3];
  float connector_radius;
  
  void draw() {
    glColor3f(0, 0, 0);
    glutSolidSphere(.15, 10, 10);
  }
};

class SceneGraph {
 public:
  SceneGraph() {}
  TreeNode root;
  float frameTime;
  uint32_t numFrames, frameSize, currentFrame;
  unsigned int last_id;
  vector<TreeNode> joints;
  vector<Connector> connectors;
  vector<float*> frames;

  void CreateRoot(const char * name, uint32_t id);
  void CreateJoint(const char * name, uint32_t id);
  void CreateEndSite(const char * name, uint32_t id);
  void SetChild(uint32_t parent, uint32_t child);
  void SetOffset(uint32_t id, float * offset);
  void SetNumChannels(uint32_t id, uint16_t num);
  void SetChannelFlags(uint32_t id, uint16_t flags);
  void SetChannelOrder(uint32_t id, int * order);
  void SetFrameIndex(uint32_t id, uint32_t index);
  void SetFrameTime(float delta);
  void SetNumFrames(uint32_t num);
  void SetFrameSize(uint32_t size);
  void AddFrame(float * data);
  void SetCurrentFrame(uint32_t frameNumber);
};


#endif

