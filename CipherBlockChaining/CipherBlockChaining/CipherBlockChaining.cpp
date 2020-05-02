#include <iostream>
#include "magma.h"
#include "DataClass.h"

class Hook {
    Key256 key;
    ull init_vec;
public:
    Hook(Key256 key_prmt, ull init_vec_prmtr) : key(key_prmt), init_vec(init_vec_prmtr) {}

    Data& encrypt(Data& text_prmtr) {
        ull tmp_init_vec = init_vec;
        Magma var_M(key);
        for (int i = 0; i < text_prmtr.getLenght(); ++i) {
            text_prmtr.xor_block(tmp_init_vec, i);
            text_prmtr.set_block(var_M.encrypt(text_prmtr.get_block(i)), i);
            tmp_init_vec = text_prmtr.get_block(i);
        }
        return text_prmtr;
    }

    Data& decrypt(Data& text_prmtr) {
        ull tmp_init_vec = init_vec;
        Magma var_M(key);
        ull tmp;
        for (int i = 0; i < text_prmtr.getLenght(); ++i) {
            //ull temp = text_prmtr.get_block(i);
            text_prmtr.set_block(var_M.decrypt(text_prmtr.get_block(i)), i);
            txt_swap(text_prmtr.get_block(i), tmp_init_vec);
            text_prmtr.xor_block(tmp_init_vec, i);
            //tmp_init_vec = temp;
        }
        return text_prmtr;
    }
 };

int main(){
    std::cout << std::hex;
    Key256 key(0xcbbaa9988ffeeddc, 0x7761100655443322, 0xf0f5f6ff4f71f2f3, 0xfbfcf9dfefaf8fff);

    ull iv = 0xff6554f5f6f78f9f;

    std::vector<ull> text = { 0xfe10dcba98765432, 0xddfa54f88fee0a99, 0x7ff554766fcfdfe };

    Data Test(text, text.size());
    Test.print_txt("Text before encryption");

    Hook mssg(key, iv);
    Test = mssg.encrypt(Test);
    Test.print_txt("Text after encryption");

    Test = mssg.decrypt(Test);
    Test.print_txt("Text after decryption");
    return 0;
}
