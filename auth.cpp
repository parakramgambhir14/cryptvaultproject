#include "auth.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <fstream>
#include <limits>
#include <cctype>
#include <vector>
#include <algorithm>
#include "vault.h"
#include "../security/security.h"

std::vector<Entry> Auth::decoyData;

Auth::Auth(Config& cfg) : config(cfg), attempts(0), isRealAccess(false), currentVault(nullptr), 
                          masterPassword("") {
    initDecoyData();
}

Auth::~Auth() {
    if (currentVault) delete currentVault;
}

void Auth::initDecoyData() {
    decoyData = {
        {"Google", "decoy1@gmail.com", "decoy123", "Personal", "2026-01-01 00:00"},
        {"GitHub", "decoyuser", "fakepw", "Work", "2026-01-02 00:00"},
        {"Bank", "fake@bank.com", "bank123", "Finance", "2026-01-03 00:00"}
    };
}

bool Auth::authenticate() {
    std::ifstream masterFile(config.masterFile());
    bool hasMaster = masterFile.good();
    masterFile.close();

    if (!hasMaster) {
        std::string inputPW, confirmPW;
        do {
            std::cout << "[?] Set Master Password: ";
            inputPW = getPasswordMasked();
            updateActivity();

            std::cout << "[?] Confirm Master Password: ";
            confirmPW = getPasswordMasked();
            updateActivity();

            if (inputPW != confirmPW) {
                std::cout << "[ERROR] Passwords do not match. Try again.\n\n";
            }
        } while (inputPW != confirmPW);

        std::ofstream out(config.masterFile());
        out << inputPW;
        out.close();
        std::cout << "[OK] Master Password set. Access granted to REAL VAULT.\n";
        currentVault = new Vault(config.realVaultFile(), true);
        isRealAccess = true;
        masterPassword = inputPW;
        return true;
    }

    std::ifstream file(config.masterFile());
    std::string stored;
    std::getline(file, stored);
    file.close();

    int localAttempts = 0;
    while (localAttempts < config.maxAttempts()) {
        std::cout << "[?] Enter Master Password: ";
        std::string inputPW = getPasswordMasked();
        updateActivity();

        if (inputPW == stored) {
            if (localAttempts == 0) {
                currentVault = new Vault(config.realVaultFile(), true);
                isRealAccess = true;
                std::cout << "[OK] Access to REAL VAULT.\n";
            } else {
                currentVault = new Vault(config.decoyVaultFile(), false);
                isRealAccess = false;
                showDecoyMessage();
            }
            masterPassword = inputPW;
            return true;
        } else {
            localAttempts++;
            std::cout << "[ERROR] Incorrect password. Attempts remaining: " << (config.maxAttempts() - localAttempts) << "\n";
        }
    }
    
    triggerSelfDestruct();
    return false;
}

bool Auth::reauthenticate() {
    logout();
    return authenticate();
}

void Auth::logout() {
    if (currentVault) {
        delete currentVault;
        currentVault = nullptr;
    }
    attempts = 0;
    isRealAccess = false;
    masterPassword = "";
}

void Auth::showDecoyMessage() {
    std::cout << "[INFO] Accessing secondary vault.\n";
}

void Auth::triggerSelfDestruct() {
    std::remove(config.masterFile().c_str());
    std::remove(config.realVaultFile().c_str());
    std::remove(config.decoyVaultFile().c_str());
    
    std::cout << "\n[ALERT] 3 FAILED ATTEMPTS. SECURITY PROTOCOL INITIATED.\n";
    std::cout << "[OK] REAL VAULT PERMANENTLY DESTROYED.\n";
    std::cout << "[*] System locked.\n\n";
}