#include "memory.h"
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

Record::Record(std::string str) {
    // cout << "Creating Record";
    string temp;
    this->id = str.substr(0, str.find('\t')).c_str();
    stringstream s(str);
    getline(s, temp, '\t');
    s >> this->rating >> this->votes;
};

pBlock::pBlock(pBlock * ptr, rBlock * rPtr){
    this->next = ptr;
    this->pointers[0] = rPtr;
    this->numPtrs++;
};

Memory::Memory(){
    this->rBlks = new rBlock[300000];
    pBlock pBlk = pBlock (nullptr, &this->rBlks[0]);
    this->pBlks.push_back(pBlk);
};

rBlock * Memory::getLastRBlock(){    
    return this->pBlks.back().pointers[this->pBlks.back().numPtrs-1];
};

rBlock * Memory::addRBlock(){

};

pBlock * Memory::getLastPBlock(){    
    return &this->pBlks.back();
};

void Memory::addRecord(string str) {
    cout << "Creating Record: ";
    Record r = Record(str);
    rBlock * lastRBlk = this->getLastRBlock();

    // Add record
    cout << "Adding Record" << endl;
    if (lastRBlk->numRecords < 4) {
        lastRBlk->records[lastRBlk->numRecords] = r;
        lastRBlk->numRecords++;
    } else {
        cout << "Make R Block" << endl;
    }
    cout << lastRBlk->numRecords << endl;
};

