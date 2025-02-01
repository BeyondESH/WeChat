//
// Created by Beyond on 2025/1/19.
//

#ifndef CRYPTO_H
#define CRYPTO_H
#include <openssl/evp.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
class Crypto {
public:
    static std::string stringToSha256(const std::string);
};



#endif //CRYPTO_H
