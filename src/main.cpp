#include "MemoryManager.h"
#include <iostream>

int main() {
    MemoryManager mm;
    mm.init(1024);

    int a = mm.allocate_first_fit(200);
    int b = mm.allocate_first_fit(300);
    int c = mm.allocate_first_fit(100);

    mm.free_block(b);

    mm.dump();
    return 0;
}