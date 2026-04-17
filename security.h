#ifndef SECURITY_H
#define SECURITY_H

#include <string>

class Security {
public:
    Security();
    ~Security();
    std::string encrypt(const std::string& data);
    std::string decrypt(const std::string& data);
};

#endif