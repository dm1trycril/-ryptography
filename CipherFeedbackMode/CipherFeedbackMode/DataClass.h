#pragma once
#ifndef DATACLASS_H
#define DATACLAS_H

#include <vector>

typedef unsigned long long ull; // 64
typedef unsigned long ul; //32

class Data {
    std::vector<ull> txt_blocks;
    int lenght;
public:
    Data(const std::vector<ull>& tmp_txt, int l) : lenght(l) { for (ull mem : tmp_txt) { txt_blocks.push_back(mem); } }

    void xor_block(ull iv, int nmbr) { txt_blocks[nmbr] = txt_blocks[nmbr] ^ iv; }

    void print_txt(std::string comment = "") {
        std::cout << "\n" << comment << "\n";
        std::cout << '\n';
        for (ull& mem : txt_blocks) { std::cout << mem << ' '; }
        std::cout << '\n';
    }

    ull& get_block(int nmbr) { return txt_blocks[nmbr]; }

    void set_block(ull txt_prmtr, int nmbr) { txt_blocks[nmbr] = txt_prmtr; }

    int getLenght() { return lenght; }

};

#endif // !DATACLASS_H