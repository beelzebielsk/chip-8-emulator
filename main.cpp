#include <cstring>
#include <iostream>
#include <iomanip>
#include <exception>
#include <cstdio>

/* Source: <https://en.wikipedia.org/wiki/CHIP-8#Virtual_machine_description>
 * - 4K of memory locations, each of which are 8 bits wide (one byte).
 * - 16 8-bit data registers, named V0 through VF.
 */
constexpr unsigned int MEM_SIZE = 4096; // Size of the memory, in bytes.
constexpr unsigned int SCREEN_HEIGHT = 32; // Pixels
constexpr unsigned int SCREEN_WIDTH = 64; // Pixels
constexpr unsigned int SCREEN_SIZE = SCREEN_HEIGHT * SCREEN_WIDTH;
constexpr unsigned int NUM_REGISTERS = 16;

using byte = unsigned char;
using word = union {
    unsigned short WORD;
    struct {
        byte low, high;
    };
};
byte memory[MEM_SIZE];
byte registers[NUM_REGISTERS];

/* These two functions are written as they are to convert between the
 * chip8's big-endian memory (big-endian for instructions, though most
 * everything else is just 8-bits wide, making endianness
 * inconsequential). I read in the correct order and write in the
 * correct order, assuming the host is little-endian.
 *
 * Source: <https://en.wikipedia.org/wiki/CHIP-8#Opcode_table>
 */
word readWord(byte* memory, word address) {
    word location;
    location.WORD = memory[address.WORD] << 8 | memory[address.WORD + 1];
    return location;
}

void writeWord(byte* memory, word address, word value) {
    memory[address.WORD] = value.high;
    memory[address.WORD + 1] = value.low;
}

std::ostream& operator<<(std::ostream& o, word w) {
    return o << "0x" << std::hex << std::setw(4) << std::setfill('0') << w.WORD;
}


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
word pc;
/* We're going to trust that the stack pointer points to a location in
 * the stack array. If no one tries to fudge with it directly, and
 * stick to just pushes and pops, we should be fine. However, there's
 * really only 16 entries in there, so hopefully that works.
 */
word sp;
word instruction;
word indexRegister;
word stack[16];

byte screenBuffer[SCREEN_SIZE];
byte delayTimer, soundTimer;

byte keyState[16];

namespace chip8 {
    using std::cout;
    using std::endl;
    using ::memory;
    void initialize() {
        std::memset(memory, 0, MEM_SIZE);
        std::memset(registers, 0, NUM_REGISTERS);
        std::memset(screenBuffer, 0, SCREEN_SIZE);
        std::memset(stack, 0, sizeof(stack));
        // Chip8 assumes that the application is loaded to this memory
        // location.
        pc.WORD = 0x200;
        sp.WORD = 0;
        instruction.WORD = 0;
        indexRegister.WORD = 0;

        /* space of a single letter sprite: 4x5 = 20 bytes (0x14
         *   bytes)
         * - space of 16 sprites = 320 bytes (0x140 bytes)
         * - start the sprites at location 0x50 (location 80, the 81st
         *   byte)
         * - so the fonts should end at 400th byte, (0x190)
         * - that leaves 112 bytes of space between the font and the
         *   start of the application
         */

        // TODO: Load the application.
        FILE* application = fopen("(null)", "rb");
        //fread(memory + 0x200, sizeof(byte), 0xFFF - 0x200 + 1, application);
        fread(memory + 0x200, sizeof(byte), 380, application);
        fclose(application);
    }

    void emulateCycle() {
        // Fetch
        cout << instruction << endl;
        word highNibble;
        word lowNibble;
        word address;
        byte constant;
        instruction = readWord(memory, pc);
        // 0x_XXX;
        highNibble.WORD = instruction.WORD & 0xF000;
        //0xX_XX;
        byte name1 = instruction.high & 0x0F;
        //0xXX_X;
        byte name2 = instruction.low & 0xF0;
        // 0xXXX_;
        lowNibble.WORD = instruction.low & 0x0F;
        // 0xX___;
        address.WORD = instruction.WORD & 0x0FFF;
        // 0xXX__;
        constant = instruction.low;
        // Decode and Execute
        switch (highNibble.low) {
            case 0x1:
                pc.WORD = instruction.WORD & 0x0FFF;
                goto skipIncrement;
                break;
            // Call subroutine.
            case 0x2:
                // + 2 because we want to return to the instruction
                // after the call, not the to call instruction itself.
                stack[sp.WORD].WORD = pc.WORD + 2;
                sp.WORD += 2;
                pc.WORD = address.WORD;
                goto skipIncrement;
                break;
            // Skip instruction if equal.
            case 0x3:
                if (registers[name1] == constant) {
                    pc.WORD += 4;
                    goto skipIncrement;
                }
                break;
            // Skip next instruction if not equal.
            case 0x4:
                if (registers[name1] != constant) {
                    pc.WORD += 4;
                    goto skipIncrement;
                }
                break;
            case 0x5:
                if (registers[name1] == registers[name2]) {
                    pc.WORD += 4;
                    goto skipIncrement;
                }
                break;
            case 0x6:
                registers[name1] = constant;
                break;
            case 0x7:
                registers[name1] += constant;
                break;
            case 0x8:
                switch (lowNibble.WORD) {
                    // Assignment.
                    case 0:
                        registers[name1] = registers[name2];
                        break;
                    case 1:
                        registers[name1] |= registers[name2];
                        break;
                    case 2:
                        registers[name1] &= registers[name2];
                        break;
                    case 3:
                        registers[name1] ^= registers[name2];
                        break;
                    case 4:
                        // TODO: Implement the carry flag.
                        registers[name1] += registers[name2];
                        break;
                    case 5:
                        // TODO: Implement the borrow flag.
                        registers[name1] -= registers[name2];
                        break;
                    case 6:
                        registers[name1] = registers[name2] >> 1;
                        break;
                    case 7:
                        registers[name1] = registers[name2] - registers[name1];
                        break;
                    case 0xE:
                        // Set register F (15) to the most significant
                        // bit of registers[name2];
                        registers[0xF] = registers[name2] & 0x80;
                        registers[name1] = registers[name2] << 1;
                        break;
                }
                break;
            case 0x9:
                if (registers[name1] != registers[name2]) {
                    pc.WORD += 4;
                    goto skipIncrement;
                }
            case 0xA:
                indexRegister.WORD = address.WORD;
                break;
            case 0xB:
                indexRegister.WORD = address.WORD + registers[0];
                break;
            case 0xC: {
                // TODO: Make random actually random.
                byte random = 1;
                registers[name1] = constant & random;
                break;
            }
            case 0xD:
                /* TODO: Impelement sprite drawing.
                 * This requires:
                 * - Figuring out how to draw to a screen.
                 * - Reading up on how the graphics memory is laid
                 *   out (like, if we're drawing a 4x5 box of pixels,
                 *   how do I read that? Are sprites stored as bits?
                 *   Bytes? Do I have to update memory when I draw?
                 */
                // In this one instance, the constant is a nibble, and
                // not a byte.
                constant &= 0x0F;
                break;
            case 0xE:
                switch (instruction.low) {
                    case 0x9E:
                        if (keyState[registers[name1]]) {
                            pc.WORD += 4;
                            goto skipIncrement;
                        }
                        break;
                    case 0xA1:
                        if (!keyState[registers[name1]]) {
                            pc.WORD += 4;
                            goto skipIncrement;
                        }
                        break;
                }
                break;
            case 0xF:
                switch (instruction.low) {
                    case 0x07:
                        registers[name1] = delayTimer;
                        break;
                        /*
                    case 0x0A:
                        // TODO: Create blocking operation for key
                        // press. Blocking means truly blocking.
                        // Instructions halt entirely, because this is
                        // a one program machine.
                        registers[name1] = delayTimer;
                        break;
                        */
                    case 0x15:
                        delayTimer = registers[name1];
                        break;
                    case 0x18:
                        soundTimer = registers[name1];
                        break;
                    case 0x1E:
                        indexRegister.WORD += registers[name1];
                        break;
                    //case 0x29:
                        // TODO: Set index register to the
                        //break;
                    case 0x33: {
                        byte bcdValue = registers[name1];
                        memory[indexRegister + 2] = bcdValue % 10;
                        bcdValue /= 10;
                        memory[indexRegister + 1] = bcdValue % 10;
                        bcdValue /= 10;
                        memory[indexRegister] = bcdValue % 10;
                        break;
                    }
                    case 0x55:
                        /* Copy all registers from V0 to VX, including
                         * VX, to the memory location starting at I,
                         * where V0 is the 1st register, VX is the Xth
                         * register.
                         * Thus, name1 + 1 registers must be copied,
                         * because if name1 = 0, we'd have to copy 1
                         * register.
                         */
                        std::memcpy(memory + indexRegister.WORD, registers, name1 + 1);
                        break;
                    case 0x65:
                        std::memcpy(registers, memory + indexRegister.WORD, name1 + 1);
                        break;
                }
                break;
            default:
                std::cerr << "Opcode " << instruction << " not implemented!" << std::endl;
                throw std::exception();

        }
        pc.WORD += 2;
skipIncrement:
        // This is a stupid, idiotic hack, to have a place to jump to.
        2;
    }
}

int
main() {
    // Set up render system. Graphics stuff from the host computer.
    
    // Initialize the emulator.
    chip8::initialize();

    // for (;;) {
        // Emulation Loop.
        chip8::emulateCycle();
        // if the draw flag is set, then draw.
        // Read the current state of the keys.
    // }
}
