#ifndef VAULT_H
#define VAULT_H

#include <string>
#include <vector>
#include "entry.h"
#include "security/security.h"

class Vault {
private:
    std::string filename;
    bool isRealVault;
    Security* security;
    std::vector<Entry> entries;

    void loadEntries();
    void saveEntries();

public:
    Vault(const std::string& fname, bool realVault);
    ~Vault();
    std::vector<Entry>& getEntries() { return entries; }
    void addEntry(const Entry& e);
    void removeEntry(size_t idx);
    void updateEntry(size_t idx, const Entry& e);
    int getStrength(const std::string& pw);
};

#endif