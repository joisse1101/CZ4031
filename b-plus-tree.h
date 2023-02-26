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

    public : 
    Node();
};





class BPTree {
    private:
        Node *root;

        void insertInternal(keys_struct x, Node *cursor, Node *child);
        int removeInternal(keys_struct x, Node *cursor, Node *child);
        Node *findParent(Node *cursor, Node *child);


    public:
        BPTree();
        void searchIndividual(float key);
        void searchRange(float lowerKeyBound, float upperKeyBound);
        void insert(keys_struct x);
        void remove(keys_struct x);



};

#endif