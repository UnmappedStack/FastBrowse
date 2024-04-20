#include "loadfiles.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <Windows.h>
#include <cctype>
#include <lmcons.h>
#include <fstream>
namespace fs = std::filesystem;
void searchDirectory(const std::wstring& path, std::vector<std::wstring>& allFiles) {
    try {
        for (const auto& entry : fs::directory_iterator(path)) {
            try {
                if (fs::is_directory(entry.status())) {
                    if (1) {
                        allFiles.push_back(entry.path().wstring() + L"\\");
                        searchDirectory(entry.path().wstring(), allFiles);
                    }
                }
                else if (fs::is_regular_file(entry.status())) {
                    allFiles.push_back(entry.path().wstring());
                }
            }
            catch (const fs::filesystem_error& ex) {
            }
            catch (const std::system_error& ex) {
            }
        }
    }
    catch (const fs::filesystem_error& ex) {
    }
    catch (const std::system_error& ex) {
    }
}
std::vector<std::wstring> getFiles(std::wstring fullDirectory) {
    std::vector<std::wstring> allFiles;
    try {
         std::wcout << L"Loading your files..." << std::endl;
         searchDirectory(fullDirectory, allFiles);
         std::wcout << L"Done." << std::endl;
         return allFiles;
    }
    catch (const std::system_error& ex) {
    }
    catch (...) {
    }
}

