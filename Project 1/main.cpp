#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <list>

#include "memory.h"
#include "b_plus_tree.h"


using namespace std;
void build_b_plus_tree(Memory db, int numRead);
void experiment2(BPlusTree tree);
void experiment3(Memory db);
void experiment4(Memory db);

int main() {
    cout << "\n -------- CZ4031 Db Management System Project 1 by Group 47 --------\n" << endl;

    // Load data.tsv file
    string file = "data.tsv";
    ifstream data_file("data/" + file);
    cout << "\nReading " << file << " ...\n";

    float progress = 0;

    // Init memory
    int numRecords = std::count(istreambuf_iterator<char>(data_file), istreambuf_iterator<char>(), '\n')-1;
    data_file.seekg( 0, std::ios::beg );
    Memory db = Memory(numRecords);

    // Init variables to read data.tsv
    string copied;
    string headers;
    getline(data_file, headers);
    int numRead = 0;
    int numMod = numRecords/10;
    int barWidth = 70;
    int pos;

    while (getline(data_file,copied)) {
        db.addRecord(copied);
        numRead++;
        if (numRead % numMod == 0 || numRead == numRecords){
            progress = float(numRead)/float(numRecords);
            std::cout << "[";
            pos = barWidth * progress;
            for (int i = 0; i < barWidth; ++i) {
                if (i < pos) std::cout << "=";
                else if (i == pos) std::cout << ">";
                else std::cout << " ";
            }
            std::cout << "] " << int(progress * 100.0) << " %\r";
            std::cout.flush();
        }
    };
    
    data_file.close();

    cout << "\nTotal records read: " << numRead << "\n" << endl;
    build_b_plus_tree(db, numRead);

    
    // db.printData(); // TODO uncomment final

    
    // experiment2(db);

    // experiment3(db);

    // experiment4(db);
}

void build_b_plus_tree(Memory db, int numRead)
{
    cout << "\n------------------- B+ Tree Initialisation/ Building ------------------------" << endl;
    
    BPlusTree tree; 
    Key key_obj; 
    pBlock * pBlk = db.getFirstPBlock();
    rBlock * rBlk;  
    int num_records_added = 0;

    float progress = 0.0;
    int barWidth = 70;
    int pos;


    cout << " First pointer block... " << endl; 
    cout << " Number of pointer blocks: " << db.numPBlk << endl;
    cout << "Iterating through the pointers to insert keys ----------------- " << endl;
    for (int k=0; k< db.numPBlk; k++) 
    {
        for(int i=0; i<pBlk->numPtrs;i++)
        {
            rBlk = pBlk->pointers[i];
            for (int j=0; j<rBlk->numRecords; j++) 
            {
                // cout << "The first record is..." << rBlk->records[j].votes << endl;
                // cout << "The first record's address is " << &rBlk->records[j] << endl;
                
                key_obj.key_num = rBlk->records[j].votes;
                key_obj.addrs.push_back(&pBlk);
                tree.insert_node(key_obj); 
                num_records_added++;
                
                cout << "Records added as of now: " << num_records_added; 
            }

        }

        pBlk = pBlk->next;
        
    }

    cout << "Number of records added (Expected 1070318) " << num_records_added << endl;

    experiment2(tree);

}

void experiment2(BPlusTree tree)
{

    cout << "\n------------------- B+ Tree Properties ------------------------" << endl;
    cout << "Parameter (N) of B+ Tree: " << tree.get_max_keys() << endl ;
    cout << "Number of Nodes in B+ Tree: " << tree.get_num_nodes() << endl ;
    cout << "Depth of B+ Tree: " << tree.get_tree_depth(tree.get_root()) << endl ;
    cout << "Keys of Root Node: ";
    tree.print_root_keys(tree.get_root());
    tree.display_tree(tree.get_root());
}

void experiment3(Memory db) {
    cout << "\n ---------------------- Exp 3: numVotes = 500 ----------------------" << endl;
    
    cout << "\n1. B+ tree search" << endl;

    cout << "\n2. Linear scan of dB" << endl;
    db.linearScanEqual(500);
}

void experiment4(Memory db) {
    cout << "\n --------------- Exp 4: 30,000 <= numVotes <= 40,0000 --------------" << endl;
    
    cout << "\n1. B+ tree search" << endl;

    cout << "\n2. Linear scan of dB" << endl;
    db.linearScanRange(30000, 40000);
}