#ifndef AUTH_H
#define AUTH_H

#include <string>
#include <vector>
#include "entry.h"
#include "../utils/config.h"

class Vault;

class Auth {
private:
    Config& config;
    Vault* currentVault;
    int attempts;
    bool isRealAccess;
    std::string masterPassword;
    static std::vector<Entry> decoyData;

    bool checkSpecialCondition();
    void initDecoyData();
    void showDecoyMessage();
    void triggerSelfDestruct();

public:
    Auth(Config& cfg);
    ~Auth();
    bool authenticate();
    bool reauthenticate();
    void setRealAccess(bool real) { isRealAccess = real; }
    bool getRealAccess() const { return isRealAccess; }
    Vault* getVault() { return currentVault; }
    bool isAuthenticated() const { return currentVault != nullptr; }
    void logout();
    
    // Retrieves the password used for the CURRENT session (works for both Real and Decoy)
    std::string getMasterPassword() const { return masterPassword; } 
};

#endif