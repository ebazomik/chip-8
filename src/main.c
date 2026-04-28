#include <stdio.h>

int main(int argc, char ** argv){

    if(argc < 2){
        printf("Usage: %s <ROM>\n", argv[0]);
        return 1;
    }

    char * rom = argv[1];
    printf("Loading rom => %s ...\n", rom);

    //--------------------------------------
    // Initialization
    //--------------------------------------

    //--------------------------------------
    // Emulation
    //--------------------------------------


    //--------------------------------------
    // Cleanup
    //--------------------------------------


    return 0;
}
