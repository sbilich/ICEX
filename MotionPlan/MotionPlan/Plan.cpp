//
//  Plan.cpp
//  MotionPlan
//
//  Created by Sara Bilich on 6/23/16.
//  Copyright © 2016 Sara Bilich. All rights reserved.
//

#include "Plan.hpp"
#include "Node.hpp"
using namespace std;

Plan::Plan(Node *end) {
    while (end) {
        plan.push_front(end);
        end = end->getParent();
    }
}