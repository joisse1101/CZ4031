#include "memory.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

Memory::Memory(){
    pBlock p = pBlock();
    this->firstBlk = &p;
    this->lastBlk = &p;

    rBlock r = rBlock();
    this->firstBlk->pointers.push_back(&r);
};

Record Memory::addRecord(std::string str) {
    Record r;
    string temp;
    r.id = str.substr(0, str.find('\t')).c_str();
    stringstream s(str);
    getline(s, temp, '\t');
    s >> r.rating >> r.votes;
    return r;
}

