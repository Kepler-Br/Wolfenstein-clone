#include <iostream>
#include "main_loop.h"

int main() {
    Main_loop main_loop({800, 600});
    main_loop.start();
    return 0;
}
