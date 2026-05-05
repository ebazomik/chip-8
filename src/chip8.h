#ifndef CHIP8_H_
#define CHIP8_H_



#define CHIP8_MEMORY_SIZE 0x1000
#define CHIP8_START_PROGRAM 0x200
#define CHIP8_MAX_PROGRAM_SIZE (CHIP8_MEMORY_SIZE - CHIP8_START_PROGRAM)
#define CHIP8_SCREEN_WIDTH 64
#define CHIP8_SCREEN_HEIGHT 32

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
    unsigned char gfx[CHIP8_SCREEN_WIDTH * CHIP8_SCREEN_HEIGHT];

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

    // Bitmask for quirks
    unsigned int quirks;

};
typedef struct Chip8 Chip8;


enum CHIP8_KEY {
  CHIP8_KEY_1 = 0x01,
  CHIP8_KEY_2 = 0x02,
  CHIP8_KEY_3 = 0x03,
  CHIP8_KEY_4 = 0x0C,
  CHIP8_KEY_Q = 0x04,
  CHIP8_KEY_W = 0x05,
  CHIP8_KEY_E = 0x06,
  CHIP8_KEY_R = 0x0D,
  CHIP8_KEY_A = 0x07,
  CHIP8_KEY_S = 0x08,
  CHIP8_KEY_D = 0x09,
  CHIP8_KEY_F = 0x0E,
  CHIP8_KEY_Z = 0x0A,
  CHIP8_KEY_X = 0x00,
  CHIP8_KEY_C = 0x0B,
  CHIP8_KEY_V = 0x0F,
  CHIP8_KEY_UNDEFINED = 0x10
};

typedef enum CHIP8_KEY CHIP8_KEY;

enum CHIP8_QUIRK {
    // 8XY6, 8XYE
    QUIRK_SHIFT = 0x1,
    // FX55, FX65
    QUIRK_LOAD_STORE = 0x2,
    // BNN
    QUIRK_JUMP_REL = 0x4,
    // 8XY1, 8XY2, 8XY3
    QUIRK_VF_RESET = 0x8,
};

typedef enum CHIP8_QUIRK CHIP8_QUIRK;

//--------------------------------------
// Functions
//--------------------------------------

Chip8* initChip8();

// Logging info - normal and with additional args.
#define LOG_INFO(info)  \
    do{ \
        fprintf(stdout, "[INFO] - " info "\n"); \
    } while(0);

#define LOG_INFO_ARG(info, ...) \
    do{ \
        fprintf(stdout, "[INFO] - " info "\n", ##__VA_ARGS__);  \
    } while(0)

// Logging errors - normal and with additional args.
#define LOG_ERROR(error) \
    do{ \
        fprintf(stderr, "\033[0;31m[ERROR] - " error "\033[0m\n");  \
    } while(0)

#define LOG_ERROR_ARG(error, ...) \
    do{ \
        fprintf(stderr, "\033[0;31m[ERROR] - " error "\033[0m\n", ##__VA_ARGS__);  \
    } while(0)


// Every opcode in chip8 is 2 bytes long.
// For recover and read correct opcode, shift 8bit from first chunk of 8bit memory
// and merge with next 8it chunk.
#define GET_OPCODE(chip8) \
    ((unsigned short)(chip8->memory[chip8->pc] << 8) | (unsigned short)(chip8->memory[chip8->pc]))

void destroyChip8(Chip8* chip8);

void loadROMChip8(Chip8* chip8, char* rom);

void chip8Step(Chip8* chip8);

void executeOpcode(Chip8* chip8, short opcode);


#endif // CHIP8_H_
