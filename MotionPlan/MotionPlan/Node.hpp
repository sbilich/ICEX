//
//  Node.hpp
//  MotionPlan
//
//  Created by Sara Bilich on 6/14/16.
//  Copyright © 2016 Sara Bilich. All rights reserved.
//
#pragma once
#ifndef Node_hpp
#define Node_hpp

#include <stdio.h>
#include <vector>
#include <memory>

class Node
{
    public:
    Node(float x, float y, float z, float yaw, float pitch, float roll, Node *prev);
    bool inRangeOf(Node *other);
    
    float getX() { return x; }
    float getY() { return y; }
    float getZ() { return z; }
    float getYaw() { return yaw; }
    float getPitch() { return pitch; }
    float getRoll() { return roll; }
    Node *getParent() { return parent; }
    int getChildren() { return children; }
    int getLength() { return curLength; }
    void increaseChildCount() { children++; }
    bool isPath() { return inPath; }
    void markInPath() { inPath = true; }
    
    private:
    float x;
    float y;
    float z;
    float yaw;
    float pitch;
    float roll;
    Node *parent;
    int children;
    int curLength;
    bool inPath;
};

#endif /* Node_hpp */
