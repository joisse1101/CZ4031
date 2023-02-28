#pragma once
using namespace std;

// Key within Node
struct Key
{
	float key_num;
	//vector<void*>duplicate_key;
};

//Index Block within Tree
class Node
{

	bool check_leaf; 	// Leaf or Internal 
	Key* keys; // Keys within the Node
	Node** child_nodes; // Children that are pointing to that node
	int num_keys; // Number of keys in node
	int max_keys; // N parameter 
	// Record *records; // For leaf nodes that will contain the actual data 
	friend class BPlusTree;
public:
	//Constructor and Functions
	Node();
	void printTest();
};

class BPlusTree 
{
	Node* root;

public: 
	// Constructor
	BPlusTree();
	// Functions for insertions
	void insert_node(Key key_obj);
	void update_internal(Key key_obj, Node* parent, Node* child);
	Node* create_new_root(Node* current, Node* new_node, Key key);
	Node* find_parent(Node* current, Node* child);
	void display_tree(Node* current);
	Node* get_root();
	
	// Calculations
	int get_max_keys();
	void get_tree_depth(Node* root);
	void get_num_nodes(Node* root);
};