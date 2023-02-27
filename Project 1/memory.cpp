#include "memory.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

Memory::Memory(){};

Record Memory::addRecord(std::string str) {
    Record r;
    string temp;
    r.id = str.substr(0, str.find('\t')).c_str();
    stringstream s(str);
    getline(s, temp, '\t');
    s >> r.rating >> r.votes;
    return r;
}

