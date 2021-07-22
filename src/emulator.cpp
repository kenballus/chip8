#include <cstdint>
#include <algorithm> // for std::min
#include <cstring>
#include <iostream> // for reading rom into memory

#include "emulator.hpp"

using std::uint8_t;
using std::uint16_t;

Chip8::Chip8() {
    uint8_t font[] = {
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
        0xF0, 0x80, 0xF0, 0x80, 0x80, // F
    };
    std::memcpy(memory, font, 5 * 15);

    int ctr = 0;
    while (std::cin.peek() != -1) {
        memory[0x200 + ctr] = std::cin.get();
        ctr++;
    }
    std::cout << "Copied " << std::dec << ctr << std::hex << " bytes into rom memory." << std::endl;
}

void Chip8::cls() {
    std::cout << "Clearing screen." << std::endl;
    memset(screen, 0, NUM_PX);
}

void Chip8::ret() {
    std::cout << "Returning to subroutine called from address 0x" << stack[sp] << std::endl;
    pc = stack[sp--];
}

void Chip8::jmp(uint16_t addr) {
    std::cout << "Jumping to 0x" << addr << std::endl;
    pc = addr - INSTRUCTION_SIZE;
}

void Chip8::call(uint16_t addr) {
    std::cout << "Calling subroutine at 0x" << addr << std::endl;
    sp++;
    stack[sp] = pc;
    pc = addr - INSTRUCTION_SIZE;
}

void Chip8::seval(uint8_t reg, uint8_t val) {
    if (registers[reg] == val) {
        pc += INSTRUCTION_SIZE;
    }
}

void Chip8::sneval(uint8_t reg, uint8_t val) {
    if (registers[reg] != val) {
        pc += INSTRUCTION_SIZE;
    }
}

void Chip8::sereg(uint8_t reg1, uint8_t reg2) {
    if (registers[reg1] == registers[reg2]) {
        pc += INSTRUCTION_SIZE;
    }
}

void Chip8::ldval(uint8_t reg, uint8_t val) {
    registers[reg] = val;
}

void Chip8::addval(uint8_t reg, uint8_t val) {
    registers[reg] += val;
}

void Chip8::ldreg(uint8_t reg1, uint8_t reg2) {
    registers[reg1] = reg2;
}

void Chip8::orreg(uint8_t reg1, uint8_t reg2) {
    registers[reg1] |= registers[reg2];
}

void Chip8::andreg(uint8_t reg1, uint8_t reg2) {
    registers[reg1] &= registers[reg2];
}

void Chip8::xorreg(uint8_t reg1, uint8_t reg2) {
    registers[reg1] ^= registers[reg2];
}

void Chip8::addreg(uint8_t reg1, uint8_t reg2) {
    uint8_t prev = std::min(registers[reg1], registers[reg2]);
    registers[reg1] += registers[reg2];
    if (prev > registers[reg1]) {
        registers[0xF] = 1;
    }
}

void Chip8::subreg(uint8_t reg1, uint8_t reg2) {
    if (registers[reg1] < registers[reg2]) {
        registers[0xF] = 1;
    }
    registers[reg1] -= registers[reg2];
}

void Chip8::shr(uint8_t reg1, uint8_t reg2) {
    if ((registers[reg1] & 1) != 0) {
        registers[0xF] = 1;
    }
    registers[reg1] >>= 1;
}

void Chip8::subnreg(uint8_t reg1, uint8_t reg2) {
    if (registers[reg1] > registers[reg2]) {
        registers[0xF] = 1;
    }
    registers[reg1] = registers[reg2] - registers[reg1];
}

void Chip8::shl(uint8_t reg1, uint8_t reg2) {
    if ((registers[reg1] & 1<<7) != 0) {
        registers[0xF] = 1;
    }
    registers[reg1] <<= 1;
}

void Chip8::snereg(uint8_t reg1, uint8_t reg2) {
    if (registers[reg1] != registers[reg2]) {
        pc += INSTRUCTION_SIZE;
    }
}

void Chip8::ldi(uint16_t val) {
    i = val;
}

void Chip8::jmpv0(uint16_t val) {
    pc = registers[0] + val;
}

void Chip8::rnd(uint8_t reg, uint16_t mask) {
    registers[reg] = (rand() % 255) & mask;
}

void Chip8::toggle_pixel(uint8_t reg1, uint8_t reg2) {
    screen[registers[reg1] * CHIP8_SCREEN_WIDTH + registers[reg2]] ^= 1;
}

void Chip8::drw(uint8_t reg1, uint8_t reg2, uint8_t bytes_in_sprite) {
    for (uint ctr = 0; ctr < bytes_in_sprite; ctr++) {
        screen[registers[reg1] * CHIP8_SCREEN_WIDTH + registers[reg2] + ctr] ^= memory[i];
        if (screen[registers[reg1] * CHIP8_SCREEN_WIDTH + registers[reg2] + ctr] == 0 && memory[i] == 1) {
            registers[0xF] = 1;
        }
    }
}

void Chip8::skp(uint8_t reg) {
    if (keys_pressed[registers[reg]]) {
        pc += INSTRUCTION_SIZE;
    }
}

void Chip8::sknp(uint8_t reg) {
    if (!keys_pressed[registers[reg]]) {
        pc += INSTRUCTION_SIZE;
    }
}

void Chip8::lddt(uint8_t reg) {
    registers[reg] = dt;
}

void Chip8::ldk(uint8_t reg) {
    waiting_for_key = true;
    for (int i = 0; i < NUM_KEYS; i++) {
        if (keys_pressed[i]) {
            waiting_for_key = false;
            registers[reg] = i;
            return;
        }
    }
}

void Chip8::ldintodt(uint8_t reg) {
    dt = registers[reg];
}

void Chip8::ldintost(uint8_t reg) {
    st = registers[reg];
}

void Chip8::addi(uint8_t reg) {
    i += reg;
}

void Chip8::ldf(uint8_t reg) {
    i = registers[reg] * 5; // Each character in a font is 5 bytes.
}

void Chip8::ldb(uint8_t reg) {
    auto hundreds = registers[reg] / 100;
    auto tens = (registers[reg] - 100 * hundreds) / 10;
    auto ones = registers[reg] - 100 * hundreds - 10 * tens;

    memory[i] = hundreds;
    memory[i + 1] = tens;
    memory[i + 2] = ones;
}

void Chip8::storange(uint8_t last_reg) {
    for (int j = 0; j < NUM_REGISTERS; j++) {
        memory[i + j] = registers[j];
    }
}

void Chip8::ldrange(uint8_t last_reg) {
    for (int j = 0; j < NUM_REGISTERS; j++) {
        registers[j] = memory[i + j];
    }
}

void Chip8::beep() {
    
}

void Chip8::execute() {
    pc += INSTRUCTION_SIZE;
    apply_opcode(memory[pc] << 8 | memory[pc + 1]);
}

void Chip8::update_timers() {
    if (st > 0) {
        st--;
    }
    if (dt > 0) {
        dt--;
        beep();
    }
}

void Chip8::apply_opcode(uint16_t opcode) {
    std::cout << "Executing opcode: 0x" << opcode << " at address 0x" << pc << std::endl;
    if (opcode == 0x0000) {
        std::cout << "Quitting." << std::endl;
        dump_state();
        // dump_mem();
        exit(0);
    }
    else if (opcode == 0x00e0) {
        cls();
    }
    else if (opcode == 0x00ee) {
        ret();
    }
    else if (0x1000 <= opcode && opcode <= 0x1fff) {
        jmp(opcode & 0x0fff);
    }
    else if (0x2000 <= opcode && opcode <= 0x2fff) {
        call(opcode & 0x0fff);
    }
    else if (0x3000 <= opcode && opcode <= 0x3fff) {
        seval((opcode & 0x0f00) >> 8, opcode & 0x00ff);
    }
    else if (0x4000 <= opcode && opcode <= 0x4fff) {
        sneval((opcode & 0x0f00) >> 8, opcode & 0x00ff);
    }
    else if (0x5000 <= opcode && opcode <= 0x5ff0) {
        sereg((opcode & 0x0f00) >> 8, (opcode & 0x00f0) >> 4);
    }
    else if (0x6000 <= opcode && opcode <= 0x6fff) {
        ldval((opcode & 0x0f00) >> 8, opcode & 0x00ff);
    }
    std::cout << std::endl;
}

void Chip8::dump_state() {
    for (int ctr = 0; ctr < 16; ctr++) {
        std::cout << "R" << ctr << ": " << (unsigned int)registers[ctr] << " ";
    }
    std::cout << "\n";
    std::cout << "SP: " << (unsigned int)sp << " "
              << "ST: " << (unsigned int)st << " "
              << "DT: " << (unsigned int)dt << " "
              << "I:  " << (unsigned int)i  << " "
              << "PC: " << (unsigned int)pc << std::endl;
}

void Chip8::dump_mem() {
    for (int ctr = 0; ctr < 4096; ctr++) {
        if (ctr % 32 == 0) {
            std::cout << "\n0x" << ctr << ":\t";
        }

        if (memory[ctr] <= 0xf) {
            std::cout << "0";
        }
        std::cout << (int)memory[ctr] << " ";
    }
}