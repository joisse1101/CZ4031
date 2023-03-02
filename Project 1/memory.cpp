#include "memory.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <cmath>

using namespace std;

// helper function
int roundHundred(int num){
    return (int(ceil(num) / 100) + 1) * 100;
};

Record::Record(std::string str) {
    string temp;
    strcpy(this->id, str.substr(0, str.find('\t')).c_str());
    stringstream s(str);
    getline(s, temp, '\t');
    s >> this->rating >> this->votes;
};

pBlock::pBlock(pBlock * ptr, rBlock * rPtr){
    this->next = ptr;
    this->pointers[0] = rPtr;
    this->numPtrs++;
};

Memory::Memory(int numRecords){
    this->maxRBlks = roundHundred(numRecords / this->maxRecords);
    this->maxPBlks = ceil(maxRBlks / this->maxPointers);
    
    // Empty rBlock & pBlock arrays
    this->rBlks = new rBlock[maxRBlks];
    this->pBlks = new pBlock[maxPBlks];

    // Init empty p-r blocks
    pBlock pBlk = pBlock(nullptr, &this->rBlks[0]);
    this->pBlks[this->numPBlk] = pBlk;
    this->numPBlk++;
    this->numRBlk++;
};

int Memory::getSize(){
    int metadataSize = sizeof(*this);
    int pBlksSize =  sizeof(*this->pBlks) * this->maxPBlks;
    int rBlksSize =  sizeof(*this->rBlks) * this->maxRBlks;
    return metadataSize + pBlksSize + rBlksSize;
}

rBlock * Memory::getLastRBlock(){    
    pBlock * lastPBlk = this->getLastPBlock();
    return lastPBlk->pointers[lastPBlk->numPtrs-1];
};

rBlock * Memory::addRBlock(){
    pBlock * lastPBlk = this->getLastPBlock();
    if (lastPBlk->numPtrs >= this->maxPointers) {
        lastPBlk = this->addPBlock();
    }
    lastPBlk->pointers[lastPBlk->numPtrs] = &this->rBlks[this->numRBlk];
    lastPBlk->numPtrs++;
    this->numRBlk++;

    if (this->numRBlk>= this->maxRBlks) {
        cout << "Max r blks reached" << endl;
    }
    return this->getLastRBlock();
};

pBlock * Memory::getFirstPBlock(){
    return &this->pBlks[0];
};

pBlock * Memory::getLastPBlock(){    
    return &this->pBlks[this->numPBlk-1];
};

pBlock * Memory::addPBlock(){
    pBlock newPBlk = pBlock();
    pBlock * lastPBlk = this->getLastPBlock();
    lastPBlk->next = &newPBlk;
    this->pBlks[this->numPBlk] = newPBlk;
    this->numPBlk++;
    if (this->numPBlk >= this->maxPBlks) {
        cout << "Max p blks reached" << endl;
    }
    return this->getLastPBlock();
}

void Memory::addRecord(string str) {
    Record r = Record(str);
    rBlock * lastRBlk = this->getLastRBlock();
    if ((lastRBlk->numRecords) >= this->maxRecords) {
        lastRBlk = this->addRBlock();
    }
    lastRBlk->records[lastRBlk->numRecords] = r;
    lastRBlk->numRecords++;
    this->numRecords++;
};

void Memory::printData() {
    Record r = Record();
    cout << "\nPrinting memory data..." << endl;
    cout << "Number of records in memory: \t\t\t" << this->numRecords << endl;
    cout << "Size of a record: \t\t\t\t" << sizeof(r)  << " Bytes" << endl;
    cout << "Maximum number of records in a block: \t\t" << this->maxRecords << endl;
    cout << "Number of blocks used to store data in memory: \t" << this->numRBlk + this->numPBlk << endl;
    cout << "Size of memory (Bytes): \t\t\t" << this->getSize() << " Bytes" << endl;
    cout << "Size of memory (MB): \t\t\t\t" << this->getSize()/125000 << " MB" << endl;
}
