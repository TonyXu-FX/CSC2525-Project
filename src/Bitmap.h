#ifndef BITMAP_H
#define BITMAP_H

#include <stdio.h>
#include <cstdint>

class Bitmap {
public:
    Bitmap(size_t size);
    ~Bitmap();
    void set(size_t index);
    bool read(size_t index);

private:
    uint64_t* bitmap_;
    size_t size_;
};

#endif
