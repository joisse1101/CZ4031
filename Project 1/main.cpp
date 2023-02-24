#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "memory.h"

using namespace std;

int main() {
    cout << "\n -------- CZ4031 Db Management System Project 1 by Group 47 --------\n"
         << "\n";

    // Load data from data.tsv

    vector<Record> db; // database here

    string file = "data.tsv"; //filename
    ifstream data_file("data/" + file); //opening file
    cout << "Reading " << file << " ...\n";
    cout << "TEST5";

    string copied, temp;
    string headers;

    getline(data_file, copied);
    headers = copied;

    int i = 0;
    Record r;
    cout <<"Size of Record: " << sizeof(r.id) << "\n";
    cout <<"\nPointer: " << &r;
    cout << "\nSize of Pointer: " << sizeof(&r);


    while (getline(data_file,copied)) {
        Record r;

        r.id = copied.substr(0, copied.find('\t')).c_str();

        stringstream s(copied);
        getline(s, temp, '\t');
        s >> r.rating >> r.votes;

        db.push_back(r);

        if (i%10000 == 0) {
            cout << "Reading record: " << r.id << + " " << r.rating << " " << r.votes << " \n";
            cout <<"Size of Record: " << sizeof(r) << "\n";
        }
        
        i++;
    };
    
    data_file.close();

    cout << "\nSize : " << db.size();
    cout << "\nCapacity : " << db.capacity();
    cout << "\nMax_Size : " << db.max_size();

    cout << "\nTotal records read: " << i << "\n";
}