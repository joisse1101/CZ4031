#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "memory.h"
#include "memory.cpp"

using namespace std;

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
}