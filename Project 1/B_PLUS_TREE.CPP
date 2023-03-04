#include "b_plus_tree.h"
#include <iostream>
#include <fstream>
#include <cmath>

// Declare constant value
const int MAX_KEYS = 7;
// Constructor for a node
Node::Node()
{
	max_keys = MAX_KEYS;
	keys = new Key[MAX_KEYS];
	check_leaf = true;
	child_nodes = new Node *[MAX_KEYS + 1];
	num_keys = 0;
}

// Constructor for a BPlusTree

BPlusTree::BPlusTree()
{
	// Initialise the root first
	root = NULL;
	num_nodes = 0;
}

void BPlusTree::insert_node(Key key_obj)
{
	// cout << "Inserting Key " << key_obj.key_num << " into the tree... -----------------" << endl;
	// Tree is empty (Initialise Tree)
	if (root == NULL)
	{
		cout << "Initialising Tree..." << endl;
		root = new Node;
		root->check_leaf = true;
		root->keys[0] = key_obj;
		root->num_keys = 1;
		num_nodes++;
	}
	else
	{
		// Find the next leaf node to place key
		Node *current = root;
		Node *parent = NULL;

		// If current node is not a leaf, traverse down to the bottom
		while (!current->check_leaf)
		{
			// cout << "Current Node is not Leaf Node... Traversing down " << endl;
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
				if (i == current->num_keys - 1 || key_obj.key_num == current->keys[i].key_num)
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
			if (current->num_keys < MAX_KEYS)
			{
				// Check for duplicates
				for (int i = 0; i < current->num_keys; i++)
				{
					if (key_obj.key_num == current->keys[i].key_num)
					{
						// cout << "Detected Duplicates!" << endl;
						current->keys[i].addrs.push_back(key_obj.addrs[0]);
						return;
					}
				}
				// cout << "Node is not full..." << endl;
				int x = 0;
				int i = current->num_keys;

				// Go through keys to find point of insertion
				while (key_obj.key_num > current->keys[x].key_num && x < current->num_keys)
				{
					x++;
				}

				// Execute shifting to make space for new_key (In the case where the current key is greater than the new key)
				while (i > x)
				{
					// Execute Shifting...
					current->keys[i] = current->keys[i - 1];
					i--;
				}

				// Slot new key in...
				current->keys[x] = key_obj;
				current->num_keys++;
				current->child_nodes[current->num_keys] = current->child_nodes[current->num_keys - 1];
				current->child_nodes[current->num_keys - 1] = NULL;
			}
			// Leaf Node is FULL
			else
			{
				// cout << "Leaf Node is full..." << endl;
				// Create a new leaf node...
				Node *new_node = new Node;
				Key temp_keys[MAX_KEYS + 1];

				// Replicate keys over to temp that can fit the extra key
				for (int i = 0; i < MAX_KEYS; i++)
				{
					temp_keys[i] = current->keys[i];
				}
				// Find where the new key should be inserted...
				// Check for the last key...
				int i = 0;
				while (key_obj.key_num > temp_keys[i].key_num && i < MAX_KEYS)
				{
					i++;
				}

				for (int j = current->num_keys; j > i; j--)
				{
					temp_keys[j] = temp_keys[j - 1];
				}

				temp_keys[i] = key_obj; // Insert 6 in [3,5,10,EMPTY]Expected [3,5,6,10]

				// Setup nodes for splitting
				current->num_keys = floor((MAX_KEYS + 1) / 2); // Setting the min
				new_node->num_keys = MAX_KEYS + 1 - current->num_keys;

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
					// cout << "Assigning keys[i]:" << i << "}" << current->keys[i].key_num << " as temp_keys[i]:" << temp_keys[i].key_num << endl;
					i++;
				}

				while (j < new_node->num_keys)
				{
					new_node->keys[j] = temp_keys[i];
					// cout << "Assigning keys[j]:"<< j << "}" << new_node->keys[j].key_num << " as temp_keys[i]:" << temp_keys[i].key_num << endl;
					j++;
					i++;
				}

				num_nodes++;
				// Update the parent node
				if (current == root)
				{
					// cout << "New root is created..." << endl;
					// cout << "Create new node for node " << "Keys:" << new_node->keys[0].key_num <<endl;
					root = create_new_root(current, new_node, new_node->keys[0]);
				}

				else
				{
					update_internal(new_node->keys[0], parent, new_node);
				}
			}
		}
	}
}

Node *BPlusTree::create_new_root(Node *current, Node *new_node, Key key_obj)
{
	num_nodes++;
	Node *new_root = new Node;
	// cout << "Creating new root..." << endl;
	new_root->keys[0] = key_obj;
	new_root->num_keys = 1;
	new_root->check_leaf = false;
	new_root->child_nodes[0] = current;
	new_root->child_nodes[1] = new_node;
	// cout << "Setting " << new_root->keys[0].key_num << " as first key in root" << endl;

	return new_root;
}

void BPlusTree::update_internal(Key key_obj, Node *current, Node *child)
{
	// Check if parent node is FULL
	if (current->num_keys < MAX_KEYS)
	{
		// If parent node is NOT FULL...
		int i = 0;
		int j = current->num_keys;
		// Tranverse to the point of insertion
		while (key_obj.key_num > current->keys[i].key_num && i < current->num_keys)
		{
			i++;
		}

		// cout << "Point of insertion for keys is:  " << i << endl;
		// cout << "The Key inserting is : " << key_obj.key_num << endl;

		// Execute Shifting for both key and child node (pointer)
		while (j > i)
		{
			current->keys[j] = current->keys[j - 1];
			current->child_nodes[j + 1] = current->child_nodes[j];
			j--;
		}

		// Update the new values
		current->keys[i] = key_obj;
		current->num_keys++;
		current->child_nodes[i + 1] = child;
	}

	else
	{
		// If Parent is FULL...
		// cout << "Parent is FULL!!!----------------" << endl;
		// Prepare new internal node
		Node *new_internal = new Node;
		new_internal->check_leaf = false;

		Key temp_keys[MAX_KEYS + 1];
		Node *temp_child[MAX_KEYS + 2];

		// Replicate the values
		// Key Values
		for (int i = 0; i < MAX_KEYS; i++)
		{
			temp_keys[i] = current->keys[i];
			// cout << "Keys within parent node is: " << temp_keys[i].key_num << endl;
		}
		// Children
		for (int i = 0; i < MAX_KEYS + 1; i++)
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

		// cout << "Point of insertion is: "<< i << endl;

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

		new_internal->num_keys = MAX_KEYS - floor((MAX_KEYS + 1) / 2);
		current->num_keys = floor((MAX_KEYS + 1) / 2);

		// Update both keys and child_nodes for internal node (CURRENT AND NEW_INTERNAL)
		i = 0;
		j = current->num_keys + 1;

		while (i < j)
		{
			current->keys[i] = temp_keys[i];
			current->child_nodes[i] = temp_child[i];
			i++;
		}

		i = 0;
		j = current->num_keys + 1;
		while (i < new_internal->num_keys)
		{
			new_internal->keys[i] = temp_keys[j];
			new_internal->child_nodes[i] = temp_child[j];
			i++;
			j++;
		}
		new_internal->child_nodes[new_internal->num_keys] = temp_child[j];
		num_nodes++;

		if (current == root)
		{
			// cout << "Create new root for internal node " << "Keys:" << current->keys[current->num_keys].key_num <<endl;
			// cout << "Created a new root from internal" << endl;
			root = create_new_root(current, new_internal, current->keys[current->num_keys]);
		}

		else
		{
			// cout << "Current is not root..." << endl;
			update_internal(current->keys[current->num_keys], find_parent(root, current), new_internal);
		}
	}
}

Node *BPlusTree::find_parent(Node *current, Node *child)
{
	Node *parent;
	if (current->check_leaf == true || current->child_nodes[0]->check_leaf == true)
	{
		return NULL;
	}

	for (int i = 0; i < current->num_keys + 1; i++)
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

void BPlusTree::display_tree(Node *current)
{

	cout << "Root node: " << endl;
	for (int i = 0; i < current->num_keys; i++)
	{
		cout << "|" << current->keys[i].key_num << "|";
	}

	Node *child;
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

Node *BPlusTree::get_root()
{
	return root;
}

int BPlusTree::get_max_keys()
{
	return MAX_KEYS;
}
int BPlusTree::get_tree_depth(Node *current)
{
	int depth = 0;
	while (current->check_leaf == false)
	{
		current = current->child_nodes[0];
		depth++;
	}

	return depth;
}
int BPlusTree::get_num_nodes()
{
	return num_nodes;
}

void BPlusTree::print_root_keys(Node *root)
{
	for (int i = 0; i < root->num_keys; i++)
	{
		cout << root->keys[i].key_num << " | ";
	}
	cout << '\n';
}

// void BPlusTree :: searchRange(float lowerBound, float upperBound)
// {
// 	int indexNodes= 0 ;
// 	if(root = NULL){
// 		throw std::logic_error("Tree is empty");

// 	}
// 	else{

// 		Node *current = root;
// 		int level = 0;
// 		while(!current->check_leaf){
// 			cout << "Index node at level: " << level << endl;
// 			for(int k=0 ; k < current->num_keys ; k++){
// 				cout << current -> keys[k].key_num<< "|";
// 			}
// 			cout << endl;
// 			indexNodes++;
// 			if (lowerBound >= current->keys[current -> num_keys-1].key_num){
// 				current = current->child_nodes[current->num_keys];

// 			}
// 			else{
// 				int i =0;
// 				while(lowerBound >= current -> keys[i].key_num)
// 					i++;
// 				current = current->child_nodes[i];
// 			}
// 			level++;

// 		}
// 		cout << "Number of index nodes accessed: " << indexNodes << endl;
// 		int i=0;
// 		while(current-> keys[i].key_num<=upperBound){
// 			for(int j=0 ; j< current -> keys[i].add_vect[j])
// 		}

// 	}
// }

void BPlusTree::delete_key(float key_num)
{
	// Case 1: Key is only @ leaf node & Min key requirement is hit after removal
	Node *current = root;
	Node *parent = NULL;
	int left_index, right_index;

	while (!current->check_leaf)
	{
		cout << "DELETION: Current Node is not Leaf Node... Traversing down " << endl;
		parent = current;
		int i = 0;
		while (i < current->num_keys)
		{
			// Traverse down the children of the tree
			if (key_num < current->keys[i].key_num)
			{
				left_index = i - 1;
				right_index = i + 1;
				current = current->child_nodes[i];
				break;
			}
			// Node does not contain any key smaller than current key
			if (i == current->num_keys - 1 || key_num == current->keys[i].key_num)
			{
				// Move to next node
				left_index = i;
				right_index = i + 2;
				current = current->child_nodes[i + 1];
				break;
			}

			i++;
		}
	}

	if (current->check_leaf)
	{
		int x = 0;
		// Find position of key to be removed
		while (x < current->num_keys)
		{
			if (current->keys[x].key_num == key_num)
			{
				cout << "Key has been found at " << x << endl;
				break;
			}
			x++;
		}

		if (x == MAX_KEYS)
		{
			cout << "Key has not been found!" << endl;
			return;
		}

		// Execute shifting
		while (x < current->num_keys - 1)
		{
			cout << "Shifting index " << x << " to " << x + 1 << endl;

			current->keys[x] = current->keys[x + 1];
			x++;
		}
		current->num_keys--;
		// if the leaf node is the root node...
		if (current == root)
		{
			if (current->num_keys == 0)
			{
				cout << "There are no more keys in this tree... INVALID TREE" << endl;
				delete[] current->keys;
				delete[] current->child_nodes;
				root = NULL;
			}
			else
			{
				for (int i = 0; i < MAX_KEYS + 1; i++)
				{
					// Update the children pointers
					current->child_nodes[i] = NULL;
				}
			}
		}
		// if the leaf node is NOT the root node...
		else
		{
			// Update the children pointers (Children pointing to next block)
			current->child_nodes[current->num_keys] = current->child_nodes[current->num_keys + 1];
			current->child_nodes[current->num_keys + 1] = NULL;

			// Check if min key requirement is kept, if not kept, the tree has to be tweaked.
			if (current->num_keys >= (MAX_KEYS + 1) / 2)
			{
				cout << "Key " << key_num << " has been successfully deleted. " << endl;
				return;
			}

			else
			{ // Leaf does not meet requirement
				cout << "Number of key does not meet requirements... Borrowing from the left/right nodes ------" << endl;
				// Check if there is a left node available
				if (left_index >= 0)
				{
					Node *left_node = parent->child_nodes[left_index];

					// Check if the left node has a spare key to provide...
					if (left_node->num_keys > floor((MAX_KEYS + 1) / 2))
					{
						// Shifting key and pointers to make space for the new key and pointers
						// Taking key over from the left side...
						for (int x = current->num_keys; x > 0; x--)
						{
							current->keys[x] = current->keys[x - 1];
						}

						// Updating current
						current->keys[0] = left_node->keys[left_node->num_keys - 1];
						current->child_nodes[current->num_keys + 1] = current->child_nodes[current->num_keys];
						current->child_nodes[current->num_keys] = NULL;
						current->num_keys++;

						// Updating left
						//  left_node->keys[left_node->num_keys-1] = NULL;
						left_node->child_nodes[left_node->num_keys - 1] = current;
						left_node->child_nodes[left_node->num_keys] = NULL;
						left_node->num_keys--;

						// Updating parent to be smallest value
						parent->keys[left_index] = current->keys[0];
						cout << "Took a key from LEFT NODE!" << endl;
						return;
					}
				}

				// Check if there is a right node available
				else if (right_index <= parent->num_keys)
				{
					Node *right_node = parent->child_nodes[right_index];

					if (right_node->num_keys > floor((MAX_KEYS + 1) / 2))
					{
						// Take key from right node
						// Update current
						current->keys[current->num_keys] = right_node->keys[0];
						current->child_nodes[current->num_keys + 1] = current->child_nodes[current->num_keys];
						current->child_nodes[current->num_keys] = NULL;
						current->num_keys--;

						// Update right node
						// Shift the node to remove the key on most left
						for (int x = 0; x < right_node->num_keys - 1; x++)
						{
							right_node->keys[x] = right_node->keys[x + 1];
						}
						// delete right_node->keys[right_node->num_keys-1];
						right_node->child_nodes[right_node->num_keys - 1] = right_node->child_nodes[right_node->num_keys];
						right_node->child_nodes[right_node->num_keys] = NULL;

						// Updating parent to be smallest value
						parent->keys[left_index] = right_node->keys[0];
						cout << "Took a key from RIGHT NODE!" << endl;
						return;
					}
				}
			}

			// No available keys to lend... Merging is required
			if (left_index >= 0)
			{
				cout << "BASE: Merging Left << Current... " << endl;
				Node *left_node = parent->child_nodes[left_index];

				// Merging Left << Current
				for (int i = left_node->num_keys, x = 0; x < current->num_keys; i++, x++)
				{
					left_node->keys[i] = current->keys[x];
				}
				// Update values
				left_node->child_nodes[left_node->num_keys] = NULL;
				left_node->num_keys += current->num_keys;
				left_node->child_nodes[left_node->num_keys] = current->child_nodes[current->num_keys];
				delete_internal(parent, current, parent->keys[left_index].key_num);
				delete[] current->keys;
				delete[] current->child_nodes;
				delete current;
				num_nodes--;
			}
			else if (right_index <= parent->num_keys)
			{
				Node *right_node = parent->child_nodes[right_index];
				// Merging Current << Right
				cout << "BASE: Merging Current << Right... " << endl;
				// Shifting the keys over...
				// Updating keys
				int x = 0, i = current->num_keys;
				while (x < right_node->num_keys)
				{
					current->keys[i] = right_node->keys[x];
					i++;
					x++;
				}

				// Updating child_nodes
				current->child_nodes[current->num_keys] = NULL;
				current->num_keys += right_node->num_keys;
				current->child_nodes[current->num_keys] = right_node->child_nodes[right_node->num_keys];
				delete_internal(parent, right_node, right_node->keys[0].key_num);
				delete[] right_node->keys;
				delete[] right_node->child_nodes;
				delete current;
				num_nodes--;
			}
		}
	}
}

void BPlusTree::delete_internal(Node *current, Node *child, float key_num)
{
	// Delete internal nodes
	cout << "Updating internal nodes!!" << endl;

	if (current != root)
	{
		// Get position of key and child that is going to be removed
		int loc = 0;
		while (current->keys[loc].key_num != key_num)
		{
			loc++;
		}

		// Carry out shifting for keys...
		for (int i = loc; i < current->num_keys - 1; i++)
		{
			current->keys[i] = current->keys[i + 1];
		}

		loc = 0;
		while (current->child_nodes[loc] != child)
		{
			loc++;
		}

		// Carry out shifting for child
		for (int i = loc; i < current->num_keys; i++)
		{
			current->child_nodes[i] = current->child_nodes[i + 1];
		}

		current->num_keys--;

		// Check if the internal nodes meet its requirement after removal.
		if (current->num_keys < floor(MAX_KEYS / 2) && current != root)
		{
			// Does not meet requirement, therefore, check neighbour nodes.
			// Find parent of parent
			cout << "Internal Parent doesnt meet requirement!" << endl;
			Node *parent = find_parent(root, current);
			// Check if you can borrow from left and right nodes

			int child_index = 0;
			// Get position of child node
			while (parent->child_nodes[child_index] != current)
			{
				child_index++;
			}

			// Get left and right nodes (Parent)
			int parent_left_index = child_index - 1;
			int parent_right_index = child_index + 1;
			cout << "Position of child in parent is: " << child_index << endl;
			cout << "Both neighbour indexes are " << parent_left_index << " & " << parent_right_index << endl;

			// Check if there is a left node for the parent
			if (parent_left_index >= 0)
			{
				Node *left_node = parent->child_nodes[parent_left_index];

				// See if there is a key to borrow from the left node
				if (left_node->num_keys >= (floor(MAX_KEYS / 2) + 1))
				{

					// Shifting keys and child to make space for taken key
					current->child_nodes[current->num_keys + 1] = current->child_nodes[current->num_keys];
					for (int x = current->num_keys; x > 0; x--)
					{
						current->keys[x] = current->keys[x - 1];
						current->child_nodes[x] = current->child_nodes[x - 1];
					}

					// Update keys and child of parent (Taking the smallest node) - Since its internal node...
					current->keys[0] = parent->keys[parent_left_index];
					current->child_nodes[0] = left_node->child_nodes[left_node->num_keys];
					current->num_keys++;

					// Update left node
					left_node->child_nodes[left_node->num_keys] = NULL;
					left_node->num_keys--;

					// Update parent of parent
					parent->keys[parent_left_index] = left_node->keys[left_node->num_keys];
					cout << "Took a key " << current->keys[0].key_num << " from LEFT NODE!" << endl;
					return;
				}
			}

			// There is a right node existing...
			else if (parent_right_index <= parent->num_keys)
			{
				Node *right_node = parent->child_nodes[parent_right_index];

				// Right node has a key that is available
				// TBD
				for (int x = 0; x < right_node->num_keys; x++)
				{
					cout << "Number of keys(Right node) " << right_node->num_keys << endl;
					cout << "Right node has key " << right_node->keys[x].key_num << endl;
				}

				if (right_node->num_keys >= (floor(MAX_KEYS / 2) + 1))
				{
					cout << "Right node has keys to offer" << endl;
					// Add the taken right key into into the current (Smallest)
					current->keys[current->num_keys] = parent->keys[child_index];
					current->child_nodes[current->num_keys + 1] = right_node->child_nodes[0];

					// Update parent...
					parent->keys[child_index] = right_node->keys[0];

					// Carry out shifting for the right keys and pointers since first key was taken away...
					right_node->child_nodes[right_node->num_keys - 1] = right_node->child_nodes[right_node->num_keys];

					for (int i = 0; i < right_node->num_keys - 1; i++)
					{
						right_node->keys[i] = right_node->keys[i + 1];
						right_node->child_nodes[i] = right_node->child_nodes[i + 1];
					}

					cout << "Took a key " << current->keys[current->num_keys].key_num << " from RIGHT NODE!" << endl;

					current->num_keys++;
					right_node->num_keys--;
					return;
				}
			}

			// Borrowing from neighbouring nodes not possible, merging instead...

			cout << "INTERNAL: Borrowing from neighbour not possible... Merging instead... " << endl;
			if (parent_left_index >= 0)
			{
				cout << "INTERNAL: Merging LEFT << CURRENT" << endl;
				Node *left_node = parent->child_nodes[parent_left_index];
				left_node->keys[left_node->num_keys] = parent->keys[parent_left_index];

				// Take first key from parent...& fill up remaining
				for (int i = left_node->num_keys + 1, x = 0; x < current->num_keys; x++, i++)
				{
					left_node->keys[i] = current->keys[x];
				}

				for (int i = left_node->num_keys + 1, x = 0; x < current->num_keys + 1; x++, i++)
				{
					left_node->child_nodes[i] = current->child_nodes[x];
					current->child_nodes[x] = NULL;
				}

				left_node->num_keys += current->num_keys + 1;
				current->num_keys = 0;
				num_nodes--;
				delete_internal(parent, current, parent->keys[parent_left_index].key_num);
			}

			// Looking at right instead...
			else if (parent_right_index <= parent->num_keys)
			{
				cout << "INTERNAL: Merging CURRENT << RIGHT" << endl;
				Node *right_node = parent->child_nodes[parent_right_index];

				// No nodes available for lending... Merging with right node then... (Current << Right)
				current->keys[current->num_keys] = parent->keys[parent_right_index - 1];

				// Take first key from parent...& fill up remaining
				for (int i = current->num_keys + 1, x = 0; x < right_node->num_keys; x++, i++)
				{
					current->keys[i] = right_node->keys[x];
				}

				for (int i = current->num_keys + 1, x = 0; x < right_node->num_keys + 1; x++, i++)
				{
					current->child_nodes[i] = right_node->child_nodes[x];
					right_node->child_nodes[x] = NULL;
				}

				current->num_keys += right_node->num_keys + 1;
				right_node->num_keys = 0;
				num_nodes--;
				delete_internal(parent, right_node, parent->keys[parent_right_index - 1].key_num);
			}
		}

		else
		{
			// Requirements are all met so no action is needed.
			cout << "Internal Node has been deleted successfully! " << endl;
			return;
		}
	}

	else if (current == root)
	{
		if (current->num_keys - 1 == 0)
		{
			if (current->child_nodes[1] == child)
			{
				delete[] child->keys;
				delete[] child->child_nodes;
				delete child;
				root = current->child_nodes[0];
				delete[] current->keys;
				delete[] current->child_nodes;
				delete current;
				num_nodes--;
				cout << "Root Node has been changed to the ." << endl;
				return;
			}

			else if (current->child_nodes[0] == child)
			{
				delete[] child->keys;
				delete[] child->child_nodes;
				delete child;
				root = current->child_nodes[1];
				delete[] current->keys;
				delete[] current->child_nodes;
				delete current;
				num_nodes--;
				cout << "Root Node has been changed." << endl;
				return;
			}
		}
	}
}
