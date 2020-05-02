#include <iostream>
#include "magma.h"


class Gamma {
    Key256 key;
    ull init_vec;
public:
    Gamma(Key256 tmp_key, ull i_v) : key(tmp_key), init_vec(i_v) {}

    Data& crypt_function(Data& tmp_txt) {
        ull tmp_vec = init_vec;
        Magma tmp_M(key);
        for (int i = 0; i < tmp_txt.getLenght(); ++i) {
            tmp_vec = tmp_M.crypt(tmp_vec);
            tmp_txt.xor_block(tmp_vec, i);
        }
        return tmp_txt;
    }
};

int main()
{
    std::cout << std::hex;
    Key256 key(0xffeeddccbbaa9988, 0x7766554433221100, 0xf0f1f2f3f4f5f6f7, 0xf8f9fafbfcfdfeff);

    ull iv = 0xff6554f5f6f78f9f;

    std::vector<ull> text = { 0xfedcba9876543210, 0xffeeddfa540a9988, 0x7766fcfdfeff554 };

    Data Test(text, text.size());
    Test.print_txt();

    Gamma mssg(key, iv);
    Test = mssg.crypt_function(Test);
    Test.print_txt();

    Test = mssg.crypt_function(Test);
    Test.print_txt();

    return 0;
}