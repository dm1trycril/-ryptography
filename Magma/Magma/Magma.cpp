#include <iostream>
#include <cmath>
using namespace std;

typedef unsigned long long ullong;  //64b
typedef unsigned long ulong;        //32b

class int256 {
    ullong bits[4];
public:
    int256(ullong bit0, ullong bit1, ullong bit2, ullong bit3) : bits{ bit0, bit1, bit2, bit3 } {}
    ullong get_bit(int i) { return bits[i]; }
};

class Magma {
    int256 key;
    ullong mod32 = 0xffffffff;
    ulong split_key[8];//ключ разбитый на 8 частей
    ulong round_key[32];//раудовые ключи(выбраные из основного ключа)
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
    void setsplit_key() {
        for (int i = 0; i < 4; ++i) {
            ullong Lpart = key.get_bit(i);
            ullong Rpart = Lpart & mod32;
            Lpart >>= 32;
            split_key[i * 2] = Lpart;
            split_key[i * 2 + 1] = Rpart;
        }
    }
    ulong replace_by_substitution(ullong temp_Rpart, ullong temp_round_key, int pi) {
        temp_Rpart += temp_round_key;//правая часть и раундовый ключ складываются
        temp_Rpart &= mod32;//по модулю 32
        int un[8];
        for (int i = 0; i < 8; ++i) {//разбиваем на 8 частей
            temp_round_key = temp_Rpart & 0xf;//в х записываем 4 правых(младших) бита
            un[8 - i - 1] = temp_round_key;//записываем справа налево
            temp_Rpart >>= 4;//сдвигаем, чтобы можно было применить маску дальше
        }
        for (int i = 0; i < 8; ++i) { un[i] = substitution_block[i][un[i]]; }//преобразуем каждое 4битное число в соотвествии с таблицой
        for (int i = 0; i < 8; ++i) {//восстанавливаем преобразованная правая часть
            temp_Rpart += un[i];
            temp_Rpart <<= 4;
        }
        temp_Rpart >>= 4;//убирает лишний сдвиг
        temp_Rpart = (temp_Rpart << 11) | (temp_Rpart >> 21);// делается циклический сдвиг на 11 влево
        temp_Rpart &= mod32;//уюираем лишние левые 11 бит
        return temp_Rpart;
    }
    ullong round(ullong& Lpart, ullong& Rpart) {//раунды
        for (int i = 0; i < 31; ++i) {//31 преобразование с раундовыми ключами
            std::swap(Rpart, Lpart);
            Rpart = Rpart ^ replace_by_substitution(Lpart, round_key[i], i);
        }
        //last round, 32
        Lpart = Lpart ^ replace_by_substitution(Rpart, round_key[31], 31);
        Lpart <<= 32;
        Lpart += Rpart;
        return Lpart;
    }
    void setXkey() {//функция создаёт последовательность раундовых ключей
        for (int i = 0; i < 24; ++i) { round_key[i] = split_key[i % 8]; }
        for (int i = 7; i >= 0; --i) { round_key[32 - i - 1] = split_key[i]; }
    }
public:
    Magma(int256 key) : key{ key } {
        setsplit_key();
    }
    ullong encrypt(ullong data) {//блок сообщения(64) разбивается на две части (0...0 левая часть), (0...0 правая часть) и шифруется с помощью раундовых ключей
        ullong Lpart = data;
        ullong Rpart = Lpart & mod32;
        Lpart >>= 32;
        setXkey();//0..7 0...7 0...7 7...0
        data = round(Lpart, Rpart);//шифруем с помощью раундов
        return data;
    }
    ullong decrypt(ullong data) {//блок сообщения(64) разбивается на две части (0...0 левая часть), (0...0 правая часть) и шифруется с помощью раундовых ключей
        ullong Lpart = data;
        ullong Rpart = Lpart & mod32;
        Lpart >>= 32;
        //reverse xkey
        ulong* tmp = round_key;//0...7 7...0 7...0 7...0
        for (int i = 8; i < 16; ++i) { std::swap(round_key[i], tmp[32 - i - 1]); }
        data = round(Lpart, Rpart);//расшифровываем с помощью раундов
        return data;
    }
    int256 getKey() { return key; }
};

int main() {
    cout << hex;
    int256 key(0xffeeddccbbaa9988, 0x7766554433221100, 0xf0f1f2f3f4f5f6f7, 0xf8f9fafbfcfdfeff);
    Magma a(key);
    ullong text = 0xfedcba9876543210;
    ullong message = a.encrypt(text);
    cout << "\ntext before encryption " << text << '\n';
    cout << "\ntext after encryption " << message << '\n';
    message = a.decrypt(message);
    cout << "\ntext before decryption " << message << '\n';
    return 0;
}