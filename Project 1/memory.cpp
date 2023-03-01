#include "memory.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

Memory::Memory(){
    pBlock pBlk = pBlock();
    this->firstBlk = &pBlk;
    this->lastBlk = &pBlk;
    this->numPBlk++;

    rBlock rBlk = rBlock();
    this->firstBlk->pointers.push_back(&rBlk);
    this->numRBlk++;
};

Record::Record(std::string str) {
    // cout << "Creating Record";
    string temp;
    this->id = str.substr(0, str.find('\t')).c_str();
    stringstream s(str);
    getline(s, temp, '\t');
    s >> this->rating >> this->votes;
};

int rBlock::getSize() {
    return sizeof(*this);
}

void Memory::addRecord(string str) {
    cout << "Creating Record";
    Record r = Record(str);
    cout << r.id << r.rating << r.votes;
    

    // 1. Check if block is full
    // cout << this->lastBlk->pointers.back()->getSize();
    // if (this->lastBlk->pointers.back()->getSize() > blkSize)
    // {
    //     cout << "Last record block is full";
    //     // TODO: Create new record block. Increment pointer.
    // };
    // Add record
    cout << "Adding Record";
    // this->lastBlk->pointers.back()->records.push_back(r);
    // this->numRecords++;
};

