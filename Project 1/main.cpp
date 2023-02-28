#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <list>

#include "memory.h"
#include "memory.cpp"
#include "b_plus_tree.cpp"
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
    cout << "Reading " << file << " ...\n";
    cout << "TEST5";

    string copied;
    string headers;

    getline(data_file, copied);
    headers = copied;

    int i = 0;

    while (getline(data_file,copied)) {
        Record r = db.addRecord(copied);

        if (i%10000 == 0) {
            // cout << "Reading record: " << r.id << + " " << r.rating << " " << r.votes << " \n";
            // cout <<"Size of Record: " << sizeof(r) << "\n";
        }
        
        i++;
    };
    
    data_file.close();

    cout << "\nTotal records read: " << i << "\n";
    
    experiment2();

}

void experiment2()
{
    // ---------------- B + Tree Testing --------------------------

    cout << "\n------------------- B+ Tree Testing ------------------------" << endl;
    
    BPlusTree tree;
    Key new_key;

    list<int> key_list { 5, 10, 6, 8, 1, 15, 22,100, 11, 20, 300 };
    for (int key: key_list)
    {
        new_key.key_num = key;
        tree.insert_node(new_key);
    }

    cout << "\n------------------- B+ Tree Properties ------------------------" << endl;
    cout << "Parameter (N) of B+ Tree :" << tree.get_max_keys() << endl ;
    tree.display_tree(tree.get_root());
}
