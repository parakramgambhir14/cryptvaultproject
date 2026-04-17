#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include <ctime>
#include <vector>
#include <algorithm>
#include <cctype>
#include <thread>
#include <chrono>
#include <cstdlib>
#include "core/auth.h"
#include "core/vault.h"
#include "utils/config.h"

// Define globals
std::atomic<time_t> lastActivity(std::time(nullptr));
std::atomic<bool> keepRunning(true);
std::atomic<bool> forceLogout(false);

void inactivityTimer() {
    while (keepRunning) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (!keepRunning) break;
        
        if (std::time(nullptr) - lastActivity >= 20) {
            if (!forceLogout) { 
                std::cout << "\n\n[SECURITY ALERT] 20 seconds of inactivity detected.\n";
                std::cout << "Auto-locking vault. Press ENTER to return to login...\n";
                forceLogout = true;
            }
        }
    }
}

std::string getCurrentTime() {
    auto now = std::time(nullptr);
    auto t = std::localtime(&now);
    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", t);
    return std::string(buffer);
}

int getStrength(Vault* v, const std::string& pw) {
    return v ? v->getStrength(pw) : 0;
}

std::string strengthToString(int s) {
    if (s >= 5) return "STRONG";
    if (s >= 3) return "MEDIUM";
    return "WEAK";
}

void printTable(const std::vector<Entry>& entries, bool revealPasswords) {
    std::cout << "\n+----+--------------------------+-----------------+----------------------+\n";
    std::cout << "| #  | Service                  | Username        | Password             |\n";
    std::cout << "+----+--------------------------+-----------------+----------------------+\n";
    for (size_t i = 0; i < entries.size(); ++i) {
        const auto& e = entries[i];
        std::string pwDisplay = revealPasswords ? e.password : "********";
        std::cout << "| " << std::setw(2) << i+1 << " | " 
                  << std::setw(24) << e.service.substr(0,24) << " | "
                  << std::setw(15) << e.username.substr(0,15) << " | "
                  << std::setw(20) << pwDisplay.substr(0,20) << " |\n";
    }
    std::cout << "+----+--------------------------+-----------------+----------------------+\n";
}

bool containsIgnoreCase(const std::string& str, const std::string& term) {
    std::string s = str, t = term;
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    std::transform(t.begin(), t.end(), t.begin(), ::tolower);
    return s.find(t) != std::string::npos;
}

int main() {
    std::thread timer(inactivityTimer);
    timer.detach();

    std::cout << "\n";
    std::cout << "     CRYPTVAULT v3.2 PRO\n";
    std::cout << "     Secure Password Manager\n\n";

    Config config;

    // Outer loop allows returning to login screen instead of closing program
    while (keepRunning) {
        Auth auth(config);
        forceLogout = false;
        updateActivity();

        if (!auth.authenticate()) {
            keepRunning = false; // Program fully exits if self-destruct is triggered
            break;
        }

        Vault* vault = auth.getVault();
        bool authenticated = true;

        while (authenticated && !forceLogout) {
            updateActivity();

            std::cout << "\n=== MAIN MENU ===\n";
            std::cout << "1. View Entries\n";
            std::cout << "2. Add Entry\n";
            std::cout << "3. Search\n";
            std::cout << "4. Edit Entry\n";
            std::cout << "5. Password Strength\n";
            std::cout << "6. Reauth\n";
            std::cout << "0. Exit\n";
            std::cout << "Choice: ";

            // Using getline instead of cin >> choice so hitting ENTER alone unblocks the thread
            std::string inputStr;
            std::getline(std::cin, inputStr);

            // Immediately check if inactivity triggered while we waited for choice input
            if (forceLogout) {
                authenticated = false;
                break;
            }

            int choice = -1;
            if (!inputStr.empty()) {
                try {
                    choice = std::stoi(inputStr);
                } catch (...) {
                    choice = -1;
                }
            } else {
                continue; // User just pressed Enter by mistake during normal use, reprint menu
            }

            updateActivity();
            auto& entries = vault->getEntries();

            switch (choice) {
                case 1: {  
                    printTable(entries, false); 
                    if (!entries.empty()) {
                        std::cout << "\nDo you want to reveal passwords? (y/n): ";
                        std::string ans;
                        std::getline(std::cin, ans);
                        updateActivity();
                        if (forceLogout) break;
                        
                        if (!ans.empty() && tolower(ans[0]) == 'y') {
                            std::cout << "Enter Master Password: ";
                            std::string mp = getPasswordMasked();
                            updateActivity();
                            
                            if (mp == auth.getMasterPassword()) {
                                printTable(entries, true);
                                std::cout << "\nPress ENTER to hide passwords...";
                                std::cin.get();
                                updateActivity();
                                std::cout << "[OK] Passwords hidden and secured.\n";
                            } else {
                                std::cout << "[ERROR] Incorrect Password.\n";
                            }
                        }
                    }
                    break;
                }
                case 2: {  
                    Entry e;
                    std::cout << "Service: "; std::getline(std::cin >> std::ws, e.service); updateActivity();
                    std::cout << "Username: "; std::getline(std::cin, e.username); updateActivity();
                    std::cout << "Password: "; std::getline(std::cin, e.password); updateActivity();
                    std::cout << "Category: "; std::getline(std::cin, e.category); updateActivity();
                    e.timestamp = getCurrentTime();
                    vault->addEntry(e);
                    std::cout << "[OK] Added.\n";
                    break;
                }
                case 3: {  
                    std::string term;
                    std::cout << "Search term: "; std::getline(std::cin >> std::ws, term);
                    updateActivity();
                    std::vector<Entry> results;
                    for (const auto& e : entries) {
                        if (containsIgnoreCase(e.service, term) || containsIgnoreCase(e.username, term)) {
                            results.push_back(e);
                        }
                    }
                    printTable(results, false);
                    break;
                }
                case 4: {  
                    std::cout << "Enter Master Password to edit entries: ";
                    std::string mp = getPasswordMasked();
                    updateActivity();
                    
                    if (mp != auth.getMasterPassword()) {
                        std::cout << "[ERROR] Incorrect Password.\n";
                        break;
                    }

                    printTable(entries, false);
                    size_t idx;
                    std::cout << "Entry #: "; 
                    std::string idxStr;
                    std::getline(std::cin, idxStr);
                    try { idx = std::stoul(idxStr); } catch (...) { idx = 0; }
                    updateActivity();
                    
                    if (idx == 0 || idx > entries.size()) {
                        std::cout << "[ERROR] Invalid.\n";
                        break;
                    }
                    idx--;
                    Entry newe = entries[idx];
                    std::cout << "New Service (" << newe.service << "): "; std::getline(std::cin, newe.service); updateActivity();
                    std::cout << "New Username (" << newe.username << "): "; std::getline(std::cin, newe.username); updateActivity();
                    std::cout << "New Password (" << newe.password << "): "; std::getline(std::cin, newe.password); updateActivity();
                    std::cout << "New Category (" << newe.category << "): "; std::getline(std::cin, newe.category); updateActivity();
                    newe.timestamp = getCurrentTime();
                    vault->updateEntry(idx, newe);
                    std::cout << "[OK] Updated.\n";
                    break;
                }
                case 5: {  
                    std::string pw;
                    std::cout << "Password: "; std::getline(std::cin >> std::ws, pw);
                    updateActivity();
                    int s = getStrength(vault, pw);
                    std::cout << "Score: " << s << " (" << strengthToString(s) << ")\n";
                    break;
                }
                case 6: {  
                    if (auth.reauthenticate()) {
                        vault = auth.getVault();
                        std::cout << "[OK] Reauthenticated.\n";
                    } else {
                        authenticated = false;
                        keepRunning = false;
                    }
                    updateActivity();
                    break;
                }
                case 0:
                    std::cout << "[BYE] Secure exit.\n";
                    authenticated = false;
                    keepRunning = false;
                    break;
                default:
                    std::cout << "\n[ERROR] Invalid choice! Select 0-6.\n";
                    break;
            }
            
            if (authenticated && !forceLogout) {
                std::cout << "\nPress ENTER to continue...";
                std::cin.get();
                updateActivity();
            }
        }
        
        auth.logout();
        if (forceLogout) {
            std::cout << "\n--- RETURNED TO SECURE LOGIN ---\n";
        }
    }
    
    return 0;
}