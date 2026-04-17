#include "vault.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <limits>
#include "security/security.h"

Vault::Vault(const std::string& fname, bool realVault) 
    : filename(fname), isRealVault(realVault), security(new Security()) {
    entries.clear();
    loadEntries();
}

Vault::~Vault() {
    delete security;
}

void Vault::loadEntries() {
    std::ifstream file(filename);
    if (!file.is_open()) return;
    std::string data;
    std::string line;
    while (std::getline(file, line)) data += line;
    data = security->decrypt(data);
    std::istringstream iss(data);
    std::string row;
    while (std::getline(iss, row, '\n')) {
        if (row.empty()) continue;
        std::istringstream rowiss(row);
        Entry e;
        std::getline(rowiss, e.service, '|');
        std::getline(rowiss, e.username, '|');
        std::getline(rowiss, e.password, '|');
        std::getline(rowiss, e.category, '|');
        std::getline(rowiss, e.timestamp);
        entries.push_back(e);
    }
}

void Vault::saveEntries() {
    std::ostringstream oss;
    for (const auto& e : entries) {
        oss << e.service << "|" << e.username << "|" << e.password << "|" 
            << e.category << "|" << e.timestamp << "\n";
    }
    std::string data = security->encrypt(oss.str());
    std::ofstream file(filename);
    file << data;
}

void Vault::addEntry(const Entry& e) {
    entries.push_back(e);
    saveEntries();
}

void Vault::removeEntry(size_t idx) {
    if (idx < entries.size()) {
        entries.erase(entries.begin() + idx);
        saveEntries();
    }
}

void Vault::updateEntry(size_t idx, const Entry& e) {
    if (idx < entries.size()) {
        entries[idx] = e;
        saveEntries();
    }
}

int Vault::getStrength(const std::string& pw) {
    int score = 0;
    if (pw.length() >= 8) score += 2;
    if (pw.length() >= 12) score += 2;
    
    bool hasLower = false, hasUpper = false, hasDigit = false, hasSpecial = false;
    for (char c : pw) {
        if (std::islower(c)) hasLower = true;
        else if (std::isupper(c)) hasUpper = true;
        else if (std::isdigit(c)) hasDigit = true;
        else hasSpecial = true;
    }
    
    if (hasLower && hasUpper) score += 2;
    if (hasDigit) score += 1;
    if (hasSpecial) score += 2;
    
    return score;
}