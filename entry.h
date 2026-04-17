#ifndef ENTRY_H
#define ENTRY_H

#include <string>

struct Entry {
    std::string service;
    std::string username;
    std::string password;
    std::string category;
    std::string timestamp;
};

#endif