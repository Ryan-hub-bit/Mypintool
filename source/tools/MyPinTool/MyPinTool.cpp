// #include <pin.H>
// #include <iostream>
// #include <fstream>
// #include <unordered_map>
// #include <unordered_set>
// #include <mutex>
// #include <string>

// // Output file for logging indirect jumps
// std::ofstream OutputFile;

// // Mutex for thread-safe access to shared data
// std::mutex IndirectJumpMutex;

// // Global variables to store main executable and `.text` section information
// ADDRINT mainImageBase = 0;
// ADDRINT mainImageHigh = 0;
// ADDRINT textSectionBase = 0;
// ADDRINT textSectionHigh = 0;
// std::string mainImageName;

// // Structure to track jump information
// struct JumpInfo {
//     ADDRINT address;                         // Address of the jump instruction
//     std::string routine;                     // Routine name containing the jump
//     std::unordered_set<ADDRINT> targets;     // Set of unique jump target addresses
//     std::string insText;                     // Full instruction text
//     int count;                               // Number of times this jump was executed
// };

// // Hash map to store unique jumps
// std::unordered_map<ADDRINT, JumpInfo> IndirectJumps;

// // Simplified indirect jump detection
// BOOL IsIndirectJump(INS ins) {
//     return INS_IsIndirectControlFlow(ins) && !INS_IsCall(ins);
// }

// // Track indirect jumps within `.text` section and valid target address range
// VOID TrackIndirectJump(ADDRINT ip, CONTEXT* ctxt, ADDRINT target, const std::string* insText) {
//     if (ip >= textSectionBase && ip < textSectionHigh && target >= textSectionBase && target < textSectionHigh) {
//         std::lock_guard<std::mutex> lock(IndirectJumpMutex);
//         auto& jumpInfo = IndirectJumps[ip];

//         if (jumpInfo.count == 0) {
//             jumpInfo.address = ip;
//             jumpInfo.insText = *insText;

//             PIN_LockClient();
//             RTN rtn = RTN_FindByAddress(ip);
//             jumpInfo.routine = RTN_Valid(rtn) ? RTN_Name(rtn) : "Unknown";
//             PIN_UnlockClient();
//         }

//         jumpInfo.targets.insert(target); // Add the target to the set
//         jumpInfo.count++;
//     }
// }

// // Load the image and identify the main executable and `.text` section
// VOID ImageLoad(IMG img, VOID* v) {
//     if (IMG_IsMainExecutable(img)) {
//         mainImageBase = IMG_LowAddress(img);
//         mainImageHigh = IMG_HighAddress(img);
//         mainImageName = IMG_Name(img);

//         // std::cout << "Main Executable: " << mainImageName << std::endl;
//         // std::cout << "Image Base Address: 0x" << std::hex << mainImageBase << std::endl;
//         // std::cout << "Image High Address: 0x" << mainImageHigh << std::endl;

//         // Find `.text` section range
//         for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec)) {
//             if (SEC_Name(sec) == ".text") {
//                 textSectionBase = SEC_Address(sec);
//                 textSectionHigh = textSectionBase + SEC_Size(sec);
//                 // std::cout << ".text Section Base: 0x" << std::hex << textSectionBase << std::endl;
//                 // std::cout << ".text Section High: 0x" << textSectionHigh << std::endl;
//                 break;
//             }
//         }

//         if (textSectionBase == 0 && textSectionHigh == 0) {
//             std::cerr << "Error: Could not find .text section in the main executable!" << std::endl;
//         }
//     }
// }

// // Finalize and log the results
// VOID Fini(INT32 code, VOID* v) {
//     if (OutputFile.is_open()) {
//         OutputFile << "Indirect Jumps in " << mainImageName << ":\n";
//         OutputFile << "Image Base Address: 0x" << std::hex << mainImageBase << "\n";
//         OutputFile << "Image High Address: 0x" << mainImageHigh << "\n";
//         OutputFile << ".text Section Base: 0x" << textSectionBase << "\n";
//         OutputFile << ".text Section High: 0x" << textSectionHigh << "\n";
//         OutputFile << "-----------------------------------\n";

//         std::lock_guard<std::mutex> lock(IndirectJumpMutex);
//         for (const auto& entry : IndirectJumps) {
//             const auto& jumpInfo = entry.second;
//             OutputFile << "Address: 0x" << std::hex << jumpInfo.address
//                        << " | Routine: " << jumpInfo.routine
//                        << " | Instruction: " << jumpInfo.insText
//                        << " | Executed: " << std::dec << jumpInfo.count << " times\n"
//                        << " | Targets: ";

//             for (const auto& target : jumpInfo.targets) {
//                 OutputFile << "0x" << std::hex << target << " ";
//             }

//             OutputFile << "\n";
//         }

//         OutputFile.close();
//     }
// }

// // Instrument instructions to track indirect jumps
// VOID Instruction(INS ins, VOID* v) {
//     if (IsIndirectJump(ins)) {
//         ADDRINT insAddr = INS_Address(ins);
//         if (insAddr >= textSectionBase && insAddr < textSectionHigh) {
//             std::string* disassembly = new std::string(INS_Disassemble(ins));

//             INS_InsertCall(ins, IPOINT_TAKEN_BRANCH, (AFUNPTR)TrackIndirectJump,
//                            IARG_INST_PTR,
//                            IARG_CONTEXT,
//                            IARG_BRANCH_TARGET_ADDR,
//                            IARG_PTR, disassembly,
//                            IARG_END);
//         }
//     }
// }

// // Main function
// int main(int argc, char* argv[]) {
//     if (PIN_Init(argc, argv)) {
//         std::cerr << "PIN initialization failed." << std::endl;
//         return 1;
//     }

//     OutputFile.open("/home/isec/Documents/indirectjump/redis/disassembly.log", std::ios::app);

//     if (!OutputFile.is_open()) {
//         std::cerr << "Failed to open the output file." << std::endl;
//         return 1;
//     }

//     IMG_AddInstrumentFunction(ImageLoad, 0);
//     INS_AddInstrumentFunction(Instruction, 0);
//     PIN_AddFiniFunction(Fini, 0);

//     PIN_StartProgram();
//     return 0;
// }


// #include <pin.H>
// #include <iostream>
// #include <fstream>
// #include <unordered_map>
// #include <unordered_set>
// #include <mutex>
// #include <string>
// #include <filesystem>  // For extracting the filename from the full path

// // Output file for logging indirect jumps
// std::ofstream OutputFile;

// // Mutex for thread-safe access to shared data
// std::mutex IndirectJumpMutex;

// // Global variables to store main executable and `.text` section information
// ADDRINT mainImageBase = 0;
// ADDRINT mainImageHigh = 0;
// ADDRINT textSectionBase = 0;
// ADDRINT textSectionHigh = 0;
// std::string mainImageName;

// // Structure to track jump information
// struct JumpInfo {
//     ADDRINT address;                         // Address of the jump instruction
//     std::string routine;                     // Routine name containing the jump
//     std::unordered_set<ADDRINT> targets;     // Set of unique jump target addresses
//     std::string insText;                     // Full instruction text
//     int count;                               // Number of times this jump was executed
// };

// // Hash map to store unique jumps
// std::unordered_map<ADDRINT, JumpInfo> IndirectJumps;

// // Simplified indirect jump detection
// BOOL IsIndirectJump(INS ins) {
//     return INS_IsIndirectControlFlow(ins) && !INS_IsCall(ins);
// }

// // Track indirect jumps within `.text` section and valid target address range
// VOID TrackIndirectJump(ADDRINT ip, CONTEXT* ctxt, ADDRINT target, const std::string* insText) {
//     if (ip >= textSectionBase && ip < textSectionHigh && target >= textSectionBase && target < textSectionHigh) {
//         std::lock_guard<std::mutex> lock(IndirectJumpMutex);
//         auto& jumpInfo = IndirectJumps[ip];

//         if (jumpInfo.count == 0) {
//             jumpInfo.address = ip;
//             jumpInfo.insText = *insText;

//             PIN_LockClient();
//             RTN rtn = RTN_FindByAddress(ip);
//             jumpInfo.routine = RTN_Valid(rtn) ? RTN_Name(rtn) : "Unknown";
//             PIN_UnlockClient();
//         }

//         jumpInfo.targets.insert(target); // Add the target to the set
//         jumpInfo.count++;
//     }
// }

// // Get the last part of the path (filename) from the full path
// std::string GetFileNameFromPath(const std::string& path) {
//     std::filesystem::path filePath(path);
//     return filePath.filename().string(); // Returns the filename without the directory
// }

// // Load the image and identify the main executable and `.text` section
// VOID ImageLoad(IMG img, VOID* v) {
//     if (IMG_IsMainExecutable(img)) {
//         mainImageBase = IMG_LowAddress(img);
//         mainImageHigh = IMG_HighAddress(img);
//         mainImageName = IMG_Name(img);

//         // Get the last part of the path (filename)
//         std::string fileName = GetFileNameFromPath(mainImageName);

//         // Construct the output file path using the filename
//         std::string outputFilePath = "/home/isec/Documents/indirectjump/result/" + fileName + "_disassembly.log";

//         OutputFile.open(outputFilePath, std::ios::app); // Open the log file for the current binary

//         if (!OutputFile.is_open()) {
//             std::cerr << "Failed to open the output file for " << mainImageName << std::endl;
//             return;
//         }

//         // Find `.text` section range
//         for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec)) {
//             if (SEC_Name(sec) == ".text") {
//                 textSectionBase = SEC_Address(sec);
//                 textSectionHigh = textSectionBase + SEC_Size(sec);
//                 break;
//             }
//         }

//         if (textSectionBase == 0 && textSectionHigh == 0) {
//             std::cerr << "Error: Could not find .text section in the main executable!" << std::endl;
//         }
//     }
// }

// // Finalize and log the results
// VOID Fini(INT32 code, VOID* v) {
//     if (OutputFile.is_open()) {
//         OutputFile << "Indirect Jumps in " << mainImageName << ":\n";
//         OutputFile << "Image Base Address: 0x" << std::hex << mainImageBase << "\n";
//         OutputFile << "Image High Address: 0x" << mainImageHigh << "\n";
//         OutputFile << ".text Section Base: 0x" << textSectionBase << "\n";
//         OutputFile << ".text Section High: 0x" << textSectionHigh << "\n";
//         OutputFile << "-----------------------------------\n";

//         std::lock_guard<std::mutex> lock(IndirectJumpMutex);
//         for (const auto& entry : IndirectJumps) {
//             const auto& jumpInfo = entry.second;
//             OutputFile << "Address: 0x" << std::hex << jumpInfo.address
//                        << " | Routine: " << jumpInfo.routine
//                        << " | Instruction: " << jumpInfo.insText
//                        << " | Executed: " << std::dec << jumpInfo.count << " times\n"
//                        << " | Targets: ";

//             for (const auto& target : jumpInfo.targets) {
//                 OutputFile << "0x" << std::hex << target << " ";
//             }

//             OutputFile << "\n";
//         }

//         OutputFile.close();
//     }
// }

// // Instrument instructions to track indirect jumps
// VOID Instruction(INS ins, VOID* v) {
//     if (IsIndirectJump(ins)) {
//         ADDRINT insAddr = INS_Address(ins);
//         if (insAddr >= textSectionBase && insAddr < textSectionHigh) {
//             std::string* disassembly = new std::string(INS_Disassemble(ins));

//             INS_InsertCall(ins, IPOINT_TAKEN_BRANCH, (AFUNPTR)TrackIndirectJump,
//                            IARG_INST_PTR,
//                            IARG_CONTEXT,
//                            IARG_BRANCH_TARGET_ADDR,
//                            IARG_PTR, disassembly,
//                            IARG_END);
//         }
//     }
// }

// // Main function
// int main(int argc, char* argv[]) {
//     if (PIN_Init(argc, argv)) {
//         std::cerr << "PIN initialization failed." << std::endl;
//         return 1;
//     }

//     IMG_AddInstrumentFunction(ImageLoad, 0);
//     INS_AddInstrumentFunction(Instruction, 0);
//     PIN_AddFiniFunction(Fini, 0);

//     PIN_StartProgram();
//     return 0;
// }

// #include <pin.H>
// #include <iostream>
// #include <fstream>
// #include <unordered_map>
// #include <unordered_set>
// #include <mutex>
// #include <string>
// #include <filesystem>
// #include <map>
// #include <sstream>

// // Mutex for thread-safe access to shared data
// std::mutex IndirectJumpMutex;

// // Global variables to store main executable and `.text` section information
// ADDRINT mainImageBase = 0;
// ADDRINT mainImageHigh = 0;
// ADDRINT textSectionBase = 0;
// ADDRINT textSectionHigh = 0;
// std::string mainImageName;

// // Map to store indirect jumps (address -> set of unique target addresses)
// std::map<ADDRINT, std::unordered_set<ADDRINT>> IndirectJumpTargets;

// // Map to store disassembly instructions for jump addresses
// std::map<ADDRINT, std::string> JumpDisassemblyInstructions;

// // Function to extract filename from the full path
// std::string GetFileNameFromPath(const std::string& path) {
//     std::filesystem::path filePath(path);
//     return filePath.filename().string(); // Returns the filename without the directory
// }

// // Function to load previous indirect jump data from file (if any)
// void LoadIndirectJumpData(const std::string& filePath) {
//     std::ifstream inputFile(filePath);
//     if (inputFile.is_open()) {
//         std::string line;
//         ADDRINT ip, target;
//         while (std::getline(inputFile, line)) {
//             std::istringstream ss(line);
//             if (ss >> std::hex >> ip) {
//                 while (ss >> std::hex >> target) {
//                     std::lock_guard<std::mutex> lock(IndirectJumpMutex);
//                     IndirectJumpTargets[ip].insert(target); // Accumulate targets for the same address
//                 }
//             }
//         }
//         inputFile.close();
//     }
// }

// // Save indirect jump data to file in ascending order of the jump addresses
// void SaveIndirectJumpData(const std::string& filePath) {
//     std::ofstream outputFile(filePath);
//     if (outputFile.is_open()) {
//         // Iterate over the map which is sorted by jump address (key)
//         for (const auto& entry : IndirectJumpTargets) {
//             // Print the jump address in hexadecimal
//             outputFile << "0x"<< std::hex << entry.first;

//             // Print each corresponding target address in hexadecimal
//             for (const auto& target : entry.second) {
//                 outputFile << " " << "0x"<< std::hex << target;
//             }

//             // Print a new line after each entry
//             outputFile << "\n";
//         }
//         outputFile.close();
//     } else {
//         std::cerr << "Error: Unable to open output file for saving indirect jump data." << std::endl;
//     }
// }

// // Save the disassembly instructions for indirect jumps to a separate file
// void SaveDisassemblyInstructions(const std::string& filePath) {
//     std::ofstream outputFile(filePath);
//     if (outputFile.is_open()) {
//         // Write the image base address to the file
//         // outputFile << "Image Base Address: 0x" << std::hex << mainImageBase << "\n\n";

//         // Iterate over the map which is sorted by jump address (key)
//         for (const auto& entry : JumpDisassemblyInstructions) {
//             // Output the jump address and corresponding disassembly instruction
//             outputFile << "0x" << std::hex << entry.first << ": " << entry.second << "\n";
//         }
//         outputFile.close();
//     } else {
//         std::cerr << "Error: Unable to open output file for saving disassembly instructions." << std::endl;
//     }
// }

// // Track indirect jumps within `.text` section and valid target address range
// VOID TrackIndirectJump(ADDRINT ip, CONTEXT* ctxt, ADDRINT target, const std::string* insText) {
//     if (ip >= textSectionBase && ip < textSectionHigh && target >= textSectionBase && target < textSectionHigh) {
//         std::lock_guard<std::mutex> lock(IndirectJumpMutex);
//         IndirectJumpTargets[ip].insert(target); // Insert target to the set for the given address
//         JumpDisassemblyInstructions[ip] = *insText; // Save the disassembly instruction for the jump address
//     }
// }

// // Load the image and identify the main executable and `.text` section
// VOID ImageLoad(IMG img, VOID* v) {
//     if (IMG_IsMainExecutable(img)) {
//         mainImageBase = IMG_LowAddress(img);
//         mainImageHigh = IMG_HighAddress(img);
//         mainImageName = IMG_Name(img);

//         // Get the last part of the path (filename)
//         std::string fileName = GetFileNameFromPath(mainImageName);

//         // Construct the output file paths using the filename
//         std::string jumpDataFilePath = "/home/isec/Documents/indirectjump/" + fileName + "_indirect_jumps.txt";
//         std::string imageInfoFilePath = "/home/isec/Documents/indirectjump/" + fileName + "_image_info.txt";
//         std::string disassemblyFilePath = "/home/isec/Documents/indirectjump/" + fileName + "_disassembly_instructions.txt";

//         // Load previous indirect jump data from the file (if any)
//         LoadIndirectJumpData(jumpDataFilePath);

//         // Write image information to the image info file
//         std::ofstream imageInfoFile(imageInfoFilePath);
//         if (imageInfoFile.is_open()) {
//             imageInfoFile << "Main Executable: " << mainImageName << "\n";
//             imageInfoFile << "Image Base Address: 0x" << std::hex << mainImageBase << "\n";
//             imageInfoFile << "Image High Address: 0x" << std::hex << mainImageHigh << "\n";
//             imageInfoFile.close();
//         }

//         // Find `.text` section range
//         for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec)) {
//             if (SEC_Name(sec) == ".text") {
//                 textSectionBase = SEC_Address(sec);
//                 textSectionHigh = textSectionBase + SEC_Size(sec);
//                 break;
//             }
//         }

//         if (textSectionBase == 0 && textSectionHigh == 0) {
//             std::cerr << "Error: Could not find .text section in the main executable!" << std::endl;
//         }

//         // Save disassembly instructions for indirect jumps
//         //SaveDisassemblyInstructions(disassemblyFilePath);
//     }
// }

// // Finalize and log the results
// VOID Fini(INT32 code, VOID* v) {
//     std::string fileName = GetFileNameFromPath(mainImageName);
//     std::string jumpDataFilePath = "/home/isec/Documents/indirectjump/" + fileName + "_indirect_jumps.txt";

//     // Save the indirect jump data to file (append new data)
//     SaveIndirectJumpData(jumpDataFilePath);

//     // Save the disassembly instructions
//     std::string disassemblyFilePath = "/home/isec/Documents/indirectjump/" + fileName + "_disassembly_instructions.txt";
//     SaveDisassemblyInstructions(disassemblyFilePath);
// }

// // Instrument instructions to track indirect jumps
// VOID Instruction(INS ins, VOID* v) {
//     if (INS_IsIndirectControlFlow(ins) && !INS_IsCall(ins)) {
//         ADDRINT insAddr = INS_Address(ins);
//         if (insAddr >= textSectionBase && insAddr < textSectionHigh) {
//             std::string* disassembly = new std::string(INS_Disassemble(ins));

//             INS_InsertCall(ins, IPOINT_TAKEN_BRANCH, (AFUNPTR)TrackIndirectJump,
//                            IARG_INST_PTR,
//                            IARG_CONTEXT,
//                            IARG_BRANCH_TARGET_ADDR,
//                            IARG_PTR, disassembly,
//                            IARG_END);
//         }
//     }
// }

// // Main function
// int main(int argc, char* argv[]) {
//     if (PIN_Init(argc, argv)) {
//         std::cerr << "PIN initialization failed." << std::endl;
//         return 1;
//     }

//     IMG_AddInstrumentFunction(ImageLoad, 0);
//     INS_AddInstrumentFunction(Instruction, 0);
//     PIN_AddFiniFunction(Fini, 0);

//     PIN_StartProgram();
//     return 0;
// }


#include <pin.H>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <string>
#include <filesystem>
#include <map>
#include <sstream>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

// Mutex for thread-safe access to shared data
std::mutex IndirectJumpMutex;

// Global variables to store main executable and `.text` section information
ADDRINT mainImageBase = 0;
ADDRINT mainImageHigh = 0;
ADDRINT textSectionBase = 0;
ADDRINT textSectionHigh = 0;
std::string mainImageName;

// Map to store indirect jumps (address -> set of unique target addresses)
std::map<ADDRINT, std::unordered_set<ADDRINT>> IndirectJumpTargets;

// Map to store disassembly instructions for jump addresses
std::map<ADDRINT, std::string> JumpDisassemblyInstructions;

// Global variable to store data file paths
std::string jumpDataFilePath = "/home/isec/Documents/indirectjump/indirect_jumps_backup.txt";
std::string disassemblyFilePath = "/home/isec/Documents/indirectjump/disassembly_backup.txt";

// Function to extract filename from the full path
std::string GetFileNameFromPath(const std::string& path) {
    std::filesystem::path filePath(path);
    return filePath.filename().string(); // Returns the filename without the directory
}

// Function to load previous indirect jump data from file (if any)
void LoadIndirectJumpData(const std::string& filePath, const std::string& backupPath) {
    std::ifstream inputFile(filePath);
    if (inputFile.is_open()) {
        std::string line;
        ADDRINT ip, target;
        while (std::getline(inputFile, line)) {
            std::istringstream ss(line);
            if (ss >> std::hex >> ip) {
                while (ss >> std::hex >> target) {
                    std::lock_guard<std::mutex> lock(IndirectJumpMutex);
                    IndirectJumpTargets[ip].insert(target); // Accumulate targets for the same address
                }
            }
        }
        inputFile.close();
    }
      std::ifstream BackupinputFile(backupPath);
    if (BackupinputFile.is_open()) {
        std::string line;
        ADDRINT ip, target;
        while (std::getline(BackupinputFile, line)) {
            std::istringstream ss(line);
            if (ss >> std::hex >> ip) {
                while (ss >> std::hex >> target) {
                    std::lock_guard<std::mutex> lock(IndirectJumpMutex);
                    IndirectJumpTargets[ip].insert(target); // Accumulate targets for the same address
                }
            }
        }
        BackupinputFile.close();
    }
}

// Save indirect jump data to file in ascending order of the jump addresses
void SaveIndirectJumpData(const std::string& filePath) {
    std::ofstream outputFile(filePath);
    if (outputFile.is_open()) {
        // Iterate over the map which is sorted by jump address (key)
        for (const auto& entry : IndirectJumpTargets) {
            // Print the jump address in hexadecimal
            outputFile << "0x" << std::hex << entry.first;

            // Print each corresponding target address in hexadecimal
            for (const auto& target : entry.second) {
                outputFile << " " << "0x" << std::hex << target;
            }

            // Print a new line after each entry
            outputFile << "\n";
        }
        outputFile.close();
    } else {
        std::cerr << "Error: Unable to open output file for saving indirect jump data." << std::endl;
    }
}

// Save the disassembly instructions for indirect jumps to a separate file
void SaveDisassemblyInstructions(const std::string& filePath) {
    std::ofstream outputFile(filePath);
    if (outputFile.is_open()) {
        // Write the image base address to the file
        // outputFile << "Image Base Address: 0x" << std::hex << mainImageBase << "\n\n";

        // Iterate over the map which is sorted by jump address (key)
        for (const auto& entry : JumpDisassemblyInstructions) {
            // Output the jump address and corresponding disassembly instruction
            outputFile << "0x" << std::hex << entry.first << ": " << entry.second << "\n";
        }
        outputFile.close();
    } else {
        std::cerr << "Error: Unable to open output file for saving disassembly instructions." << std::endl;
    }
}

// Track indirect jumps within `.text` section and valid target address range
VOID TrackIndirectJump(ADDRINT ip, CONTEXT* ctxt, ADDRINT target, const std::string* insText) {
    if (ip >= textSectionBase && ip < textSectionHigh && target >= textSectionBase && target < textSectionHigh) {
        std::lock_guard<std::mutex> lock(IndirectJumpMutex);
        IndirectJumpTargets[ip].insert(target); // Insert target to the set for the given address
        JumpDisassemblyInstructions[ip] = *insText; // Save the disassembly instruction for the jump address
    }
}

// Load the image and identify the main executable and `.text` section
VOID ImageLoad(IMG img, VOID* v) {
    if (IMG_IsMainExecutable(img)) {
            // Get the last part of the path (filename)
        std::string fileName = GetFileNameFromPath(mainImageName);
        mainImageBase = IMG_LowAddress(img);
        mainImageHigh = IMG_HighAddress(img);
        mainImageName = IMG_Name(img);
         // Construct the output file paths using the filename
        std::string imageBaseFilePath = "/home/isec/Documents/indirectjump/result/" + fileName + "_image_info.txt";
             // Open the file to append image base and end address
        std::ofstream outputFile(imageBaseFilePath); // Open in append mode
        if (outputFile.is_open()) {
            // Output the image base and end address to the file
            outputFile << "Main Image Base Address: 0x" << std::hex << mainImageBase << std::endl;
            outputFile << "Main Image End Address: 0x" << std::hex << mainImageHigh << std::endl;
            outputFile.close();
        } else {
            std::cerr << "Error: Unable to open file for saving image base and end address." << std::endl;
        }

    
        // Global variable to store data file paths
        std::string jumpDataFileBackupPath = "/home/isec/Documents/indirectjump/indirect_jumps_backup.txt";
        std::string disassemblyFileBackupPath = "/home/isec/Documents/indirectjump/disassembly_backup.txt";

        // Construct the output file paths using the filename
        jumpDataFilePath = "/home/isec/Documents/indirectjump/result/" + fileName + "_indirect_jumps.txt";
        disassemblyFilePath = "/home/isec/Documents/indirectjump/result/" + fileName + "_disassembly_instructions.txt";

        // Load previous indirect jump data from the file (if any)
        LoadIndirectJumpData(jumpDataFilePath,jumpDataFileBackupPath);

        // Find `.text` section range
        for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec)) {
            if (SEC_Name(sec) == ".text") {
                textSectionBase = SEC_Address(sec);
                textSectionHigh = textSectionBase + SEC_Size(sec);
                break;
            }
        }

        if (textSectionBase == 0 && textSectionHigh == 0) {
            std::cerr << "Error: Could not find .text section in the main executable!" << std::endl;
        }
    }
}

// Signal handler to handle termination signals
void SignalHandler(int signum) {
    std::cerr << "Received signal " << signum << ". Saving data..." << std::endl;
    // Ensure data is saved
    SaveIndirectJumpData(jumpDataFilePath);
    SaveDisassemblyInstructions(disassemblyFilePath);
    exit(signum); // Exit after saving data
}

// Function to register signal handler
void RegisterSignalHandlers() {
    signal(SIGINT, SignalHandler);   // Handle interrupt signal (Ctrl+C)
    signal(SIGSEGV, SignalHandler);  // Handle segmentation fault
    signal(SIGTERM, SignalHandler);  // Handle termination signal
    // Add other signals as needed
}

// Function to register a backup exit handler using atexit
void RegisterExitHandler() {
    atexit([]() {
        std::cerr << "Program is exiting normally. Saving data..." << std::endl;
        SaveIndirectJumpData(jumpDataFilePath);
        SaveDisassemblyInstructions(disassemblyFilePath);
    });
}

// // Finalize and log the results
// VOID Fini(INT32 code, VOID* v) {
//     // std::cerr << "Program finished. Saving data..." << std::endl;
//     // Get the last part of the path (filename)
//         std::string fileName = GetFileNameFromPath(mainImageName);

//         // Construct the output file paths using the filename
//         jumpDataFilePath = "/home/isec/Documents/indirectjump/result/" + fileName + "_indirect_jumps.txt";
//         disassemblyFilePath = "/home/isec/Documents/indirectjump/result/" + fileName + "_disassembly_instructions.txt"
//     SaveIndirectJumpData(jumpDataFilePath);
//     SaveDisassemblyInstructions(disassemblyFilePath);
// }
// Finalize and log the results
VOID Fini(INT32 code, VOID* v) {
    std::string fileName = GetFileNameFromPath(mainImageName);
    std::string jumpDataFilePath = "/home/isec/Documents/indirectjump/result/" + fileName + "_indirect_jumps.txt";

    // Save the indirect jump data to file (append new data)
    SaveIndirectJumpData(jumpDataFilePath);

    // Save the disassembly instructions
    std::string disassemblyFilePath = "/home/isec/Documents/indirectjump/result/" + fileName + "_disassembly_instructions.txt";
    SaveDisassemblyInstructions(disassemblyFilePath);
}

// Instrument instructions to track indirect jumps
VOID Instruction(INS ins, VOID* v) {
    if (INS_IsIndirectControlFlow(ins) && !INS_IsCall(ins)) {
        ADDRINT insAddr = INS_Address(ins);
        if (insAddr >= textSectionBase && insAddr < textSectionHigh) {
            std::string* disassembly = new std::string(INS_Disassemble(ins));

            INS_InsertCall(ins, IPOINT_TAKEN_BRANCH, (AFUNPTR)TrackIndirectJump,
                           IARG_INST_PTR,
                           IARG_CONTEXT,
                           IARG_BRANCH_TARGET_ADDR,
                           IARG_PTR, disassembly,
                           IARG_END);
        }
    }
}

// Main function
int main(int argc, char* argv[]) {
    if (PIN_Init(argc, argv)) {
        std::cerr << "PIN initialization failed." << std::endl;
        return 1;
    }

    // Register signal handlers and atexit function for backup saving
    RegisterSignalHandlers();
    RegisterExitHandler();

    IMG_AddInstrumentFunction(ImageLoad, 0);
    INS_AddInstrumentFunction(Instruction, 0);
    PIN_AddFiniFunction(Fini, 0);

    PIN_StartProgram();
    return 0;
}

