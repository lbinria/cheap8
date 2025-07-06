#include <iostream>
#include "cheap8.h"
#include "renderer.h"


int main() {
    
    Cheap8 cheap;
    cheap.init();
    // cheap.load("../roms/IBM_Logo.ch8");
    // cheap.load("../roms/Chip8_Picture.ch8");
    // cheap.load("../roms/test.ch8");
    // cheap.load("../roms/call_ret_test.ch8");
    cheap.load("../roms/conditional_test.ch8");
    cheap.print_program();
    cheap.loop();
    cheap.clean();


    return 0;
}