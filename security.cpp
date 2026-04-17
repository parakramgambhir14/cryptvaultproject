#include "security.h"
#include <cstring>

Security::Security() {}

Security::~Security() {}

std::string Security::encrypt(const std::string& data) {
    std::string enc = data;
    for (char& c : enc) c ^= 0xAA;
    return enc;
}

std::string Security::decrypt(const std::string& data) {
    return encrypt(data);
}