//
// Created by Beyond on 2025/1/19.
//

#include "Crypto.h"
#include <iomanip>
#include <sstream>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <iostream>

/**
 * 密码学工具的实现，用于安全认证
 */

std::string Crypto::stringToSha256(const std::string &str)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];
    EVP_MD_CTX *context = EVP_MD_CTX_new();

    if (!context)
    {
        std::cerr << "创建EVP_MD_CTX失败" << std::endl;
        return "";
    }

    if (!EVP_DigestInit_ex(context, EVP_sha256(), nullptr))
    {
        std::cerr << "初始化摘要上下文失败" << std::endl;
        EVP_MD_CTX_free(context);
        return "";
    }

    if (!EVP_DigestUpdate(context, str.c_str(), str.length()))
    {
        std::cerr << "更新摘要失败" << std::endl;
        EVP_MD_CTX_free(context);
        return "";
    }

    if (!EVP_DigestFinal_ex(context, hash, nullptr))
    {
        std::cerr << "完成摘要失败" << std::endl;
        EVP_MD_CTX_free(context);
        return "";
    }

    EVP_MD_CTX_free(context);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return ss.str();
}

std::string Crypto::stringToMd5(const std::string &str)
{
    unsigned char hash[MD5_DIGEST_LENGTH];
    EVP_MD_CTX *context = EVP_MD_CTX_new();

    if (!context)
    {
        std::cerr << "创建EVP_MD_CTX失败" << std::endl;
        return "";
    }

    if (!EVP_DigestInit_ex(context, EVP_md5(), nullptr))
    {
        std::cerr << "初始化摘要上下文失败" << std::endl;
        EVP_MD_CTX_free(context);
        return "";
    }

    if (!EVP_DigestUpdate(context, str.c_str(), str.length()))
    {
        std::cerr << "更新摘要失败" << std::endl;
        EVP_MD_CTX_free(context);
        return "";
    }

    if (!EVP_DigestFinal_ex(context, hash, nullptr))
    {
        std::cerr << "完成摘要失败" << std::endl;
        EVP_MD_CTX_free(context);
        return "";
    }

    EVP_MD_CTX_free(context);

    std::stringstream ss;
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return ss.str();
}

std::string Crypto::generateRandomString(size_t length)
{
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    std::string result;
    result.reserve(length);

    unsigned char buf[length];
    if (RAND_bytes(buf, length) != 1)
    {
        std::cerr << "生成随机字节失败" << std::endl;
        // 回退到安全性较低的方法
        for (size_t i = 0; i < length; ++i)
        {
            result += alphanum[rand() % (sizeof(alphanum) - 1)];
        }
        return result;
    }

    for (size_t i = 0; i < length; ++i)
    {
        result += alphanum[buf[i] % (sizeof(alphanum) - 1)];
    }

    return result;
}

std::string Crypto::generateVerificationCode(size_t length)
{
    static const char digits[] = "0123456789";

    std::string result;
    result.reserve(length);

    unsigned char buf[length];
    if (RAND_bytes(buf, length) != 1)
    {
        std::cerr << "为验证码生成随机字节失败" << std::endl;
        // 回退到安全性较低的方法
        for (size_t i = 0; i < length; ++i)
        {
            result += digits[rand() % (sizeof(digits) - 1)];
        }
        return result;
    }

    for (size_t i = 0; i < length; ++i)
    {
        result += digits[buf[i] % (sizeof(digits) - 1)];
    }

    return result;
}
