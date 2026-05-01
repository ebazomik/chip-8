#include <stdio.h>
#include "chip8.h"

int main(int argc, char ** argv){

    if(argc < 2){
        printf("Usage: %s <ROM>\n", argv[0]);
        return 1;
    }

    char * rom = argv[1];
    LOG_INFO_ARG("[INFO] - ROM selected: %s", rom);

    //--------------------------------------
    // Initialization
    //--------------------------------------
    Chip8* chip8 = initChip8();
    LOG_INFO_ARG("[INFO] - allocated Chip-8 at 0x%p", (void*)chip8);

    loadROMChip8(chip8, rom);
    LOG_INFO_ARG("[INFO] - loading ROM: %s", rom);

    //--------------------------------------
    // Emulation
    //--------------------------------------


    //--------------------------------------
    // Cleanup
    //--------------------------------------
    destroyChip8(chip8);

    return 0;
}
