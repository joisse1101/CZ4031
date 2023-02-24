#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

struct Record {
    string id;
    float rating;
    int votes;
};

int main() {
    cout << "\n -------- CZ4031 Db Management System Project 1 by Group 47 --------\n"
         << "\n";

    // Load data from data.tsv

    vector<void *> db; // database here

    string file = "data.tsv"; //filename
    ifstream data_file("data/" + file); //opening file
    cout << "Reading " << file << " ...\n";

    string copied, temp;
    string headers;

    getline(data_file, copied);
    headers = copied;

    int i = 0;
    //Record r;
    //cout <<"Size of Record: " << sizeof(r.id) << "\n";

    while (getline(data_file,copied)) {
        Record r;

        r.id = copied.substr(0, copied.find('\t')).c_str();

        stringstream s(copied);
        getline(s, temp, '\t');
        s >> r.rating >> r.votes;

        if (i%10000 == 0) {
            cout << "Reading record: " << r.id << + " " << r.rating << " " << r.votes << " \n";
            cout <<"Size of Record: " << sizeof(r) << "\n";
        }


        i++;
    };
    data_file.close();

    cout << "Total records read: " << ++i << "\n";
}