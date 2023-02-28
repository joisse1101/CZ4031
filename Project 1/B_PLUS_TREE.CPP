#include "b_plus_tree.h"
#include "memory.h"
#include <iostream>
#include <fstream>
#include <cmath> 

// Declare constant value
const int MAX_KEYS = 3; 
// Constructor for a node
Node::Node()
{
	max_keys = MAX_KEYS;
	keys = new Key[MAX_KEYS];
	check_leaf = true;
	child_nodes = new Node * [MAX_KEYS + 1]; 
	num_keys = 0;
	// records = new Record[MAX_KEYS];
}


void Node:: printTest()
{
	cout << "Printing for testing" << endl;
}

// Constructor for a BPlusTree

BPlusTree::BPlusTree()
{
	// Initialise the root first
	root = NULL;
}

void BPlusTree::insert_node(Key key_obj)
{
	// Tree is empty (Initialise Tree) 
	if (root == NULL)
	{
		cout << "Initialising Tree..." << endl;
		root = new Node;
		root->check_leaf = true;
		root->keys[0] = key_obj;
		root->num_keys = 1;
	}
	else
	{
		// Find the next leaf node to place key 
		Node* current = root;
		Node* parent = NULL;

		// If current node is not a leaf, traverse down to the bottom 
		while (!current->check_leaf)
		{
			cout << "Current Node is not Leaf Node... Traversing down " << endl;
			parent = current;
			int i = 0;
			while (i < current->num_keys)
			{


				// Traverse down the children of the tree
				if (key_obj.key_num < current->keys[i].key_num)
				{
					current = current->child_nodes[i];
					break;
				}
				// Node does not contain any key smaller than current key
				if (i == current->num_keys - 1)
				{
					// Move to next node
					current = current->child_nodes[i + 1];
					break;
				}

				i++;
			}
		}

		if (current->check_leaf == true)
		{
			// If Node is NOT FULL...  
			if (current-> num_keys < MAX_KEYS)
			{
				cout << "Node is not full..." << endl;
				int x = MAX_KEYS; // x = 3
				// Go through keys till not NULL
				while (trunc(current->keys[x - 1].key_num) != current->keys[x - 1].key_num ) {
					cout << "Going through keys" << endl;
					x--;
				}

				//Execute shifting to make space for new_key (In the case where the current key is greater than the new key)
				// Condition: New key is smaller than the last key in node and Node is not completely empty
				
				while (current->keys[x - 1].key_num > key_obj.key_num && x != 0) {
					// Execute Shifting...
					cout << "Executing Shifting... " << endl;
					current->keys[x] = current->keys[x - 1];
					x--;
				}
				// Slot new key in... 
				current->keys[x] = key_obj;
				current->num_keys++;
				cout << "Added new Key..." << current->keys[x].key_num << endl;
				current->child_nodes[current->num_keys] = current->child_nodes[current->num_keys - 1];
				current->child_nodes[current->num_keys - 1] = NULL;
			}
			// Leaf Node is FULL 
			else {
				cout << "Leaf Node is full..." << endl;
				// Create a new leaf node... 
				Node* new_node = new Node;
				Key temp_keys[MAX_KEYS + 1];

				// Replicate keys over to temp that can fit the extra key
				for (int i=0; i < MAX_KEYS; i++)
				{
					temp_keys[i] = current->keys[i];
				}

				// Find where the new key should be inserted... 
				// Check for the last key...
				int i = 0;
				while (key_obj.key_num > temp_keys[i].key_num && i<MAX_KEYS) 
				{
					i++; 
				}

				for (int j = current->num_keys; j > i; j--) 
				{
					temp_keys[j] = temp_keys[j - 1];
				}


				temp_keys[i] = key_obj; // Insert 6 in [3,5,10,EMPTY]Expected [3,5,6,10]

				// TBD
				for (int x = 0; x < MAX_KEYS + 1; x++) {
					cout << "temp_keys[" << x << "]:" << temp_keys[x].key_num << endl;
				}

				// Setup nodes for splitting
				current-> num_keys = (MAX_KEYS + 1) / 2; // Setting the min
				new_node-> num_keys = MAX_KEYS + 1 - current->num_keys;

				// Set the last child ptr of current to the new node 
				current->child_nodes[current->num_keys] = new_node;
				new_node->child_nodes[new_node->num_keys] = current->child_nodes[MAX_KEYS];

				// Reset the previous node pointers 
				current->child_nodes[MAX_KEYS] = NULL;

				i = 0; 
				int j = 0;
				while (i < current->num_keys)
				{
					current->keys[i] = temp_keys[i];
					//cout << "Assigning keys[i]:" << i << "}" << current->keys[i].key_num << " as temp_keys[i]:" << temp_keys[i].key_num << endl;
					i++;
				}

				while (j < new_node->num_keys)
				{
					new_node->keys[j] = temp_keys[i];
					//cout << "Assigning keys[j]:"<< j << "}" << new_node->keys[j].key_num << " as temp_keys[i]:" << temp_keys[i].key_num << endl;
					j++; i++;
				}

				// TBD
				for (int x = 0; x < current->num_keys;x++) 
				{
					cout << "Keys in current after adding key " << key_obj.key_num << ": "<< current->keys[x].key_num<< endl;
				}
				// TBD
				for (int x = 0; x < new_node->num_keys;x++)
				{
					cout << "Keys in new after adding key " << key_obj.key_num << ": " << new_node->keys[x].key_num << endl;
				}

				// Update the parent node
				if (current != root)
				{
					cout << "Inserting Internal Node..." << endl;
					update_internal(new_node->keys[0], parent, new_node);
				}

				else
				{
					cout << "New root is created..." << endl;
					root = create_new_root(current, new_node, new_node->keys[0]);
				}


			}

		}

	}

}


Node* BPlusTree::create_new_root(Node* current, Node* new_node, Key key_obj)
{
	cout << "Updated internal node values!" << endl;
	Node* new_root = new Node;
	// Set smallest node to be in parent node
	new_root->keys[0] = key_obj;
	new_root->num_keys= 1; 

	new_root->check_leaf = false;
	new_root->child_nodes[0] = current;
	new_root->child_nodes[1] = new_node;

	return new_root;

}

void BPlusTree::update_internal(Key key_obj, Node* current, Node* child) 
{
	// Check if parent node is FULL
	if (current->num_keys < MAX_KEYS)
	{
		// If parent node is NOT FULL...
		int i = 0;
		int j = current->num_keys;
		// Tranverse to the point of insertion

		while (key_obj.key_num > current->keys[i].key_num && i < current->num_keys) {
			i++;
		}

		cout << "Point of insertion for keys is:  " << i << endl;
		cout << "The Key inserting is : " << key_obj.key_num << endl;

		// Execute Shifting for both key and child node (pointer) 
		while (j > i)
		{
			current->keys[j] = current->keys[j - 1];
			current->child_nodes[j + 1] = current->child_nodes[j];
			j--;
		}

		// Update the new values
		current-> keys[i] = key_obj;
		current-> num_keys++;
		current-> child_nodes[i + 1] = child;
		
		// TBD
		for (int x = 0; x < current->num_keys; x++) {
			cout << "Keys in parent: " << current->keys[x].key_num << endl;
		}
		// TBD
		for (int x = 0; x < child->num_keys; x++) {
			cout << "Keys in child " << child->keys[x].key_num << endl;
		}
	}
	// If Parent is FULL...
	else 
	{
		cout << "Parent is FULL!!!" << endl;
		// Prepare new internal node
		Node* new_node = new Node;
		new_node->check_leaf = false;

		Key temp_keys[MAX_KEYS + 1];
		Node* temp_child[MAX_KEYS + 2];

		// Replicate the values
		// Key Values
		for (int i = 0; i < MAX_KEYS; i++) 
		{
			temp_keys[i] = current->keys[i];
		}
		// Children
		for (int i = 0; i < MAX_KEYS+1; i++)
		{
			temp_child[i] = current->child_nodes[i];
		}
		int i = 0;
		int j = MAX_KEYS;

		// Finding the point of insertion... 
		while (key_obj.key_num > temp_keys[i].key_num && i < MAX_KEYS) 
		{
			i++;
		}

		// Execute Shifting for both key and child node 
		while (j > i)
		{
			temp_keys[j] = temp_keys[j - 1];
			temp_child[j + 1] = temp_child[j];
			j--;

		}

		// Update the values
		temp_keys[i] = key_obj;
		temp_child[i + 1] = child;

		new_node->num_keys = MAX_KEYS - (MAX_KEYS + 1) / 2;
		current->num_keys = (MAX_KEYS + 1) / 2;

		// Update both keys and child_nodes
		i = 0;

		while (i < new_node->num_keys) {
			new_node->keys[i] = temp_keys[i];
			new_node->child_nodes[i] = temp_child[i];
			i++;
		}
		new_node->child_nodes[i] = temp_child[i];

		if (current != root)
		{
			update_internal(current->keys[current->num_keys], find_parent(root, current), new_node);
		}

		else 
		{
			
			root = create_new_root(current, new_node, current->keys[current->num_keys]);
		}
		
	}

}

Node* BPlusTree::find_parent(Node* current, Node* child)
{
	Node *parent = NULL;
	if (current->check_leaf == true || current->child_nodes[0]->check_leaf == true )
	{
		return NULL;
	}

	for (int i = 0; i < current-> num_keys + 1; i++) 
	{
		if (current->child_nodes[i] == child) 
		{
			return current;
		}
		else 
		{
			parent = find_parent(current->child_nodes[i], child);
			if (parent != NULL) 
				return parent;
		}

	}

	return parent;
}

void BPlusTree::display_tree(Node* current) 
{

	cout << "Root node: " << endl;
	for (int i = 0; i < current->num_keys; i++)
	{
		cout << "|" << current->keys[i].key_num << "|";
	}

	Node* child; 
	cout << endl;

	for (int i = 0; i < current->num_keys + 1; i++)
	{
		child = current->child_nodes[i];
		if (child != NULL)
		{
			cout << "Child Node " << i << ":" << endl;
			for (int j = 0; j < child->num_keys; j++)
			{
				cout << child->keys[j].key_num << "|";
			}
		}
		cout << endl;
	}
}

Node* BPlusTree::get_root() 
{
	return root;
}

int BPlusTree::get_max_keys()
{
	return MAX_KEYS;
}
void BPlusTree:: get_tree_depth(Node* root)
{
	cout << "In Progress" << endl;
}
void BPlusTree:: get_num_nodes(Node* root)
{
	cout << "In Progress" << endl;
}


