#ifndef FILTER_H
#define FILTER_H

#include <cstdint>

class Filter {
public:
    virtual void insert(int64_t entry) = 0;
    virtual bool query(int64_t entry) = 0;
};

#endif