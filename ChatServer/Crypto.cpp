//
// Created by Beyond on 2025/1/19.
//

#include "Crypto.h"
std::string Crypto::stringToSha256(const std::string str) {
    EVP_MD_CTX* context = EVP_MD_CTX_new();//创建上下文
    const EVP_MD* md = EVP_sha256();//使用sha256加密
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;

    EVP_DigestInit_ex(context, md, nullptr);//初始化
    EVP_DigestUpdate(context, str.c_str(), str.size());//更新
    EVP_DigestFinal_ex(context, hash, &lengthOfHash);//结果存储到hash中
    EVP_MD_CTX_free(context);//释放上下文

    std::stringstream ss;//转换为字符串
    for (unsigned int i = 0; i < lengthOfHash; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}
