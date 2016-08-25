//
//  MotionPlan.cpp
//  MotionPlan
//
//  Created by Sara Bilich on 6/14/16.
//  Copyright © 2016 Sara Bilich. All rights reserved.
//

#include "MotionPlan.hpp"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
using namespace std;

int randInt(int l, int h)
{
    float r = rand() / (float)RAND_MAX;
    return (1.0f - r) * l + r * h;
}

float randFloat(float l, float h)
{
    float r = rand() / (float)RAND_MAX;
    return (1.0f - r) * l + r * h;
}

MotionPlan::MotionPlan(float x0, float x1, float y0, float y1, float z0, float z1, bool map) :
   x0(x0),
   x1(x1),
   y0(y0),
   y1(y1),
   z0(z0),
   z1(z1),
   gridSize(24.0),
   range(2.0),
   curIndex(0),
   curPath(0),
   mapFile(NULL),
   printMap(map)
{
    table = new unordered_map<Node *, int>();
    occupied_cells = new vector<int>();
    ax = (gridSize - 1) / (fabs(x0) + x1);
    bx = (gridSize - 1) / 2.0;
    ay = (gridSize - 1) / (fabs(y0) + y1);
    by = (gridSize - 1) / 2.0;
    createRoadMap();
}

/**
 * Goes through a linked list of Nodes and marks them as part of a path.
 */
void markAsPath(Node *path) {
    Node *cur = path;
    while (cur) {
        cur->markInPath();
        cur = cur->getParent();
    }
}

/**
 * Returns the distance from |start| to |end|.
 */
float getDistanceFrom(Node *start, Node *end) {
    float dx = pow(start->getX() - end->getX(), 2.0);
    float dy = pow(start->getY() - end->getY(), 2.0);
    float dz = pow(start->getZ() - end->getZ(), 2.0);
    return pow(dx + dy + dz, 0.5);
}

/**
 * Returns the distance from |node| to it's root.
 */
float getDistance(Node *node) {
    float distance = 0;
    Node *cur = node;
    
    while (cur->getParent()) {
        Node *parent = cur->getParent();
        float dx = pow(cur->getX() - parent->getX(), 2.0);
        float dy = pow(cur->getY() - parent->getY(), 2.0);
        float dz = pow(cur->getZ() - parent->getZ(), 2.0);
        distance += pow(dx + dy + dz, 0.5);
        cur = cur->getParent();
    }
    
    return distance;
}


/**
 * Generates a path from |start| to |goal|, returns a vector of a pointer to a linked-list
 * of Node structs, representing the path from |start| to |goal|.
 */
vector<Node *> *MotionPlan::getOnePath(Node *start, Node *goal)
{
    Node *path = NULL;
    float cur_dist = getDistanceFrom(start, goal) * 1.360;
    bool timeout = false;
    bool found = false;
    time_t timer;
    time(&timer);
    time_t cur;
    int tries = 6;
    const double TIME_ELAPSED = tries * 60.0; // in seconds
    
    while (!found && !timeout) {
        Node *cur_path = this->getPath(start, goal);
        if (cur_path && (getDistance(cur_path) < cur_dist)) { // if path found
            time(&cur);
            printf("Time elapsed is %f\n", difftime(cur, timer));
            found = true;
            path = cur_path;
        }
        
        if (printMap) {
            this->writeNodeTable();
        }
        this->resetRoadMap();
        curPath++;
        
        time(&cur);
        if (difftime(cur, timer) > TIME_ELAPSED) {
            timeout = true;
        }
    }
    
    auto *final = new vector<Node *>();
    final->push_back(path);
    
    if (path == NULL)
        printf("No path found that is less than Euclidean distance\n");
    else
        printf("found a path\n");
    
    printf("Num of paths searched %d\n", curPath);
    return final;
}

/**
 * Generates paths from |start| to |goal|, returns a vector of pointers to linked-lists
 * of Node structs, representing the path from |start| to |goal|.
 */
vector<Node *> *MotionPlan::getPossiblePaths(Node *start, Node *goal)
{
    auto *paths = new vector<Node *>();
    bool timeout = false;
    time_t timer;
    time(&timer);
    time_t cur;
    int tries = 6;
    const double TIME_ELAPSED = tries * 60.0; // in seconds
    
    while (!timeout) {
        char name[100];
        sprintf(name, "/Users/sarabilich/Documents/ICEX/MotionPlan/MotionPlan/roadmaps/roadmap%d.txt", curPath);
        mapFile = fopen(name, "w");
        if (mapFile == NULL) {
            perror("Error");
        }
        Node *path = this->getPath(start, goal);
        
        fclose(mapFile);
        
        if (path) { // if path found
            paths->push_back(path);
        }
        
        if (printMap) {
            this->writeNodeTable();
        }
        this->resetRoadMap();
        curPath++;
        
        time(&cur);
        if (difftime(cur, timer) > TIME_ELAPSED) {
            timeout = true;
        }
    }
    return paths;
}

/**
 * Generates a path from |start| to |goal|, by returning a linked-list
 * of Node structs, representing the path from |start| to |goal|. The first Node
 * in the linked-list represents |start| and the last represents |goal|. If a path cannot
 * be determined, it returns NULL.
 */
Node *MotionPlan::getPath(Node *start, Node *goal) {
    Node *path = NULL;
    this->addNode(start);
    
    bool timeout = false;
    time_t timer;
    time(&timer);
    time_t cur;
    const double TIME_ELAPSED = 180.0; // in seconds
    
    while (!timeout) {
        auto curNode = this->selectNextNode();
        auto newNode = this->generateNext(curNode);
        
        //if (this->isCollisionFree(newNode)) {
        this->addNode(newNode);
        if ((newNode)->inRangeOf(goal)) { // if reached goal
            path = newNode; // found path
            markAsPath(path);
            break;
        }
        
        time(&cur);
//        printf("Time is %f\n", difftime(cur, timer));
        if (difftime(cur, timer) > TIME_ELAPSED) {
            timeout = true;
        }
        // }
    }
    return path;
}

/**
 * Adds |node| to its position in |map|.
 */
void MotionPlan::addNode(Node *node) {
    if (node->getParent())
        (node->getParent())->increaseChildCount();
    
    int x_ndx = (ax * node->getX()) + bx;
    int y_ndx = (ay * node->getY()) + by;
    
    if (map[x_ndx][y_ndx] == NULL) {
        vector<Node *> *newVec = new vector<Node *>();
        map[x_ndx][y_ndx] = newVec;
        
        // add cell to list of occupied cells
        occupied_cells->push_back(y_ndx * gridSize + x_ndx);
    }
    
    auto node_list = map[x_ndx][y_ndx];
    node_list->push_back(node);
    
    if (printMap) {
        table->emplace(node, curIndex);
        fprintf(mapFile, "(%.2f, %.2f)  -  %d\n", (node)->getX(), (node)->getY(), curIndex);
        curIndex++;
    }
    
//    printf("Added node with (%f, %f) to (%d, %d)\n", node->getX(), node->getY(), x_ndx, y_ndx);
}

/**
 * Randomly selects a Node from |map| to expand off of and returns
 * a pointer to that Node.
 */
Node *MotionPlan::selectNextNode() {
    vector<Node *> *cell;
    
    int rand_cell = randInt(0, (int) occupied_cells->size() - 1);
    int index = (* occupied_cells)[rand_cell];
    int x_ndx = index % (int)gridSize;
    int y_ndx = index / (int)gridSize;
    
//    printf("Num of occupied cells: %d\n", (int) occupied_cells->size());
//    printf("rand cell is %d\n", rand_cell);
    cell = map[x_ndx][y_ndx];
//    printf("Cell at (%d, %d) has %d nodes\n", x_ndx, y_ndx, (int)cell->size());
    
    int node_ndx = randInt(0, (int)cell->size() - 1);
//    printf("Selected Node: (%f, %f)\n", (*cell)[node_ndx]->getX(), (*cell)[node_ndx]->getY());
    
    return (*cell)[node_ndx];
}

/**
 * Generates a new Node, in |range| of and connected to |parent|, and returns
 * a pointer to that Node.
 */
Node *MotionPlan::generateNext(Node *parent) {
    // randomly generate next node within a range of the parent.
    int lx, hx, ly, hy;
    
    // get lowest value possible for newX
    if (fabs(parent->getX() - x0) == 0) { // check distance between pos and lower bound of x-range
        lx = x0;
    } else {
        lx = max(parent->getX() - range, x0);
    }
    
    // get highest value possible for newX
    if (fabs(parent->getX() - x1) == 0) { // check distance between pos and higher bound of x-range
        hx = x1;
    } else {
        hx = min(parent->getX() + range, x1);
    }
    
    // get lowest value possible for newY
    if (fabs(parent->getY() - y0) == 0) { // check distance between pos and lower bound of x-range
        ly = y0;
    } else {
        ly = max(parent->getY() - range, y0);
    }
    
    // get highest value possible for newY
    if (fabs(parent->getY() - y1) == 0) { // check distance between pos and higher bound of x-range
        hy = y1;
    } else {
        hy = min(parent->getY() + range, y1);
    }
    
    float newX = randFloat(lx, hx);
    float newY = randFloat(ly, hy);
    
    //printf("Generated Node: (%f, %f)\n", newX, newY);
    Node *next = new Node(newX, newY, parent->getZ(), parent->getYaw(), parent->getPitch(), parent->getRoll(), parent);
    return next;
}

/**
 * Create and initialize a new roadMap, |map|.
 */
void MotionPlan::createRoadMap() {
    map = new vector<Node  *> **[(int)gridSize];
    for (int i = 0; i < (int)gridSize; i++) {
        map[i] = (vector<Node *> **) calloc((int)gridSize, sizeof(vector<Node *> **));
    }
}

/**
 * Clears the MotionPlan's roadMap, |map|.
 */
void MotionPlan::resetRoadMap() {
    for (int i = 0; i < (int)gridSize; i++) {
        for (int j = 0; j < (int)gridSize; j++) {
            if (map[i][j]) {
                // Delete Nodes in the vector.
                for (int k = 0; k < (int)((map[i][j])->size()); k++) {
                    // Delete Nodes that aren't in the path.
                    if (!((*map[i][j])[k]->isPath()))
                        delete (*map[i][j])[k];
                }
                delete map[i][j];
                map[i][j] = NULL;
            }
        }
    }
    occupied_cells->clear();
    
    if (printMap) {
        table->clear();
    }
}

/**
 * Writes the path starting from |cur|, the leaf node, up to it's root to the file, |file|, and marks it with
 * path number, |num|.
 */
void MotionPlan::writePathToFile(FILE *file, Node *node, int num) {
    Node *cur = node;
    fprintf(file, "Path%d:\n", num);
    fprintf(file, "number of indices: %d\n", cur->getLength());
    
    while (cur) {
        fprintf(file, "%d\n", table->at(cur));
        cur = cur->getParent();
    }
    fprintf(file, "end\n");
}

/**
 * Traverses roadmap to create a map of Nodes.
 */
void MotionPlan::writeNodeTable() {
    char name[100];
    sprintf(name, "/Users/sarabilich/Documents/ICEX/MotionPlan/MotionPlan/paths/paths%d.txt", curPath);
    FILE *path_file = fopen(name, "w");
    int num = 0;
    
    for (auto i = table->begin(); i != table->end(); ++i) {
        Node *cur = (*i).first;
        
        if (cur->getChildren() == 0) {
            writePathToFile(path_file, cur, num);
            num++;
        }
    }
    
    fclose(path_file);
    
    char name2[100];
    sprintf(name2, "/Users/sarabilich/Documents/ICEX/MotionPlan/MotionPlan/info/info%d.txt", curPath);
    FILE *info_file = fopen(name2, "w");
    
    fprintf(info_file, "%d\n", curIndex); // write num of vertices
    fclose(info_file);
    
    curIndex = 0;
}

// TODO: Implement collision checking
///**
// * Returns true if the edge from |node|'s parent to |node| doesn't collide with the environment 
// * and false if the edge has at least one collision.
// */
//bool MotionPlan::isCollisionFree(Node *node) {
//    // TO IMPLEMENT
//    bool collision = false;
//}