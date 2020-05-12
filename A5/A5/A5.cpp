
#include <iostream>
#include <vector>

typedef unsigned char byte;
typedef unsigned long word;
typedef word bit;

#define R1MASK  0x07FFFF /* 19 bits, numbered 0..18 */
#define R2MASK  0x3FFFFF /* 22 bits, numbered 0..21 */
#define R3MASK  0x7FFFFF /* 23 bits, numbered 0..22 */

#define R1MID   0x000100 /* bit 8 */
#define R2MID   0x000400 /* bit 10 */
#define R3MID   0x000400 /* bit 10 */

#define R1TAPS  0x072000 /* bits 18,17,16,13 */
#define R2TAPS  0x300000 /* bits 21,20 */
#define R3TAPS  0x700080 /* bits 22,21,20,7 */

#define R1OUT   0x040000 /* bit 18 (the high bit) */
#define R2OUT   0x200000 /* bit 21 (the high bit) */
#define R3OUT   0x400000 /* bit 22 (the high bit) */

class A5 {
    std::vector<byte> key;
    //std::vector<byte> mssg;
    word R1, R2, R3;
    word frame_value;
public:

    A5(std::vector<byte>& k, word f) : key(k), frame_value(f) {
        InitReg();
    }

    void InitReg() {
        R1 = R2 = R3 = 0;
        bit keyb, frameb;
        for (int i = 0; i < 64; ++i) {
            syncAll();
            keyb = (key[i / 8] >> (i & 7)) & 1;
            R1 ^= keyb;
            R2 ^= keyb;
            R3 ^= keyb;
        }
        for (int i = 0; i < 22; ++i) {
            syncAll();
            frameb = (frame_value >> i) & 1;
            R1 ^= frameb; 
            R2 ^= frameb; 
            R3 ^= frameb;
        }
        for (int i = 0; i < 100; ++i) {
            sync();
        }
    }

    bit parity(word x) {
        int y{ 0 };
        for (int i = 0; i < 32; ++i) {
            y += (x & 1);
            x >>= 1;
        }
        /*x ^= x >> 16;
        x ^= x >> 8;
        x ^= x >> 4;
        x ^= x >> 2;
        x ^= x >> 1;*/
        return (y + 1) % 2;
    }


    bit majority() {
        //int sum;
        int sum = parity(R1 & R1MID) + parity(R2 & R2MID) + parity(R3 & R3MID);
        return (sum >= 2 ? 1 : 0);
        /*if (sum >= 2)
            return 1;
        else
            return 0;*/
    }

    void sync() {
        bit m = majority();
        if (((R1 & R1MID) != 0) == m) R1 = syncOne(R1, R1MASK, R1TAPS);
        if (((R2 & R2MID) != 0) == m) R2 = syncOne(R2, R2MASK, R2TAPS);
        if (((R3 & R3MID) != 0) == m) R3 = syncOne(R3, R3MASK, R3TAPS);
    }
    word syncOne(word reg, word mask, word taps) {
        word temp = reg & taps;
        reg = ((reg << 1) & mask) | parity(temp);
        return reg;
    }

    void syncAll() {
        R1 = syncOne(R1, R1MASK, R1TAPS);
        R2 = syncOne(R2, R2MASK, R2TAPS);
        R3 = syncOne(R3, R3MASK, R3TAPS);
    }

    bit getbit() {
        return parity(R1 & R1OUT) ^ parity(R2 & R2OUT) ^ parity(R3 & R3OUT);
    }

    void cipherSequence(std::vector<byte>& AB, std::vector<byte>& BA){
        for (int i = 0; i <= 113 / 8; i++)
            AB[i] = BA[i] = 0;
        for (int i = 0; i < 114; i++) {
            sync();
            AB[i / 8] |= getbit() << (7 - (i & 7));
        }
        for (int i = 0; i < 114; i++) {
            sync();
            BA[i / 8] |= getbit() << (7 - (i & 7)); 
        }
    }

    void crypt(std::vector<byte>& AB, std::vector<byte>& mssg, std::vector<byte>& cipherMssg){
        for (int i = 0; i < 8; ++i) {
            cipherMssg[i] = 0;
        }
        for (int i = 0; i < 64; ++i) {
            unsigned long long mssgMask = (mssg[i%8] >> (i%8)) & 1;
            unsigned long long abMask = (AB[i / 8] >> i) & 1;
            mssgMask = mssgMask ^ abMask;
            cipherMssg[i % 8] = cipherMssg[i % 8] | (mssgMask << (i%8));
        }
    }
};

void print_message(std::vector<byte>& link) {
    for (unsigned int(element) : link) {
        std::cout << element << " ";
    }
    std::cout << std::endl;
}

int main()
{
    std::cout << std::hex;
    std::vector<byte> key{ 0x12, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };
    std::vector<byte> message{ 0x53, 0x4E, 0xAA, 0x58, 0x2F, 0xE8, 0x15, 0x1A };
    std::vector<byte> cipherMessage{ 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<byte> AtoB(15), BtoA(15);
    word frame = 0x134;

    A5 test(key, frame);

    print_message(message);

    test.cipherSequence(AtoB, BtoA);

    test.crypt(AtoB, message, cipherMessage);
    print_message(cipherMessage);

    test.cipherSequence(AtoB, BtoA);

    test.crypt(AtoB, cipherMessage, message);
    print_message(message);


    return 0;
}