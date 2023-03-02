#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <list>

#include "memory.h"
#include "b_plus_tree.h"


using namespace std;

void experiment2(); 

int main() {
    cout << "\n -------- CZ4031 Db Management System Project 1 by Group 47 --------\n"
         << "\n";

    // Load data.tsv file
    string file = "data.tsv";
    ifstream data_file("data/" + file);
    cout << "\nReading " << file << " ...\n";

    // Init memory
    int numRecords = std::count(istreambuf_iterator<char>(data_file), istreambuf_iterator<char>(), '\n');
    data_file.seekg( 0, std::ios::beg );
    Memory db = Memory(numRecords);

    // Init variables to read data.tsv
    string copied;
    string headers;
    getline(data_file, headers);
    int i = 0;

    while (getline(data_file,copied)) {
        db.addRecord(copied);        
        i++;
    };
    
    data_file.close();
    cout << "DB Size: " << db.getSize() << endl;

    cout << "\nTotal records read: " << i << endl;
    db.printData();
    
    // experiment2();

}

void experiment2()
{
    typedef unsigned char uchar;
    // ---------------- B + Tree Testing --------------------------

    cout << "\n------------------- B+ Tree Testing ------------------------" << endl;
    
    BPlusTree tree;
    Key new_key;

    list<int> key_list { 5, 10, 6, 8, 1, 15, 22,100, 11, 20, 300, 51, 80, 2, 3, 4, 12, 1};
    for (int key: key_list)
    {
        int a = 10;
        void * dummy_addr = &a; 
        new_key.key_num = key;
        // new_key.record_addr = dummy_addr; // Void Type 
        new_key.addrs.push_back(dummy_addr);
        tree.insert_node(new_key);
    }

    cout << "\n------------------- B+ Tree Properties ------------------------" << endl;
    cout << "Parameter (N) of B+ Tree: " << tree.get_max_keys() << endl ;
    cout << "Number of Nodes in B+ Tree: " << tree.get_num_nodes() << endl ;
    cout << "Depth of B+ Tree: " << tree.get_tree_depth(tree.get_root()) << endl ;
    cout << "Keys of Root Node: ";
    tree.print_root_keys(tree.get_root());

    tree.display_tree(tree.get_root());
}
