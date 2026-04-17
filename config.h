#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <atomic>
#include <ctime>
#include <iostream>
#include <conio.h> // Required for password masking on Windows

// Global trackers
extern std::atomic<time_t> lastActivity;
extern std::atomic<bool> forceLogout;

inline void updateActivity() {
    lastActivity = std::time(nullptr);
}

// Helper to mask passwords with asterisks
inline std::string getPasswordMasked() {
    std::string pw = "";
    char ch;
    while (true) {
        ch = _getch();
        if (ch == '\r' || ch == '\n') { // Enter key
            break;
        } else if (ch == '\b') { // Backspace
            if (!pw.empty()) {
                pw.pop_back();
                std::cout << "\b \b";
            }
        } else if (ch == 3 || ch == 26) { // Handle Ctrl+C / Ctrl+Z gracefully
            std::exit(0);
        } else {
            pw.push_back(ch);
            std::cout << '*';
        }
    }
    std::cout << '\n';
    return pw;
}

class Config {
public:
    Config();   
    ~Config();  

    std::string masterFile() const;
    std::string realVaultFile() const;
    std::string decoyVaultFile() const;
    int maxAttempts() const;
};

#endif