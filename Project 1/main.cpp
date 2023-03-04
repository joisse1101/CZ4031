#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <list>
#include <chrono>

// CPP program to illustrate
// Implementation of pop() function

#include <iostream>
#include <stack>

#include "memory.h"
#include "b_plus_tree.h"

using namespace std;
using namespace std::chrono;

BPlusTree build_b_plus_tree(Memory db, int numRead);
bool isInside(vector<void *> all_addrs, void *ptr_addrs);
int number_of_occ(vector<void *> all_addrs, void *ptr_addrs);
void searchRangeWithDB(int lowerBound, int upperBound, Memory db, vector<void *> all_addrs);
void searchSingleWithDB(Memory db, vector<void *> all_addrs, float key_num);

void experiment2(BPlusTree tree);
void experiment3(Memory db, BPlusTree tree);
void experiment4(Memory db, BPlusTree tree);
void experiment5(Memory db, BPlusTree tree);

int main()
{
    cout << "\n -------- CZ4031 Db Management System Project 1 by Group 47 --------\n"
         << endl;

    // Load data.tsv file
    string file = "data.tsv";
    ifstream data_file("data/" + file);
    cout << "\nReading " << file << " ...\n";

    float progress = 0;

    // Init memory
    int numRecords = std::count(istreambuf_iterator<char>(data_file), istreambuf_iterator<char>(), '\n') - 1;
    data_file.seekg(0, std::ios::beg);
    Memory db = Memory(numRecords);

    // Init variables to read data.tsv
    string copied;
    string headers;
    getline(data_file, headers);
    int numRead = 0;
    int numMod = numRecords / 10;
    int barWidth = 70;
    int pos;

    while (getline(data_file, copied))
    {
        db.addRecord(copied);
        numRead++;
        if (numRead % numMod == 0 || numRead == numRecords)
        {
            progress = float(numRead) / float(numRecords);
            std::cout << "[";
            pos = barWidth * progress;
            for (int i = 0; i < barWidth; ++i)
            {
                if (i < pos)
                    std::cout << "=";
                else if (i == pos)
                    std::cout << ">";
                else
                    std::cout << " ";
            }
            std::cout << "] " << int(progress * 100.0) << " %\r";
            std::cout.flush();
        }
    };

    data_file.close();

    cout << "\nTotal records read: " << numRead << "\n"
         << endl;

    BPlusTree tree;
    tree = build_b_plus_tree(db, numRead);

    // db.printData(); // TODO uncomment final
    experiment2(tree);
    experiment3(db, tree);
    experiment4(db, tree);
    experiment5(db, tree);
}

BPlusTree build_b_plus_tree(Memory db, int numRead)
{
    cout << "\n------------------- B+ Tree Initialisation/ Building (Please hold...) ------------------------" << endl;

    BPlusTree tree;
    Key key_obj;
    pBlock *pBlk = db.getFirstPBlock();
    rBlock *rBlk;
    int num_records_added = 0;

    float progress = 0.0;
    int barWidth = 70;
    int pos;

    cout << "Iterating through the pointers to insert keys ----------------- " << endl;
    auto start = high_resolution_clock::now();

    for (int k = 0; k < db.numPBlk; k++)
    {
        // cout << "Address inserted: " << pBlk << endl;
        for (int i = 0; i < pBlk->numPtrs; i++)
        {
            rBlk = pBlk->pointers[i];
            for (int j = 0; j < rBlk->numRecords; j++)
            {
                key_obj.key_num = rBlk->records[j].votes;
                key_obj.addrs.push_back(pBlk);
                tree.insert_node(key_obj);
                key_obj.addrs.pop_back();
                num_records_added++;
            }
        }
        pBlk = pBlk->next;
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken by to build the B+ tree: \t\t\t " << duration.count() << " microseconds" << endl;
    cout << "Number of records added (Expected 1070318) \t\t\t: " << num_records_added << endl;

    return tree;
}

void experiment2(BPlusTree tree)
{

    cout << "\n-------------------Exp 2: B+ Tree Properties (After Insertion) ------------------------" << endl;
    cout << "\nParameter (N) of B+ Tree:\t" << tree.get_max_keys() << endl;
    cout << "Number of Nodes in B+ Tree:\t" << tree.get_num_nodes() << endl;
    cout << "Depth of B+ Tree:\t\t" << tree.get_tree_depth(tree.get_root()) << endl;
    cout << "Keys of Root Node:\t\t";
    tree.print_root_keys(tree.get_root());
    cout << "\n";
    cout << "Details for root: " << endl;
    tree.display_tree(tree.get_root());
    cout << "\n";
}

void experiment3(Memory db, BPlusTree tree)
{
    cout << "\n ---------------------- Exp 3: numVotes = 500 ----------------------" << endl;
    float key_num = 500;
    cout << "\n1. B+ tree search" << endl;
    vector<void *> all_addrs = tree.search(key_num);
    searchSingleWithDB(db, all_addrs, key_num);

    cout << "\n2. Linear scan of dB" << endl;
    db.linearScanEqual(500);
}

void experiment4(Memory db, BPlusTree tree)
{
    int lowerBound = 30000;
    int upperBound = 40000;

    cout << "\n --------------- Exp 4: 30,000 <= numVotes <= 40,0000 --------------" << endl;

    cout << "\n1. B+ tree search" << endl;
    vector<void *> all_addrs = tree.searchRange(lowerBound, upperBound);
    cout << "\n";
    searchRangeWithDB(lowerBound, upperBound, db, all_addrs);

    cout << "\n2. Linear scan of dB" << endl;
    db.linearScanRange(30000, 40000);
}

void experiment5(Memory db, BPlusTree tree)
{
    auto start = high_resolution_clock::now();
    cout << "\n --------------- Exp 5: Deletion of numvotes == 1000 --------------" << endl;
    cout << "\n1. B+ tree Deletion" << endl;
    tree.delete_key(1000);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    cout << "Time taken by to delete record which numVotes == 1000 the B+ tree: " << duration.count() << " microseconds" << endl;

    cout << "Attempting to remove key 1000 again..." << endl;
    tree.delete_key(1000);

    cout << "\n------------------- B+ Tree Properties (After Deletion of numvotes == 1000) ------------------------\n"
         << endl;
    cout << "Parameter (N) of B+ Tree:\t" << tree.get_max_keys() << endl;
    cout << "Number of Nodes in B+ Tree:\t" << tree.get_num_nodes() << endl;
    cout << "Depth of B+ Tree:\t\t" << tree.get_tree_depth(tree.get_root()) << endl;
    cout << "Keys of Root Node:\t\t";
    tree.print_root_keys(tree.get_root());
    cout << "\n";
    cout << "Details for root:" << endl;
    tree.display_tree(tree.get_root());
    cout << "\n";
}

void searchRangeWithDB(int lowerBound, int upperBound, Memory db, vector<void *> all_addrs)
{

    pBlock *pBlk = db.getFirstPBlock();
    rBlock *rBlk;
    int numPBlksAccessed = 0;
    int numRBlksAccessed = 0;
    int numRecords = 0;
    float sumRating = 0;
    int numRecordAddress = all_addrs.size();

    auto start = high_resolution_clock::now();
    for (int k = 0; k < db.numPBlk; k++)
    {
        // If all_address array does not contain the address of current pBlk, move to next ptrBlk
        bool exist = isInside(all_addrs, pBlk);
        if (exist)
        {
            int num_occ = number_of_occ(all_addrs, pBlk);

            for (int i = 0; i < pBlk->numPtrs; i++)
            {
                rBlk = pBlk->pointers[i];
                numPBlksAccessed++;

                for (int j = 0; j < rBlk->numRecords; j++)
                {
                    numRBlksAccessed++;
                    if (rBlk->records[j].votes >= lowerBound && rBlk->records[j].votes <= upperBound)
                    {
                        // Record found!!
                        numRecords++;
                        sumRating += rBlk->records[j].rating;
                        num_occ--;
                    }
                    if (num_occ == 0)
                    {
                        break;
                    }
                }

                if (num_occ == 0)
                {
                    break;
                }
            }

            if (numRecords == numRecordAddress)
            {
                break;
            }

            pBlk = pBlk->next;
        }

        else
        {
            if (numRecords == numRecordAddress)
            {
                break;
            }

            // Go to next PtrBlk
            pBlk = pBlk->next;
        }
    }

    float avg = sumRating / (numRecords == 0 ? 1 : numRecords);
    auto stop = high_resolution_clock::now();
    cout << "\tB+ Tree search completed in \t\t" << duration_cast<microseconds>(stop - start).count() << " microseconds" << endl;
    cout << "\tNumber of movies: \t\t\t" << numRecords << endl;
    cout << "\tAverage rating: \t\t\t" << avg << endl;
    cout << "\tNumber of pointer blocks accessed: \t" << numPBlksAccessed << endl;
    cout << "\tNumber of record blocks accessed: \t" << numRBlksAccessed << endl;
    cout << "\tTotal number of blocks accessed: \t" << numPBlksAccessed + numRBlksAccessed << endl;
}

void searchSingleWithDB(Memory db, vector<void *> all_addrs, float key_num)
{

    pBlock *pBlk = db.getFirstPBlock();
    rBlock *rBlk;
    int numPBlksAccessed = 0;
    int numRBlksAccessed = 0;
    int numRecords = 0;
    float sumRating = 0;
    int numRecordAddress = all_addrs.size();

    auto start = high_resolution_clock::now();

    for (int k = 0; k < db.numPBlk; k++)
    {
        // If all_address array does not contain the address of current pBlk, move to next ptrBlk
        bool exist = isInside(all_addrs, pBlk);
        if (exist)
        {
            int num_occ = number_of_occ(all_addrs, pBlk);

            for (int i = 0; i < pBlk->numPtrs; i++)
            {
                rBlk = pBlk->pointers[i];
                numPBlksAccessed++;

                for (int j = 0; j < rBlk->numRecords; j++)
                {
                    numRBlksAccessed++;
                    if (rBlk->records[j].votes == key_num)
                    {
                        // Record found!!
                        numRecords++;
                        sumRating += rBlk->records[j].rating;
                        num_occ--;
                    }
                    if (num_occ == 0)
                    {
                        break;
                    }
                }

                if (num_occ == 0)
                {
                    break;
                }
            }

            if (numRecords == numRecordAddress)
            {
                break;
            }

            pBlk = pBlk->next;
        }

        else
        {
            if (numRecords == numRecordAddress)
            {
                break;
            }

            pBlk = pBlk->next;
        }
    }

    float avg = sumRating / (numRecords == 0 ? 1 : numRecords);
    auto stop = high_resolution_clock::now();
    cout << "\tB+ Tree Search completed in \t\t" << duration_cast<microseconds>(stop - start).count() << " microseconds" << endl;
    cout << "\tNumber of movies: \t\t\t" << numRecords << endl;
    cout << "\tAverage rating: \t\t\t" << avg << endl;
    cout << "\tNumber of pointer blocks accessed: \t" << numPBlksAccessed << endl;
    cout << "\tNumber of record blocks accessed: \t" << numRBlksAccessed << endl;
    cout << "\tTotal number of blocks accessed: \t" << numPBlksAccessed + numRBlksAccessed << endl;
}

bool isInside(vector<void *> all_addrs, void *ptr_addrs)
{
    bool exists = std::find(std::begin(all_addrs), std::end(all_addrs), ptr_addrs) != std::end(all_addrs);
    return exists;
}

int number_of_occ(vector<void *> all_addrs, void *ptr_addrs)
{
    int occ = 0;
    for (int x = 0; x < all_addrs.size(); x++)
    {
        if (all_addrs[x] == ptr_addrs)
            occ++;
    }
    return occ;
}
