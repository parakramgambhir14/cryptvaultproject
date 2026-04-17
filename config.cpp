#include "config.h"

Config::Config() {
    // Initialization if needed
}

Config::~Config() {
    // Cleanup if needed
}

std::string Config::masterFile() const {
    return "data/master.txt";
}

std::string Config::realVaultFile() const {
    return "data/vault.txt";
}

std::string Config::decoyVaultFile() const {
    return "data/decoy.txt";
}

int Config::maxAttempts() const {
    return 3; // Number of tries before self-destruct
}