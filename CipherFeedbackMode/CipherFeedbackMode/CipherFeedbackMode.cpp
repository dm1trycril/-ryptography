#include <iostream>
#include "magma.h"
#include "DataClass.h"

class Feedback {
    Key256 key;
    ull init_vec;
public:
    Feedback(Key256 tmp_key, ull i_v) : key(tmp_key), init_vec(i_v) {}

    Data& encrypt(Data& tmp_txt) {
        ull tmp_vec = init_vec;
        Magma tmp_M(key);
        for (int i = 0; i < tmp_txt.getLenght(); ++i) {
            tmp_vec = tmp_M.crypt(tmp_vec);
            tmp_txt.xor_block(tmp_vec, i);
            tmp_vec = tmp_txt.get_block(i);
        }
        return tmp_txt;
    }

    Data& decrypt(Data& text_prmtr) {
        ull tmp_init_vec = init_vec;
        Magma var_M(key);
        ull tmp;
        for (int i = 0; i < text_prmtr.getLenght(); ++i) {
            tmp_init_vec = var_M.crypt(tmp_init_vec);
            txt_swap(text_prmtr.get_block(i), tmp_init_vec);
            text_prmtr.xor_block(tmp_init_vec, i);
        }
        return text_prmtr;
    }
};

int main()
{
    std::cout << std::hex;
    Key256 key(0xcbbaa9988ffeeddc, 0x7761100655443322, 0xf0f5f6ff4f71f2f3, 0xfbfcf9dfefaf8fff);

    ull iv = 0xff6554f5f6f78f9f;

    std::vector<ull> text = { 0xfe10dcba98765432, 0xddfa54f88fee0a99, 0x7ff554766fcfdfe };

    Data Test(text, text.size());
    Test.print_txt("Text before encryption");

    Feedback mssg(key, iv);
    Test = mssg.encrypt(Test);
    Test.print_txt("Text after encryption");

    Test = mssg.decrypt(Test);
    Test.print_txt("Text after decryption");
    return 0;
}