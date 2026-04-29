#include "chip8.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#define CHIP8_MEMORY_SIZE 0x1000
#define CHIP8_START_PROGRAM 0x200
#define CHIP8_MAX_PROGRAM_SIZE (CHIP8_MEMORY_SIZE - CHIP8_START_PROGRAM)

unsigned char CHIP8_FONTSET[80] = {
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


// Allocation and initialization of the Chip8 structure,
// during the initialization font is laoaded, registers and params are reset.
// Return pointer to Chip8 allocated struct.
Chip8* initChip8(){

    Chip8* chip8 = malloc(sizeof(Chip8));
    if(chip8 == NULL){
       fprintf(stderr, "Error on initialize Chip8\n");
       exit(1);
    }

    // Clean memory after malloc
    memset(chip8, 0,sizeof(Chip8));

    // Copy fontset in memory start from 80.
    // This point it is unclear.
    // Font starts from 0x00 => 0x050, or from 0x050 to 0x0A0?
    memcpy(&chip8->memory[80], CHIP8_FONTSET, 80);

    chip8->is_running = 1;  // Chip8 is running.
    chip8->pc = 0x200;      // Set program counter start position.
    chip8->I = 0;           // Set index to zero.
    chip8->sp = 0;          // Set stack pointer to zero.
    chip8->should_draw = 0; // Don't need draw at the start point.
    chip8->opcode = 0;      // Reset opcode value.

    srand(time(NULL));

    return chip8;
}

// Free memory of chip8
void destroyChip8(Chip8 *chip8){
    free(chip8);
    return;
}

// To load ROM in chip8 memory we need to check size of ROM to prevent
// overflow (we have only 0x800 bytes for program) and after copy readed bytes.
void loadROMChip8(Chip8* chip8, char* rom){

    if(chip8 == NULL || rom == NULL){
        fprintf(stderr, "[ERROR] - invalid parameters of loadROMChip8\n");
        exit(1);
    }

    int f = open(rom, O_RDONLY);

    if(f == -1){
        fprintf(stderr, "[ERROR] - when try to open rom: %s\n", strerror(errno));
        exit(1);
    }

    // Use lseek to find size of rom and check it size.
    off_t fsize = lseek(f, 0, SEEK_END);

    if(fsize <= 0){
        fprintf(stderr, "[ERROR] - ROM file is empty\n");
        exit(1);
    }
    if(fsize > CHIP8_MAX_PROGRAM_SIZE){
        fprintf(stderr, "[ERROR] - size of ROM is too big: %ld\n", fsize);
        exit(1);
    }

    // Set seek pointer to start of file.
    lseek(f, 0, SEEK_SET);

    // Read and copy in chip-8 memory directly the program bytes.
    ssize_t bytes_readed = read(f, &chip8->memory[CHIP8_START_PROGRAM], fsize);
    close(f);

    if(bytes_readed != fsize){
        fprintf(stderr, "[ERROR] - Failed to read ROM: read %ld of %ld bytes\n", bytes_readed, fsize);
        exit(1);
    }

    return;
}
