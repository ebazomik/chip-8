#include <stdio.h>
#include "chip8.h"

int main(int argc, char ** argv){

    if(argc < 2){
        printf("Usage: %s <ROM>\n", argv[0]);
        return 1;
    }

    char * rom = argv[1];
    printf("[INFO] - ROM selected: %s ...\n", rom);

    //--------------------------------------
    // Initialization
    //--------------------------------------
    Chip8* chip8 = initChip8();
    fprintf(stdout, "[INFO] - allocated Chip-8 at 0x%p\n", (void*)chip8);

    loadROMChip8(chip8, rom);
    fprintf(stdout, "[INFO] - loading ROM: %s\n", rom);

    //--------------------------------------
    // Emulation
    //--------------------------------------


    //--------------------------------------
    // Cleanup
    //--------------------------------------
    destroyChip8(chip8);

    return 0;
}
