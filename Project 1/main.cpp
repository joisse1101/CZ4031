#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <list>

#include "memory.h"
#include "b_plus_tree.h"


using namespace std;

void experiment2(); 

int main() {
    cout << "\n -------- CZ4031 Db Management System Project 1 by Group 47 --------\n"
         << "\n";

    // Load data from data.tsv
    Memory db = Memory();

    string file = "data.tsv";
    ifstream data_file("data/" + file);
    cout << "\nReading " << file << " ...\n";

    string copied;
    string headers;

    getline(data_file, copied);
    headers = copied;

    int i = 0;

    while (getline(data_file,copied)) {
        db.addRecord(copied);        
        i++;

        if (i==5) {
            break;
        }
    };
    
    data_file.close();

    cout << "\nTotal records read: " << i << "\n";
    
    // experiment2();

}

void experiment2()
{
    // ---------------- B + Tree Testing --------------------------

    cout << "\n------------------- B+ Tree Testing ------------------------" << endl;
    
    BPlusTree tree;
    Key new_key;

    list<int> key_list { 5, 10, 6, 8, 1, 15, 22,100, 11, 20, 300, 51, 80, 2, 3, 4, 12};
    for (int key: key_list)
    {
        new_key.key_num = key;
        tree.insert_node(new_key);
    }

    cout << "\n------------------- B+ Tree Properties ------------------------" << endl;
    cout << "Parameter (N) of B+ Tree :" << tree.get_max_keys() << endl ;
    cout << "Number of Nodes in B+ Tree  :" << tree.get_num_nodes() << endl ;
    tree.display_tree(tree.get_root());
}
