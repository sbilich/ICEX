//
//  main.cpp
//  MotionPlan
//
//  Created by Sara Bilich on 6/14/16.
//  Copyright © 2016 Sara Bilich. All rights reserved.
//

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include "MotionPlan.hpp"
#include "Plan.hpp"
using namespace std;

string RESOURCE_DIR; // Where the resources are loaded from

static vector<Plan *> *toPlans(vector<Node *> *paths) {
    vector<Plan *> *plans = new vector<Plan *>;
    for (int i = 0; i < paths->size(); i++) {
        Plan *plan(new Plan((*paths)[i]));
        plans->push_back(plan);
    }
    return plans;
}

static void printPlansToFiles(vector<Plan *> *plans) {
    for (int i = 0; i < plans->size(); i++) {
        char name[100];
        // sprintf(name, "/Users/sarabilich/Documents/ICEX/MotionPlan/MotionPlan/plans/plan%d.txt", i);
        // sprintf(name, "/Users/katiedavis/Documents/ICEX/Camera/ICEX/MotionPlan/MotionPlan/plans/plan%d.txt", i);
        sprintf(name, "plans/plan%d.txt", i);
        
        // char *ending_str = (char *)ending.c_str();
        // sprintf(name, "plans/plan%d.txt", num);

        FILE *file = fopen(string(RESOURCE_DIR + name).c_str(), "w");
        list<Node *> plan = (* plans)[i]->plan;
        
        for (list<Node *>::iterator j = plan.begin(); j != plan.end(); ++j) {
            fprintf(file, "(%.2f, %.2f)\n", (*j)->getX(), (*j)->getY());
        }
        fclose(file);
    }
}

int main(int argc, const char **argv) {
    if(argc < 2) {
        cout << "Please specify the resource directory." << endl;
        return 0;
    }
    RESOURCE_DIR = argv[1] + string("/");
   
    MotionPlan prm = MotionPlan(-1, 1, -1, 1, 0, 0, 1, RESOURCE_DIR);
    Node *start(new Node(-1, -1, 0, 0, 0, 0, NULL));
    Node *goal(new Node(1, 1, 0, 0, 0, 0, NULL));
    
    vector<Node *> *paths = prm.getPossiblePaths(start, goal);
    //vector<Node *> *paths = prm.getOnePath(start, goal);
    vector<Plan *> *plans = toPlans(paths);
    printPlansToFiles(plans);
    exit(EXIT_SUCCESS);
}
