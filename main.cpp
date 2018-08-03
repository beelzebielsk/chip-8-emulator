#include <cstring>
#include <iostream>

/* Source: <https://en.wikipedia.org/wiki/CHIP-8#Virtual_machine_description>
 * - 4K of memory locations, each of which are 8 bits wide (one byte).
 * - 16 8-bit data registers, named V0 through VF.
 */
#define MEMSIZE 4096
#define NUMREGISTERS 16
#define SCREENHEIGHT 64
#define SCREENWIDTH 32
#define SCREENSIZE SCREENHEIGHT * SCREENWIDTH
unsigned char memory[MEMSIZE];
unsigned char registers[NUMREGISTERS];


/* Source: <http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/>
 * - The program counter is a 16-bit register (because memory
 *   addresses go from 0 to 4095 (4096 locations in all), which
 *   requires two bytes to store.
 * - There's another register called the index register, which is also
 *   16 bits wide.
 * - There's two timers: the delay timer and the sound timer. They're
 *   both 60Hz timers. The timers work by holding the current tick out
 *   of the current 60Hz cycle. So they'll count down: 59, 58, ... 1,
 *   0, 59, ...
 *   We're going to create variables which hold the counter's state.
 * - There's also a system stack. The stack can be as big or as large
 *   as you want, I guess, based on information from Wikipedia. This
 *   guy makes a 32 byte stack, which means it can store 16 memory
 *   addresses (they're two bytes wide, bc we go up to 4096 (12
 *   bits)).
 * - The chip8 system expects a hex-keypad (0x0 -> 0xF). The article
 *   says to represent this with an array of length 16. That makes
 *   sense... if we plan to represent possible simultaneous
 *   keypresses.
 */
unsigned short pc;
unsigned char sp;
unsigned short indexRegister;
unsigned short stack[16];

unsigned char screenBuffer[SCREENSIZE];
unsigned char delayTimer, soundTimer;

unsigned char keyState[16];


namespace chip8 {
    using std::cout;
    using std::endl;
    void initialize() {
        std::memset(memory, (unsigned char) 0, MEMSIZE);
        std::memset(registers, (unsigned char) 0, NUMREGISTERS);
        std::memset(screenBuffer, (unsigned char) 0, SCREENSIZE);
    }

    void emulateCycle() {
        // Fetch
        unsigned short instruction = *(unsigned short *)(memory + pc);
        cout << instruction << endl;
        // Decode
        // Execute
    }
}

int
main() {
    chip8::initialize();
    chip8::emulateCycle();
}
