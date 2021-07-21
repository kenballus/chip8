#pragma once

#include <stdint.h>

int const CHIP8_SCREEN_WIDTH = 64;
int const CHIP8_SCREEN_HEIGHT = 32;
int const NUM_PX = CHIP8_SCREEN_WIDTH * CHIP8_SCREEN_HEIGHT;
int const NUM_KEYS = 16;
int const NUM_REGISTERS = 16;

class Chip8 {
public:
    uint8_t registers[NUM_REGISTERS] = {0};
    
    uint8_t dt = 0;
    uint8_t st = 0;

    uint16_t i = 0; // This is a bad name
    uint16_t pc = 0x200;
    uint16_t sp = 0;

    uint8_t memory[4096] = {0};
    uint16_t stack[4096] = {0};

    bool screen[NUM_PX] = {0};
    bool keys_pressed[NUM_KEYS] = {0};
    bool waiting_for_key = false;

    void toggle_pixel(uint8_t reg1, uint8_t reg2);
    void apply_opcode(uint16_t opcode);

public:
    Chip8();
    void cls();
    void ret();
    void jmp(uint16_t addr);
    void call(uint16_t addr);
    void seval(uint8_t reg, uint8_t val);
    void sneval(uint8_t reg, uint8_t val);
    void sereg(uint8_t reg1, uint8_t reg2);
    void ldval(uint8_t reg, uint8_t val);
    void addval(uint8_t reg, uint8_t val);
    void ldreg(uint8_t reg1, uint8_t reg2);
    void orreg(uint8_t reg1, uint8_t reg2);
    void andreg(uint8_t reg1, uint8_t reg2);
    void xorreg(uint8_t reg1, uint8_t reg2);
    void addreg(uint8_t reg1, uint8_t reg2);
    void subreg(uint8_t reg1, uint8_t reg2);
    void shr(uint8_t reg1, uint8_t reg2);
    void subnreg(uint8_t reg1, uint8_t reg2);
    void shl(uint8_t reg1, uint8_t reg2);
    void snereg(uint8_t reg1, uint8_t reg2);
    void ldi(uint16_t val);
    void jmpv0(uint16_t val);
    void rnd(uint8_t reg, uint16_t mask);
    void drw(uint8_t reg1, uint8_t reg2, uint8_t bytes_in_sprite);
    void skp(uint8_t reg);
    void sknp(uint8_t reg);
    void lddt(uint8_t reg);
    void ldintodt(uint8_t reg);
    void ldintost(uint8_t reg);
    void addi(uint8_t reg);
    void ldf(uint8_t reg);
    void ldk(uint8_t reg);
    void ldb(uint8_t reg);
    void storange(uint8_t last_reg);
    void ldrange(uint8_t last_reg);

    void update();
    void dump_state();
    void dump_mem();
};