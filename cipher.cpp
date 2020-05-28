#include "cipher.h"
// https://www.thecrazyprogrammer.com/2017/08/vigenere-cipher-c-c.html
std::string cipher_encrypt(std::string message, std::string key) {
    if (key.length() <= 0) return message;
    int msgLen = message.length(), keyLen = key.length(), i, j = 0;
    for(i = 0; i < msgLen; ++i) {
        if (j >= keyLen) j = 0;
        message[i] = (message[i] + key[j]);
    }
    return message;
}

std::string cipher_decrypt(std::string message, std::string key) {
    int msgLen = message.length(), keyLen = key.length(), i, j = 0;
    for(i = 0; i < msgLen; ++i) {
        if (j >= keyLen) j = 0;
        message[i] = message[i] - key[j];
    }
    return message;
}
