#include "chip8.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


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

    switch((opcode & 0xF000) >> 12){

        // =====================================

        case 0x0: {

            // 00E0 - Clear screen
            if(N == 0x0000){
                memset(chip8->gfx, 0, CHIP8_SCREEN_WIDTH * CHIP8_SCREEN_HEIGHT);
            }

            // 00EE - Return from subroutine
            else if (NN == 0x00EE){
                // The return address is saved on top of the stack by instruction 2NNN
                chip8->pc = chip8->stack[chip8->sp];
                chip8->sp--;
            }

            // 0NNN execute machine language from subroutine from address NNN
            else {
                LOG_INFO("Executing 0NNN");
            }

        } break;

        // =====================================

        case 0x1: {
            // 1NNN - Jump to address NNN
            chip8->pc = NNN;
        } break;

        // =====================================

        case 0x2: {
            // 2NNN - Execute subroutine starting at address NNN

            // Verify if stack space is full before save new return address
            if(chip8->sp == 16) LOG_ERROR("Call stack overflow");

            chip8->stack[chip8->sp] = chip8->pc;
            chip8->sp++;

            // After saved return address in the stack, jump to the
            // new address in program counter to execute new subroutine
            chip8->pc = NNN;
        } break;

        // =====================================

        case 0x3: {
            // 3XNN - Skip the following instruction if the value of register VX equals NN

            if(X >= 16){
                LOG_ERROR_ARG("3XNN, invalid value for X: %d > 15", X);
                exit(1);
            }

            if(chip8->V[X] == NN) chip8->pc += 2;
        } break;

        // =====================================

        case 0x4: {
            // 4XNN - Skip the following instruction if the value of register VX is not equal to NN

            if(X >= 16){
                LOG_ERROR_ARG("4XNN, invalid value for X: %d > 15", X);
                exit(1);
            }

            if(chip8->V[X] != NN) chip8->pc += 2;
        } break;

        // =====================================

        case 0x5: {
            // 5XY0 - Skip the following instruction if the value of register VX is equal to the value of register VY

            if(X >= 16){
                LOG_ERROR_ARG("5XNN, invalid value for X: %d > 15", X);
                exit(1);
            }

            if(Y >= 16){
                LOG_INFO_ARG("5XYN, invalid value for Y: %d > 15", Y);
                exit(1);
            }

            if(chip8->V[X] == chip8->V[Y]) chip8->pc += 2;
        } break;

        // =====================================

        case 0x6: {
            // 6XNN - Store number NN in register VX

            if(X >= 16){
                LOG_ERROR_ARG("6XNN, invalid value for X: %d > 15", X);
                exit(1);
            }

            chip8->V[X] = NN;
        } break;

        // =====================================

        case 0x7: {
            // 7XNN - Add the value NN to register VX

            if(X >= 16){
                LOG_ERROR_ARG("7XNN, invalid value for X: %d > 15", X);
                exit(1);
            }

            chip8->V[X] += NN;
        } break;

        // =====================================

        case 0x8: {

            // 8XY0 - Store the value of register VY in register VX
            if(N == 0x0000){
                if(Y >= 16){
                    LOG_ERROR_ARG("8XY0, invalid value for Y: %d > 15", Y);
                    exit(1);
                }

                chip8->V[X] = chip8->V[Y];
            }

            // 8XY1 - Set VX to VX OR VY
            else if(N == 0x0001){
                chip8->V[X] |= chip8->V[Y];
            }

            // 8XY2 - Set VX to VX AND VY
            else if(N == 0x0002){
                chip8->V[X] &= chip8->V[Y];
            }

            // 8XY3 - Set VX to VX XOR VY
            else if(N == 0x0003){
                chip8->V[X] ^= chip8->V[Y];
            }

            // 8XY4 - Add the value of register VY to register VX
            //      - Set VF to 01 if a carry occurs
            //      - Set VF to 00 if a carry does not occur
            else if(N == 0x0004){
                chip8->V[X] += chip8->V[Y];

                // Carry occurs when VX + VY > 255 (value overflow 8-bits register)
                unsigned char carry = (chip8->V[X] + chip8->V[Y]) > 255;
                if(carry){
                    chip8->V[15] = 1;
                } else {
                    chip8->V[15] = 0;
                }
            }

            // 8XY5 - Subtract the value of register VY from register VX
            //      - Set VF to 00 if a borrow occurs
            //      - Set VF to 01 if a borrow does not occur
            else if(N == 0x0005){
                chip8->V[X] = chip8->V[X] - chip8->V[Y];

                // Borrow occurs when VX < VY
                unsigned char noBorrow = chip8->V[X] >= chip8->V[Y];
                if(noBorrow){
                    chip8->V[15] = 1;
                } else {
                    chip8->V[15] = 0;
                }
            }

            // 8XY6 - Store the value of register VY shifted right one bit in register VX¹
            //      - Set register VF to the least significant bit prior to the shift
            //      - VY is unchanged
            else if(N == 0x0006){
                unsigned char lsb = chip8->V[Y] & 0x1;
                chip8->V[X] = chip8->V[Y] >> 1;

                chip8->V[15] = lsb;
            }

            // 8XY7 - Set register VX to the value of VY minus VX
            //      - Set VF to 00 if a borrow occurs
            //      - Set VF to 01 if a borrow does not occur
            else if(N == 0x0007){
                chip8->V[X] = chip8->V[Y] - chip8->V[X];

                unsigned char notBorrow = chip8->V[Y] >= chip8->V[X];
                if(notBorrow){
                    chip8->V[15] = 1;
                } else {
                    chip8->V[15] = 0;
                }
            }

            // 8XYE - Store the value of register VY shifted left one bit in register VX¹
            //      - Set register VF to the most significant bit prior to the shift
            //      - VY is unchanged
            else if (N == 0x000E) {
                unsigned char msb = (chip8->V[Y] & 0x80) >> 7;
                chip8->V[X] = chip8->V[Y] << 1;

                chip8->V[15] = msb;
            }

            else {
                LOG_ERROR_ARG("Unknown opcone 0x%x", opcode);
                exit(1);
            }

        } break;

        // =====================================

        case 0x9: {
            // TODO
        } break;

        // =====================================

        case 0xA: {
            // TODO
        } break;

        // =====================================

        case 0xB: {
            // TODO
        } break;

        // =====================================

        case 0xC: {
            // TODO
        } break;

        // =====================================

        case 0xD: {
            // TODO
        } break;

        // =====================================

        case 0xE: {
            // TODO
        } break;

        // =====================================

        case 0xF: {
            // TODO
        } break;

        // =====================================

        default:
            LOG_ERROR("Unreachable default case in chipExecuteCode");
            exit(1);
            return;
    }

    return;
}
