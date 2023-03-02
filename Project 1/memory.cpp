#include "memory.h"
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <cmath>
#include <chrono>

using namespace std;
using namespace std::chrono;

// helper function
int roundHundred(int num){
    return (int(ceil(num) / 100) + 1) * 100;
};

char ms[] = {' ', char(230), 's'};

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

int Memory::getUsedSize(){
    int metadataSize = sizeof(*this);
    int pBlksSize =  sizeof(*this->pBlks) * this->numPBlk;
    int rBlksSize =  sizeof(*this->rBlks) * this->numRBlk;
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
    pBlock * newPBlk = &this->pBlks[this->numPBlk];
    pBlock * lastPBlk = this->getLastPBlock();
    lastPBlk->next = newPBlk;
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
    cout << "Number of records in memory: \t\t\t" << this->numRecords << endl;
    cout << "Size of a record: \t\t\t\t" << sizeof(r)  << " Bytes" << endl;
    cout << "Maximum number of records in a block: \t\t" << this->maxRecords << "\n" << endl;

    cout << "Number of record blocks used: \t\t\t" << this->numRBlk << endl;
    cout << "Number of pointer blocks used: \t\t\t" << this->numPBlk << endl;
    cout << "Total number of blocks used: \t\t\t" << this->numRBlk + this->numPBlk << "\n" << endl;
    
    cout << "Size of allocated memory (Bytes): \t\t" << this->getSize() << " Bytes" << endl;
    cout << "Size of used memory (Bytes): \t\t\t" << this->getUsedSize() << " Bytes" << endl;
    cout << "Size of allocated memory (MB): \t\t\t" << this->getSize()/125000 << " MB" << endl;
    cout << "Size of used memory (MB): \t\t\t" << this->getUsedSize()/125000 << " MB" << "\n" << endl;
};

float Memory::linearScanEqual(int votes) {
    pBlock * pBlk = this->getFirstPBlock();
    rBlock * rBlk;
    int numRecords = 0;
    float sumRating = 0;

    auto start = high_resolution_clock::now();
    
    for (int k=0; k<this->numPBlk; k++) {
        for (int i=0; i<pBlk->numPtrs; i++){
            rBlk = pBlk->pointers[i];
            for (int j=0; j<rBlk->numRecords; j++) {
                if (rBlk->records[j].votes == votes){
                    numRecords++;
                    sumRating += rBlk->records[j].rating;
                }
            }
        }
        pBlk = pBlk->next;
    }

    auto stop = high_resolution_clock::now();
    cout << "Linear search completed in " << duration_cast<microseconds>(stop-start).count() << ms << endl;

    return sumRating/(numRecords == 0 ? 1 : numRecords);
}

float Memory::linearScanRange(int lowerBound, int upperBound){
    pBlock * pBlk = this->getFirstPBlock();
    rBlock * rBlk;
    int numRecords = 0;
    float sumRating = 0;

    auto start = high_resolution_clock::now();
    
    for (int k=0; k<this->numPBlk; k++) {
        for (int i=0; i<pBlk->numPtrs; i++){
            rBlk = pBlk->pointers[i];
            for (int j=0; j<rBlk->numRecords; j++) {
                if (rBlk->records[j].votes >= lowerBound && rBlk->records[j].votes <= upperBound){
                    numRecords++;
                    sumRating += rBlk->records[j].rating;
                }
            }
        }
        pBlk = pBlk->next;
    }

    auto stop = high_resolution_clock::now();
    cout << "Linear search completed in " << duration_cast<microseconds>(stop-start).count() << ms << endl;

    return sumRating/(numRecords == 0 ? 1 : numRecords);
};
