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
                }
            break;
            case 0x1000:
            jp();
            break;
            case 0xD000:
            draw_sprite();
            break;
            case 0xA000:
            ld_i(opcode);
            break;
            case 0x3000:
            se_vx();
            break;
            case 0x6000:
            ld_vx(opcode);
            break;
            case 0x7000:
            add_vx();
            break;
            default:
            std::cout << "end" << std::endl;
            break;
        }
    }

    void jp() {
        pc = opcode & 0x0FFF;
    }

    void ld_i(short opcode) {
        I = (opcode & 0x0FFF);
        pc+=2;
    }

    void ld_vx(short opcode) {
        V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
        pc+=2;

    }

    void add_vx() {
        V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
        pc += 2;
    }

    void se_vx() {
        if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
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
        V[0xF] = 0;

        // Decode opcode
        short x = V[(opcode & 0x0F00) >> 8];
        short y = V[(opcode & 0x00F0) >> 4];
        short h = opcode & 0x000F;

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
                    V[0xF] = 1;

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

    void draw() {

    }

    void render() {

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

    unsigned short stack; // Current stack
    unsigned short s_ptr; // Stack ptr

    bool draw_flag = false;

    Renderer renderer;
};