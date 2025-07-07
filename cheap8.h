#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

#include "renderer.h"



std::string charToHex(unsigned char c) {
    std::stringstream ss;
    ss << std::setw(2) << std::setfill('0') << std::hex << int(c);
    return ss.str();
}

struct Cheap8 {

    // Fontset
    unsigned char chip8_fontset[80] =
    { 
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


    void init() {
        pc = 0x200;
        opcode = 0;
        I = 0;
        s_ptr = 0;

        // Init fontset

        // Init renderer
        renderer.init();
    }

    std::vector<char> read_file(const std::string path) {
        std::filesystem::path inputFilePath{path};
        auto length = std::filesystem::file_size(inputFilePath);
        if (length == 0) {
            return {};
        }

        std::vector<char> buffer(length);
        std::ifstream inputFile(path, std::ios_base::binary);
        inputFile.read(reinterpret_cast<char*>(buffer.data()), length);
        inputFile.close();
        return buffer;
    }

    void load(const std::string path) {

        auto buffer = read_file(path);

        for (int i = 0; i < buffer.size(); ++i) {
            // 512 as program generally starts at location 0x200 = 512 in memory
            memory[i + 512] = buffer[i];
        }
    }

    void print_program() {
        for (int i = 0; i < 4096; ++i) {
            std::cout << charToHex(memory[i]) << " ";
            if (i % 10 == 0)
                std::cout << std::endl;
        }
    }

    void next() {
        // Read cur opcode
        opcode = memory[pc] << 8 | memory[pc + 1];
        // std::cout << "opcode:" << opcode << std::endl;
        // Execute opcode
        switch (opcode & 0xF000) {
            case 0x0000:
                switch(opcode) {
                    case 0x00E0:
                    clear();
                    break;
                    case 0x00EE:
                    ret();
                    break;

                }
            break;
            case 0x1000:
            jp();
            break;
            case 0x2000:
            call();
            break;
            case 0x3000:
            se_vx();
            break;
            case 0x4000:
            sne_vx();
            break;
            case 0x5000:
            se_vx_vy();
            break;
            case 0x6000:
            ld_vx(opcode);
            break;
            case 0x7000:
            add_vx();
            break;
            case 0x8000:
                switch (opcode & 0x000F) {
                    case 0x0000:
                    ld_vx_vy();
                    break;
                    case 0x0001:
                    or_vx_vy();
                    break;
                    case 0x0002:
                    and_vx_vy();
                    break;
                    case 0x0003:
                    xor_vx_vy();
                    break;
                    case 0x0004:
                    add_vx_vy();
                    break;
                    case 0x0005:
                    sub_vx_vy();
                    break;
                    case 0x0006:
                    shr_vx();
                    break;
                }
            break;
            case 0x9000:
            sne_vx_vy();
            break;
            case 0xD000:
            draw_sprite();
            break;
            case 0xA000:
            ld_i(opcode);
            break;
            default:
            std::cout << "end" << std::endl;
            break;
        }
    }

    inline short _0x000F() {
        return opcode & 0x000F;
    }
    inline short _0x00F0() {
        return (opcode & 0x00F0) >> 4;
    }
    inline short _0x0F00() {
        return (opcode & 0x0F00) >> 8;
    }
    inline short _0x00FF() {
        return opcode & 0x00FF;
    }
    inline short _0x0FFF() {
        return opcode & 0x0FFF;
    }
    
    unsigned char& VX() {
        return V[_0x0F00()];
    }

    unsigned char& VY() {
        return V[_0x00F0()];
    }

    unsigned char& VC() {
        return V[0xF];
    }

    


    void ret() {
        pc = stack[--s_ptr];
        pc += 2;
    }

    void jp() {
        pc = _0x0FFF();
    }

    void call() {
        stack[s_ptr++] = pc;
        jp();
    }

    void ld_i(short opcode) {
        I = _0x0FFF();
        pc+=2;
    }

    void ld_vx(short opcode) {
        VX() = _0x00FF();
        pc+=2;
    }

    void add_vx() {
        VX() += _0x00FF();
        pc += 2;
    }

    void ld_vx_vy() {
        VX() = VY();
        pc += 2;
    }

    void or_vx_vy() {
        VX() |= VY();
        pc += 2;
    }

    void and_vx_vy() {
        VX() &= VY();
        pc += 2;
    }

    void xor_vx_vy() {
        VX() ^= VY();
        pc += 2;
    }

    void add_vx_vy() {
        short res = VX() + VY();
        if (res > 255)
            VC() = 1;

        VX() = res;
        pc += 2;
    }

    void sub_vx_vy() {
        if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
            VC() = 1;
            
        V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
        pc += 2;
    }

    // Not sure of this
    void shr_vx() {
        V[(opcode & 0x0F00) >> 8] >>= 1;
        if (V[(opcode & 0x0F00) & (0x80 >> 7)] == 1)
            VC() = 1;

        pc += 2;
    }

    void se_vx() {
        if (VX() == _0x00FF()) {
            pc += 2;
        }
        pc += 2;
    }

    void sne_vx() {
        if (VX() != _0x00FF()) {
            pc += 2;
        }
        pc += 2;
    }

    void se_vx_vy() {
        if (VX() == VY()) {
            pc += 2;
        }
        pc += 2;
    }

    void sne_vx_vy() {
        if (VX() != VY()) {
            pc += 2;
        }
        pc += 2;
    }

    void clear() {
        for (int i = 0; i < SCREEN_SIZE; ++i) {
            screen[i] = 0;
        }
        pc+=2;
    }

    void draw_sprite() {
        VC() = 0;

        // Decode opcode
        short x = VX();
        short y = VY();
        short h = _0x000F();

        for (int row = 0; row < h; ++row) {
            draw_row(x, y, row, memory[I + row]);
        }
        draw_flag = true;
        pc += 2;
    }

    void draw_row(short x, short y, short nrow, short row) {
        for (int col = 0; col < 8; ++col) {
            if ((row & (0x80 >> col)) != 0) {
                auto &val = screen[x + col + (y + nrow) * 64];
                if (val)
                    VC() = 1;

                screen[x + col + (y + nrow) * 64] ^= true;
            }
        }
    }

    void loop() {
        while (!renderer.should_quit) {
            // if (draw_flag) {
                renderer.render(screen);
                draw_flag = false;
            // }
            next();

            renderer.poll_events();
        }
    }

    void clean() {
        renderer.clean();
    }

    void input() {
        
    }

    
    unsigned char memory[4096];
    unsigned char V[16]; // Registers
    unsigned short I; // Index register
    unsigned short pc; // Program counter
    unsigned short opcode;

    static const int SCREEN_SIZE = 64 * 32;
    bool screen[SCREEN_SIZE]; // Screen graphics

    unsigned char delay_timer;
    unsigned char sound_timer;

    unsigned short stack[16]; // Current stack
    unsigned short s_ptr; // Stack ptr

    bool draw_flag = false;

    Renderer renderer;
};