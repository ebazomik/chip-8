#include "chip8.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


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
        LOG_ERROR("Error on initialize Chip8");
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
        LOG_ERROR("[ERROR] - invalid parameters of loadROMChip8");
        exit(1);
    }

    errno = 0;
    int f = open(rom, O_RDONLY);

    if(f == -1){
        LOG_ERROR_ARG("[ERROR] - when try to open rom: %s", strerror(errno));
        exit(1);
    }

    // Use lseek to find size of rom and check it size.
    off_t fsize = lseek(f, 0, SEEK_END);

    if(fsize <= 0){
        LOG_ERROR("[ERROR] - ROM file is empty");
        exit(1);
    }
    if(fsize > CHIP8_MAX_PROGRAM_SIZE){
    LOG_ERROR_ARG("[ERROR] - size of ROM is too big: %ld\n", fsize);
        exit(1);
    }

    // Set seek pointer to start of file.
    lseek(f, 0, SEEK_SET);

    // Read and copy in chip-8 memory directly the program bytes.
    ssize_t bytes_readed = read(f, &chip8->memory[CHIP8_START_PROGRAM], fsize);
    close(f);

    if(bytes_readed != fsize){
        LOG_ERROR_ARG("[ERROR] - Failed to read ROM: read %ld of %ld bytes\n", bytes_readed, fsize);
        exit(1);
    }

    return;
}

void chip8Step(Chip8* chip8){

    if(chip8->pc >= 4095){
        LOG_ERROR_ARG("Can't increment pc: 0x%03x", chip8->pc);
        exit(1);
    }

    unsigned short opcode = GET_OPCODE(chip8);
    chip8->opcode = opcode;
    chip8->pc += 2;
    executeOpcode(chip8, opcode);

    return;
}


void executeOpcode(Chip8 *chip8, short opcode){

    // Every opcode has its own specific instruction.
    // The first hex digit (0x[n]) determines the instruction type.
    // Based on the instruction, the remaining digits represent:
    //
    // - X, Y: register indices (4 bits each, 0-15)
    // - N:    4-bit value (0-15)
    // - NN:   8-bit value (0-255)
    // - NNN:  12-bit memory address (0-4095), used for jumps/calls
    //
    // Not every instruction uses all of these.
    // The instruction type decides whether to interpret the digits as N, NN, or NNN.
    //
    // For deep in CHIP_8 instructions:
    // https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Set

    unsigned char  X   = (opcode & 0x0F00) >> 8;
    unsigned char  Y   = (opcode & 0x00F0) >> 4;
    unsigned short N   = opcode & 0x000F;
    unsigned short NN  = opcode & 0x00FF;
    unsigned short NNN = opcode & 0x0FFF;

    (void) X;
    (void) Y;
    (void) N;
    (void) NN;
    (void) NNN;

    switch((opcode & 0xF00) >> 12){
        case 0x0: {

        } break;

        case 0x1: {

        } break;

        case 0x2: {

        } break;

        case 0x3: {

        } break;

        case 0x4: {

        } break;
        case 0x5: {

        } break;

        case 0x6: {

        } break;

        case 0x7: {

        } break;

        case 0x8: {

        } break;

        case 0x9: {

        } break;
        case 0xA: {

        } break;

        case 0xB: {

        } break;

        case 0xC: {

        } break;

        case 0xD: {

        } break;

        case 0xE: {

        } break;
        case 0xF: {

        } break;


        default:
            LOG_ERROR("Unreachable default case in chipExecuteCode");
            exit(1);
            return;
    }

    return;
}
