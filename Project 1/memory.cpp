#include "memory.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

Memory::Memory(){
    pBlock p = pBlock();
    this->firstBlk = &p;
    this->lastBlk = &p;
    this->numPBlk++;

    rBlock r = rBlock();
    this->firstBlk->pointers.push_back(&r);
    this->numRBlk++;
};

Record::Record(std::string str) {
    string temp;
    this->id = str.substr(0, str.find('\t')).c_str();
    stringstream s(str);
    getline(s, temp, '\t');
    s >> this->rating >> this->votes;
};

void Memory::addRecord(string str) {
    Record r = Record(str);

}

