#include <stdint.h>
#include <iostream>

#include "./joint.h"

using namespace std;

void SceneGraph::CreateRoot(const char * name, uint32_t id) {
  string s(name, sizeof(name));
  char *copy = new char[s.length()+1];
  strcpy(copy, s.c_str());
  
  root = TreeNode(0, copy, id);
  joints.push_back(root);
  last_id++;
}

void SceneGraph::CreateJoint(const char * name, uint32_t id) {
  string s(name, sizeof(name));
  char *copy = new char[s.length()+1];
  strcpy(copy, s.c_str());
  
  joints.push_back(TreeNode(1, copy, id));
  last_id++;
}

void SceneGraph::CreateEndSite(const char * name, uint32_t id) {
  string s(name, sizeof(name));
  char *copy = new char[s.length()+1];
  strcpy(copy, s.c_str());
  
  joints.push_back(TreeNode(2, copy, id));
  last_id++;
}

void SceneGraph::SetChild(uint32_t parent, uint32_t child) {
  joints[child].parent = parent;
  joints[parent].children.push_back(child);
}

void SceneGraph::SetOffset(uint32_t id, float * offset) {
  for (int i = 0; i < 3; i++) {
    joints[id].offset[i] = offset[i];
  }
}

void SceneGraph::SetNumChannels(uint32_t id, uint16_t num) {
  joints[id].numchans = num;
}

void SceneGraph::SetChannelFlags(uint32_t id, uint16_t flags) {
  joints[id].chanflags = flags;
}

void SceneGraph::SetChannelOrder(uint32_t id, int * order) {
  for (int i = 0; i < 6; i++)
    joints[id].order[i] = order[i];
}

void SceneGraph::SetFrameIndex(uint32_t id, uint32_t index) {
  joints[id].index = index;
}

void SceneGraph::SetFrameTime(float delta) {
  frameTime = delta;
}

void SceneGraph::SetNumFrames(uint32_t num) {
  numFrames = num;
}

void SceneGraph::SetFrameSize(uint32_t size) {
  frameSize = size;
}

void SceneGraph::AddFrame(float * data) {
  float *flts = new float[frameSize];
  for(int i = 0; i < frameSize; i++)
    flts[i] = data[i];
  frames.push_back(flts);
}

void SceneGraph::SetCurrentFrame(uint32_t frameNumber) {
  currentFrame = frameNumber;
}
