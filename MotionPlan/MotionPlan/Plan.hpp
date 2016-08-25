//
//  Plan.hpp
//  MotionPlan
//
//  Created by Sara Bilich on 6/23/16.
//  Copyright © 2016 Sara Bilich. All rights reserved.
//
#pragma once
#ifndef Plan_hpp
#define Plan_hpp

#include <stdio.h>
#include <list>
#include "Node.hpp"

class Plan {
   public:
    Plan(Node *end);
    
    std::list<Node *> plan;
};

#endif /* Plan_hpp */
