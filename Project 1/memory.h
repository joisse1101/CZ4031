#ifndef MEMORY_H
#define MEMORY_H
#include <string>
#include <vector>

using namespace std;
// struct of record
struct Record {
        std::string id;
        float rating;
        int votes;
        Record(){};
        Record(std::string str);
};

// struct of record block: contains records
struct rBlock {
        unsigned short numRecords = 0; 
        Record records[4];
};

// struct of pointer block: contains pointers to record blocks
struct pBlock {
        pBlock * next = nullptr;
        unsigned short numPtrs = 0; 
        rBlock * pointers[23];
        pBlock(){};
        pBlock(pBlock * ptr, rBlock * rPtr);
};

class Memory {
        private:
        rBlock * rBlks;
        std::vector<pBlock> pBlks;
        
        public:
        Memory();

        // get last record block
        rBlock * getLastRBlock();

        // add record block to memory
        rBlock * Memory::addRBlock();

        // get last pointer block
        pBlock * getLastPBlock();

        // add record to memory
        void addRecord(std::string str);
};
// struct recordBlock {
//     std::vector<Record> records;
//     std::string * next = nullptr;
// };

#endif