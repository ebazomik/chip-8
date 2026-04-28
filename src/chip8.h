#ifndef CHIP8_H_
#define CHIP8_H_

//---------------------------------------
// Sturct
// --------------------------------------

struct Chip8 {
    // Flags for signal if CHIP-8 is running.
    unsigned char is_running;

    // CHIP-8 has 35 opcodes.
    unsigned char opcode;

    // CHIP-8 has 4K total memory.
    // System memory map:
    //
    //
    unsigned char memory[4096];

    unsigned char V[16];

    unsigned short I;
    unsigned short pc;

    unsigned char gfx[64 * 32];

    unsigned char should_draw;

    unsigned char delay_timer;
    unsigned char sound_timer;

    unsigned short stack[16];
    unsigned short sp;

    unsigned char key[16];

};
typedef struct Chip8 Chip8;

//--------------------------------------
// Functions
//--------------------------------------

Chip8* initChip8();

void destroyChip8(Chip8* chip8);

void loadROMChip8(Chip8* chip8, char* rom);


#endif // CHIP8_H_
