
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

    A5(std::vector<byte>& k, word f) : key(k), frame_value(f) {}

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
        sum >= 2 ? 1 : 0;
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

    bit encrypt(bit b){

    }
};

int main()
{
    std::cout << std::hex;
    return 0;
}