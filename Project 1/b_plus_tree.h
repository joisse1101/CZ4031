#pragma once
#include <vector>
using namespace std;

// Key within Node
struct Key
{
	float key_num;
	vector<void *> addrs; // Dynamic vector that will contain duplicates
};

// Index Block within Tree
class Node
{

	bool check_leaf;	// Leaf or Internal
	Key *keys;			// Keys within the Node
	Node **child_nodes; // Children that are pointing to that node
	int num_keys;		// Number of keys in node

	int max_keys; // N parameter
	friend class BPlusTree;

public:
	// Constructor and Functions
	Node();
};

class BPlusTree
{
	Node *root;

public:
	int num_nodes;
	int depth_of_tree;

	// Constructor
	BPlusTree();

	// Functions for insertions & Deletion
	void insert_node(Key key_obj);
	void update_internal(Key key_obj, Node *parent, Node *child);
	void delete_key(float key_num);
	void delete_internal(Node *current, Node *child, float key_num);
	Node *create_new_root(Node *current, Node *new_node, Key key);
	Node *find_parent(Node *current, Node *child);
	void display_tree(Node *current);
	Node *get_root();

	// Calculations
	int get_max_keys();
	int get_tree_depth(Node *current);
	int get_num_nodes();
	void print_root_keys(Node *root);

	// search
	vector<void *> searchRange(float lowerBound, float upperBound);
	vector<void *> search(float number);
};