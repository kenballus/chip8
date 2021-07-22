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
    // std::cout << "Clearing screen." << std::endl;
    memset(screen, 0, NUM_PX);
}

void Chip8::ret() {
    // std::cout << "Returning from subroutine to address 0x" << stack[sp] << std::endl;
    pc = stack[sp--];
}

void Chip8::jmp(uint16_t addr) {
    // std::cout << "Jumping to 0x" << addr << std::endl;
    pc = addr - INSTRUCTION_SIZE;
}

void Chip8::call(uint16_t addr) {
    // std::cout << "Calling subroutine at 0x" << addr << std::endl;
    sp++;
    stack[sp] = pc;
    pc = addr - INSTRUCTION_SIZE;
}

void Chip8::seval(uint8_t reg, uint8_t val) {
    // std::cout << "Comparing R" << (unsigned int)reg << " to value " << (unsigned int)val << " and they " << (registers[reg] == val ? " match; skipping next instruction" : " differ; doing nothing") << std::endl;
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
// std::cout << "Comparing R" << (unsigned int)reg1 << " to R" << (unsigned int)reg2 << " and they " << (registers[reg1] == registers[reg2] ? " match; skipping next instruction" : " differ; doing nothing") << std::endl;
    if (registers[reg1] == registers[reg2]) {
        pc += INSTRUCTION_SIZE;
    }
}

void Chip8::ldval(uint8_t reg, uint8_t val) {
    // std::cout << "Loading value " << (unsigned int)val << " into R" << (unsigned int)reg << std::endl;
    registers[reg] = val;
}

void Chip8::addval(uint8_t reg, uint8_t val) {
    // std::cout << "Adding " << (unsigned int)val << " to R" << (unsigned int)reg << ", currently containing " << (unsigned int)registers[reg] << std::endl;
    registers[reg] += val;
}

void Chip8::ldreg(uint8_t reg1, uint8_t reg2) {
    // std::cout << "Loading value " << (unsigned int)registers[reg2] << " from R" << (unsigned int)reg2 << " into R" << (unsigned int)reg1 << std::endl;
    registers[reg1] = registers[reg2];
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

void Chip8::toggle_pixel(uint8_t row, uint8_t col) {
    screen[(row % CHIP8_SCREEN_HEIGHT) * CHIP8_SCREEN_WIDTH + col % CHIP8_SCREEN_WIDTH] ^= 1;
}

bool Chip8::get_pixel(uint8_t row, uint8_t col) {
    return screen[(row % CHIP8_SCREEN_HEIGHT) * CHIP8_SCREEN_WIDTH + col % CHIP8_SCREEN_WIDTH];
}

void Chip8::drw(uint8_t reg1, uint8_t reg2, uint8_t bytes_in_sprite) {
    for (int byte_ctr = 0; byte_ctr < bytes_in_sprite; byte_ctr++) {
        for (int bit_ctr = 7; bit_ctr >= 0; bit_ctr--) {
            if (memory[i + byte_ctr] & 1<<bit_ctr) {
                toggle_pixel(registers[reg2] + byte_ctr, registers[reg1] + 7 - bit_ctr);
                if (not get_pixel(registers[reg2] + byte_ctr, registers[reg1] + 7 - bit_ctr)) {
                    registers[0xF] = 1;
                }
            }
            
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
    memory[i] = registers[reg] / 100; // hundreds
    memory[i + 1] = (registers[reg] % 100) / 10; // tens
    memory[i + 2] = registers[reg] % 10; // ones
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

void Chip8::crash() {
    std::cout << "Quitting." << std::endl;
    dump_state();
    // dump_mem();
    exit(0);
}

void Chip8::apply_opcode(uint16_t opcode) {
    // std::cout << "Executing opcode 0x" << opcode << " at address 0x" << pc << std::endl;
    if (opcode == 0x0000) {
        crash();
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
    else if (0x5000 <= opcode && opcode <= 0x5fff && (opcode & 0x000f) == 0x0) { // starts with 5 and ends with 0
        sereg((opcode & 0x0f00) >> 8, (opcode & 0x00f0) >> 4);
    }
    else if (0x6000 <= opcode && opcode <= 0x6fff) {
        ldval((opcode & 0x0f00) >> 8, opcode & 0x00ff);
    }
    else if (0x7000 <= opcode && opcode <= 0x7fff) {
        addval((opcode & 0x0f00) >> 8, opcode & 0x00ff);
    }
    else if (0x8000 <= opcode && opcode <= 0x8fff && (opcode & 0x000f) == 0x0) { // starts with 8 and ends with 0
        ldreg((opcode & 0x0f00) >> 8, (opcode & 0x00f0) >> 4);
    }
    else if (0x8000 <= opcode && opcode <= 0x8fff && (opcode & 0x000f) == 0x1) { // starts with 8 and ends with 1
        orreg((opcode & 0x0f00) >> 8, (opcode & 0x00f0) >> 4);
    }
    else if (0x8000 <= opcode && opcode <= 0x8fff && (opcode & 0x000f) == 0x2) { // starts with 8 and ends with 2
        andreg((opcode & 0x0f00) >> 8, (opcode & 0x00f0) >> 4);
    }
    else if (0x8000 <= opcode && opcode <= 0x8fff && (opcode & 0x000f) == 0x3) { // starts with 8 and ends with 3
        xorreg((opcode & 0x0f00) >> 8, (opcode & 0x00f0) >> 4);
    }
    else if (0x8000 <= opcode && opcode <= 0x8fff && (opcode & 0x000f) == 0x4) { // starts with 8 and ends with 4
        addreg((opcode & 0x0f00) >> 8, (opcode & 0x00f0) >> 4);
    }
    else if (0x8000 <= opcode && opcode <= 0x8fff && (opcode & 0x000f) == 0x5) { // starts with 8 and ends with 5
        subreg((opcode & 0x0f00) >> 8, (opcode & 0x00f0) >> 4);
    }
    else if (0x8000 <= opcode && opcode <= 0x8fff && (opcode & 0x000f) == 0x6) { // starts with 8 and ends with 6
        shr((opcode & 0x0f00) >> 8, (opcode & 0x00f0) >> 4);
    }
    else if (0x8000 <= opcode && opcode <= 0x8fff && (opcode & 0x000f) == 0x7) { // starts with 8 and ends with 7
        subnreg((opcode & 0x0f00) >> 8, (opcode & 0x00f0) >> 4);
    }
    else if (0x8000 <= opcode && opcode <= 0x8fff && (opcode & 0x000f) == 0xe) { // starts with 8 and ends with e
        shl((opcode & 0x0f00) >> 8, (opcode & 0x00f0) >> 4);
    }
    else if (0x9000 <= opcode && opcode <= 0x9fff && (opcode & 0x000f) == 0x0) { // starts with 9 and ends with 0
        snereg((opcode & 0x0f00) >> 8, (opcode & 0x00f0) >> 4);
    }
    else if (0xa000 <= opcode && opcode <= 0xafff) {
        ldi(opcode & 0x0fff);
    }
    else if (0xb000 <= opcode && opcode <= 0xbfff) {
        jmpv0(opcode & 0x0fff);
    }
    else if (0xc000 <= opcode && opcode <= 0xcfff) {
        rnd((opcode & 0x0f00) >> 8, opcode & 0x00ff);
    }
    else if (0xd000 <= opcode && opcode <= 0xdfff) {
        drw((opcode & 0x0f00) >> 8, (opcode & 0x00f0) >> 4, opcode & 0x000f);
    }
    else if (0xe000 <= opcode && opcode <= 0xefff && (opcode & 0x00ff) == 0x9e) { // starts with e and ends with 9e
        skp((opcode & 0x0f00) >> 8);
    }
    else if (0xe000 <= opcode && opcode <= 0xefff && (opcode & 0x00ff) == 0xa1) { // starts with e and ends with a1
        sknp((opcode & 0x0f00) >> 8);
    }
    else if (0xf000 <= opcode && (opcode & 0x00ff) == 0x07) { // starts with f and ends with 07
        lddt((opcode & 0x0f00) >> 8);
    }
    else if (0xf000 <= opcode && (opcode & 0x00ff) == 0x0a) { // starts with f and ends with 0a
        ldk((opcode & 0x0f00) >> 8);
    }
    else if (0xf000 <= opcode && (opcode & 0x00ff) == 0x15) { // starts with f and ends with 15
        ldintodt((opcode & 0x0f00) >> 8);
    }
    else if (0xf000 <= opcode && (opcode & 0x00ff) == 0x18) { // starts with f and ends with 18
        ldintost((opcode & 0x0f00) >> 8);
    }
    else if (0xf000 <= opcode && (opcode & 0x00ff) == 0x1e) { // starts with f and ends with 1e
        addi((opcode & 0x0f00) >> 8);
    }
    else if (0xf000 <= opcode && (opcode & 0x00ff) == 0x29) { // starts with f and ends with 29
        ldf((opcode & 0x0f00) >> 8);
    }
    else if (0xf000 <= opcode && (opcode & 0x00ff) == 0x33) { // starts with f and ends with 33
        ldb((opcode & 0x0f00) >> 8);
    }
    else if (0xf000 <= opcode && (opcode & 0x00ff) == 0x55) { // starts with f and ends with 55
        storange((opcode & 0x0f00) >> 8);
    }
    else if (0xf000 <= opcode && (opcode & 0x00ff) == 0x65) { // starts with f and ends with 65
        ldrange((opcode & 0x0f00) >> 8);
    }
    else {
        std::cout << "Unknown opcode " << opcode << std::endl;
    }
}

void Chip8::dump_state() {
    for (int ctr = 0; ctr < 16; ctr++) {
        std::cout << "R" << ctr << ": " << (registers[ctr] < 0xF ? "0" : "") << (unsigned int)registers[ctr] << " ";
        if (ctr == 7) {
            std::cout << "\n";
        }
    }
    std::cout << "\n" << "ST: " << (unsigned int)st << " " << "DT: " << (unsigned int)dt << " " << "I:  " << (unsigned int)i  << " " << "SP: " << (unsigned int)sp << " " << "PC: " << (unsigned int)pc << std::endl;
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