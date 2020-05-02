#pragma once
#ifndef DATACLASS_H
#define DATACLASS_H

#include <cstring>
#include <vector>

typedef unsigned long long ull; // 64
typedef unsigned long ul; //32

class Data {
    std::vector<ull> text_blocks;
    int txt_lenght;
public:
    Data(const std::vector<ull>& text_prmtr, int lenght_prmtr) : text_blocks(text_prmtr), txt_lenght(lenght_prmtr) {}

    void xor_block(ull iv, int nmbr) { text_blocks[nmbr] = text_blocks[nmbr] ^ iv; }

    int getLenght() { return txt_lenght; }

    ull& get_block(int nmbr) { return text_blocks[nmbr]; }

    void set_block(ull txt_prmtr, int nmbr) { text_blocks[nmbr] = txt_prmtr; }

    void print_txt(std::string comment = "") {
        std::cout << "\n" << comment << "\n";
        std::cout << '\n';
        for (ull& mem : text_blocks) { std::cout << mem << ' '; }
        std::cout << '\n';
    }
};

#endif // !DATACLASS_H


