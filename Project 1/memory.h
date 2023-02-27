#ifndef MEMORY_H
#define MEMORY_H
#include <string>
#include <vector>

struct Record {
        std::string id;
        float rating;
        int votes;
};

class Memory {
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