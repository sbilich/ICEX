//
//  Node.cpp
//  MotionPlan
//
//  Created by Sara Bilich on 6/14/16.
//  Copyright © 2016 Sara Bilich. All rights reserved.
//

#include "Node.hpp"
#include <math.h>
#include <list>
using namespace std;

Node::Node(float x, float y, float z, float yaw, float pitch, float roll, Node *parent) :
   x(x),
   y(y),
   z(z),
   yaw(yaw),
   pitch(pitch),
   roll(roll),
   parent(parent),
   children(0),
   inPath(false)
{
    if (parent) {
        curLength = parent->getLength() + 1;
    } else
        curLength = 1;
}

/**
 * Returns true if Node |other| contains the same value as |this|, otherwise it returns false.
 */
bool Node::inRangeOf(Node *other) {
    const float epsilon = 0.05;
    
    if (fabs(this->x - other->x) > epsilon)
        return false;
    else if (fabs(this->y - other->y) > epsilon)
        return false;
    else if (fabs(this->z - other->z) > epsilon)
        return false;
    else if (fabs(this->yaw - other->yaw) > epsilon)
        return false;
    else if (fabs(this->pitch - other->pitch) > epsilon)
        return false;
    else if (fabs(this->roll - other->roll) > epsilon)
        return false;
    return true;
}