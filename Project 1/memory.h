#ifndef MEMORY_H
#define MEMORY_H
#include <string>
#include <vector>

using namespace std;
// struct of record
struct Record {                                 // 18 + 2 padding = 20 bytes
        char id[10];                            // 10 bytes
        float rating;                           // 4 bytes
        int votes;                              // 4 bytes
        Record(){};
        Record(std::string str);
};

// struct of record block: contains records
struct rBlock {                                 // 184 bytes
        int numRecords = 0;                     // 4 bytes
        Record records[9];                      // 180 bytes
};

// struct of pointer block: contains pointers to record blocks
struct pBlock {                                 // 196 + 4 padding = 200 bytes
        pBlock * next = nullptr;                // 8 bytes
        int numPtrs = 0;                        // 4 bytes
        rBlock * pointers[23];                  // 184 bytes
        pBlock(){};
        pBlock(pBlock * ptr, rBlock * rPtr);
};

class Memory {
        private:
        int maxRecords = 9;             // in a record block
        int maxPointers = 23;           // in a pointer block
        int maxRBlks;
        int maxPBlks;
        int numRecords = 0;
        int numRBlk = 0;        
        rBlock *rBlks;
        pBlock *pBlks;

        // get last record block
        rBlock * getLastRBlock();

        // add record block to memory
        rBlock * addRBlock();

        // add pointer block to memory
        pBlock * addPBlock();
        
        public:
        int numPBlk = 0;

        // take in number of records to init memory size
        Memory(int numRecords);

        // get first pointer block;
        pBlock * getFirstPBlock();

        // get last pointer block
        pBlock * getLastPBlock();

        // get size of memory allocated
        int getSize();

        // get size of memory used
        int getUsedSize();

        // add record to memory
        void addRecord(std::string str);

        // print memory data
        void printData();

        // return number of data blocks accessed in linear scan
        float linearScanAvg(int votes);
};

#endif