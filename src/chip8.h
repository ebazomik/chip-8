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
    //      0x000-0x1FF - Chip8 Interpreter.
    //      0x200-0xFFF - Start of most programs.
    //      0x600-0xFFF - Some programs start here (ETI 660).
    //
    unsigned char memory[4096];

    // Chip-8 has 16 general puprose 8-bit registers.
    unsigned char V[16];

    // 16-bit register to store memory address.
    unsigned short I;
    // Program counter register.
    unsigned short pc;

    // The original implementation of Chip-8 used 64x32 pixel monochrome display
    unsigned char gfx[64 * 32];

    // Used for determinate if screen need to be update.
    unsigned char should_draw;

    // Chip-8 provides 2 timers, a delay timer and a sound timer.
    // The delay timer is active whenever the delay timer register (DT) is non-zero.
    // This timer does nothing more than subtract 1 from the value of DT at a rate of 60Hz.
    // When DT reaches 0, it deactivates.
    //
    // The sound timer is active whenever the sound timer register (ST) is non-zero.
    // This timer also decrements at a rate of 60Hz, however, as long as ST's value is greater than zero,
    // the Chip-8 buzzer will sound. When ST reaches zero, the sound timer deactivates.
    // The sound produced by the Chip-8 interpreter has only one tone.

    // The frequency of this tone is decided by the author of the interpreter.
    unsigned char delay_timer;
    unsigned char sound_timer;

    // Chip-8 have 16 level of stack.
    // Stack pointer (sp) trace which level of stack is used.
    unsigned short stack[16];
    unsigned short sp;

    // This array is used to store the state of keyboard.
    // Keyboard mapping layout:
    //
    //              1 | 2 | 3 | C
    //              4 | 5 | 6 | D
    //              7 | 8 | 9 | E
    //              A | 0 | B | F
    //
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
