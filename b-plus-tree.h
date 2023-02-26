#ifndef B_PLUS_TREE_H
#define B_PLUS_TREE_H

#include "memory.h"
#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

using namespace std;

struct keys_struct
{
  float key_value;         // Key value.
  vector<void *> add_vect; // List of records with same key.
};


struct Node {
    int *keys;
    Node **child_ptr;
    bool leaf;
    int n;
    friend class BPTree;
};





class BPTree {
    private:
        Node *root;
        int degree;
        Node* createNode();
        void splitNode(Node *current, Node *parent, int key);
        void insertNonFull(Node *current, int key);
    public:
        BPTree(int _degree);
        void insert(int key);
        void search(int key);
};

#endif