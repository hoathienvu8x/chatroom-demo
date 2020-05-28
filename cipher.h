#ifndef CIPHER_H
#define CIPHER_H

#include <string>

std::string cipher_encrypt(std::string message, std::string key);
std::string cipher_decrypt(std::string message, std::string key);

#endif /* CIPHER_H */
