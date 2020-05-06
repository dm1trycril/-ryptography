#pragma once
#ifndef MAGMA_H
#define MAGMA_H
typedef unsigned long long ull; // 64
typedef unsigned long ul; //32

/* 0xffffffff  32 */

class Key256 {
    ull whole_key[4];
    ul key_part[8];
public:
    Key256() {}
    Key256(ull p1, ull p2, ull p3, ull p4) : whole_key{ p1, p2, p3, p4 } {
        for (int i = 0; i < 4; ++i) {
            ull L_part = whole_key[i];
            ull R_part = whole_key[i] & (0xffffffff);
            L_part >>= 32;
            key_part[i * 2] = L_part;
            key_part[i * 2 + 1] = R_part;
        }
    }
    ul get_part(int i) { return key_part[i]; }
};

class Magma {

    //Key256 keytemp;
    ul round_key[32];

    const int substitution_block[8][16] = {
        {1, 7, 14, 13, 0, 5, 8, 3, 4, 15, 10, 6, 9, 12, 11, 2},
        {8, 14, 2, 5, 6, 9, 1, 12, 15, 4, 11, 0, 13, 10, 3, 7},
        {5, 13, 15, 6, 9, 2, 12, 10, 11, 7, 8, 1, 4, 3, 14, 0},
        {7, 15, 5, 10, 8, 1, 6, 13, 0, 9, 3, 14, 11, 4, 2, 12},
        {12, 8, 2, 1, 13, 4, 15, 6, 7, 0, 10, 5, 3, 14, 9, 11},
        {11, 3, 5, 8, 2, 15, 10, 13, 14, 1, 7, 4, 12, 9, 6, 0},
        {6, 8, 2, 3, 9, 10, 5, 12, 1, 14, 4, 7, 11, 13, 0, 15},
        {12, 4, 6, 2, 10, 5, 11, 9, 14, 8, 13, 7, 0, 3, 15, 1},
    };

    ul replace_by_substitution(ull temp_Rpart, ull temp_round_key, int pi) {
        temp_Rpart += temp_round_key;    
        temp_Rpart &= 0xffffffff;
        int un[8];
        for (int i = 0; i < 8; ++i) {
            temp_round_key = temp_Rpart & 0xf;
            un[8 - i - 1] = temp_round_key;
            temp_Rpart >>= 4;      
        }
        for (int i = 0; i < 8; ++i) { un[i] = substitution_block[i][un[i]]; }   
        for (int i = 0; i < 8; ++i) {  
            temp_Rpart += un[i];
            if (i < 7) temp_Rpart <<= 4;
        }
        temp_Rpart = ((temp_Rpart << 11) | (temp_Rpart >> 21)) & (0xffffffff);
        return temp_Rpart;
    }
    ull round(ull& L_part, ull& R_part) {
        for (int i = 0; i < 31; ++i) {  
            std::swap(R_part, L_part);
            R_part = R_part ^ replace_by_substitution(L_part, round_key[i], i);
        }
        //last round, 32
        L_part = L_part ^ replace_by_substitution(R_part, round_key[31], 31);
        L_part <<= 32;
        L_part += R_part;
        return L_part;
    }

public:
    Magma() {}
    Magma(Key256 temp) {
        for (int i = 0; i < 24; ++i) { round_key[i] = temp.get_part(i); }
        for (int i = 7; i >= 0; --i) { round_key[32 - i - 1] = temp.get_part(i); }
    }

    ull crypt(ull data) {  
        ull Lpart = data;
        ull Rpart = Lpart & (0xffffffff);
        Lpart >>= 32;
        data = round(Lpart, Rpart); 
        return data;
    }
    /* ull decrypt(ull data) {// (64)     (0...0  ), (0...0  )      
         ull L_part = data;
         ull R_part = L_part & (0xffffffff);
         L_part >>= 32;
         //reverse round keys
         ul* tmp = round_key;//0...7 7...0 7...0 7...0
         for (int i = 8; i < 16; ++i) { std::swap(round_key[i], tmp[32 - i - 1]); }
         data = round(L_part, R_part);//   
         return data;
     }*/
};
#endif // MAGMA_H