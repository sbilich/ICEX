//
//  MotionPlan.hpp
//  MotionPlan
//
//  Created by Sara Bilich on 6/14/16.
//  Copyright © 2016 Sara Bilich. All rights reserved.
//
#pragma once
#ifndef MotionPlan_hpp
#define MotionPlan_hpp

#include <stdio.h>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "Node.hpp"

class MotionPlan {
    public:
    MotionPlan(float x0, float x1, float y0, float y1, float z0, float z1, bool map, std::string proj_dir);
    std::vector<Node *> *getOnePath(Node *start, Node *goal);
    std::vector<Node *> *getPossiblePaths(Node *start, Node *goal);
    Node *getPath(Node *start, Node *goal);
    void addNode(Node *node);
    Node *selectNextNode();
    void createRoadMap();
    void resetRoadMap();
    Node *generateNext(Node *parent);
    void writePathToFile(FILE *file, Node *cur, int num);
    void writeNodeTable();
    char *createPath(std::string ending, int num);
    // bool isCollisionFree(Node *node);

    float x0;
    float x1;
    float y0;
    float y1;
    float z0;
    float z1;
    float range;
    float gridSize;
    float ax;
    float bx;
    float ay;
    float by;
    std::vector<int> *occupied_cells;
    std::vector<Node *> ***map;
    bool printMap;
    std::string proj_dir;
    int curIndex;
    int curPath;
    FILE *mapFile;
    std::unordered_map<Node *, int> *table;
};

#endif /* MotionPlan_hpp */
