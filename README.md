🔐 CryptVault
Advanced Console-Based Credential Management

Secure local storage, active memory lifecycle management, and proactive threat mitigation.

✨ Key Features
CryptVault goes beyond simple password storage by implementing active defense mechanisms against memory scraping, shoulder surfing, and brute-force attacks.
🛡️ Master Password Authentication: Secures all stored credentials behind a master password validated against a localized configuration state.
⏱️ Inactivity Auto-Lock: Utilizes asynchronous multithreading (std::thread) to continuously monitor user activity. If no input is detected for 20 seconds, the vault automatically locks and purges the decrypted memory state.
🎭 Decoy Vault Mechanism: A dual-vault system protects against forced disclosure. Subsequent failed login attempts seamlessly load a "Decoy Vault" containing pre-initialized fake credentials, hiding the real data.
💥 Self-Destruct Protocol: A safeguard against brute-force attacks. After 3 consecutive failed attempts, the system automatically deletes the master configuration, real vault, and decoy files from the local disk.
🔒 Data Encryption: The Security module obfuscates the serialized vault data using an XOR-based symmetric cipher before persisting it to the filesystem.
👁️‍🗨️ Console Input Masking: Passwords entered into the terminal are safely masked with asterisks (*) in real-time to prevent shoulder surfing.
💪 Strength Evaluator: Built-in utility to analyze and score the entropy and strength of user-provided passwords.

🏗️ Project Architecture
The codebase enforces strict separation of concerns and maintainable Object-Oriented design:

Plaintext
📦 CryptVault
 ┣ 📂 core/
 ┃ ┣ 📜 auth.cpp/h      # Login state, decoy triggers, self-destruct logic
 ┃ ┣ 📜 vault.cpp/h     # Active memory state management & file serialization
 ┃ ┗ 📜 entry.h         # Core data structure (Service, User, Pass, Category, Time)
 ┣ 📂 security/
 ┃ ┗ 📜 security.cpp/h  # Symmetric encryption/decryption (XOR)
 ┣ 📂 utils/
 ┃ ┗ 📜 config.cpp/h    # File path configs & cross-platform password masking
 ┣ 📂 data/             # Auto-generated storage (master.txt, vault.txt, decoy.txt)
 ┗ 📜 main.cpp          # App loop, UI, and detached inactivity timer thread
🧠 Memory & State Management
Security First: CryptVault actively manages memory to prevent sensitive data leakage.

The Vault instance is dynamically allocated on the heap only upon successful authentication. When a user logs out manually or is forced out by the multithreaded inactivity timer, the active vault pointer is explicitly destroyed (delete currentVault). This guarantees that no decrypted credentials linger in active RAM.

🚀 Compilation & Execution
Prerequisites
A C++11 (or higher) compliant compiler (e.g., GCC, MSVC, Clang).
Windows OS (required for <conio.h> console input masking).

Build Instructions
Ensure you are in the root directory of the project, then compile using standard g++:

Bash
g++ main.cpp core/auth.cpp core/vault.cpp utils/config.cpp security/security.cpp -o cryptvault.exe -std=c++11 -pthread
Usage
Launch the compiled executable from your terminal:

Bash
./cryptvault.exe
Follow the interactive on-screen prompts to initialize your master password, populate your vault, and evaluate credential strength
