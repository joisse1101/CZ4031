#ifndef MEMORY_H
#define MEMORY_H
#include <string>
#include <vector>

// struct of record
struct Record {
        std::string id;
        float rating;
        int votes;
        Record(std::string str);
};

int blkSize = 40;

// struct of record block: contains records
struct rBlock {
        std::vector<Record> records;
};

// struct of pointer block: contains pointers to record blocks
struct pBlock {
        std::vector<rBlock *> pointers;
        pBlock * next = nullptr;
};

class Memory {
        private:
        pBlock * firstBlk;
        pBlock * lastBlk;

        public:
        int numPBlk = 0;
        int numRBlk = 0;
        int numRecords = 0;
        
        // Function to add record to memory
        void addRecord(std::string str);
};
// struct recordBlock {
//     std::vector<Record> records;
//     std::string * next = nullptr;
// };

#endif