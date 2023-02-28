#ifndef MEMORY_H
#define MEMORY_H
#include <string>
#include <vector>

// struct of record
struct Record {
        std::string id;
        float rating;
        int votes;
};

int blkSize = 40;

// struct of a block that contains records
struct rBlock {
        std::vector<Record> records;
};

// struct of block that contains pointers to records
struct pBlock {
        std::vector<rBlock *> pointers;
        pBlock * next = nullptr;
};

class Memory {
        private:
        pBlock * firstBlk;
        pBlock * lastBlk;

        public:
        //Constructors
        Memory();

        // Function to add record to memory
        Record addRecord(std::string str);
};
// struct recordBlock {
//     std::vector<Record> records;
//     std::string * next = nullptr;
// };

#endif