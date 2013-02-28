#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <string>

#include "./joint.h"

using namespace std;

void SceneGraph::CreateRoot(const char * name, uint32_t id) {
  char *copy = new char[sizeof(name)+1];
  snprintf(copy, sizeof(name), "%s", name);

  root = TreeNode(0, copy, id);
  joints.push_back(root);
  connectors.push_back(Connector());
  last_id++;
}

void SceneGraph::CreateJoint(const char * name, uint32_t id) {
  string s(name, sizeof(name));
  char *copy = new char[sizeof(name)+1];
  snprintf(copy, sizeof(name), "%s", name);

  joints.push_back(TreeNode(1, copy, id));
  last_id++;

  // Identify slim connectors
  string slim[] = {"Toe", "Finger", "Hand", "Foot", "Thumb"};
  for (int i = 0; i < 5; i++)
    if (s.find(slim[i]) != string::npos) {
      connectors.push_back(Connector(.3, .15));
      return;
    }

  connectors.push_back(Connector());
}

void SceneGraph::CreateEndSite(const char * name, uint32_t id) {
  char *copy = new char[sizeof(name)+1];
  snprintf(copy, sizeof(name), "%s", name);

  joints.push_back(TreeNode(2, copy, id));
  connectors.push_back(Connector(.3, .15));
  last_id++;
}

void SceneGraph::SetChild(uint32_t parent, uint32_t child) {
  joints[child].parent = parent;
  joints[parent].children.push_back(child);
}

void SceneGraph::SetOffset(uint32_t id, float * offset) {
  for (int i = 0; i < 3; i++) {
    joints[id].offset[i] = offset[i];
    connectors[id].offset[i] = offset[i];
  }

  connectors[id].height = sqrt(offset[0]*offset[0] +
                               offset[1]*offset[1] +
                               offset[2]*offset[2]);
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
  for (int i = 0; i < frameSize; i++)
    flts[i] = data[i];
  frames.push_back(flts);
}

void SceneGraph::SetCurrentFrame(uint32_t frameNumber) {
  currentFrame = frameNumber;
}
