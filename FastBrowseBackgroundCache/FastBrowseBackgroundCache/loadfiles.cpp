#include "loadfiles.h"
#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <Windows.h>
#include <lmcons.h>
#include <fstream>

namespace fs = std::filesystem;

void searchDirectory(const std::wstring& path, std::vector<std::wstring>& allFiles) {
    try {
        for (const auto& entry : fs::directory_iterator(path)) {
            try {
                if (fs::is_directory(entry.status())) {
                    allFiles.push_back(entry.path().wstring() + L"\\");
                    searchDirectory(entry.path().wstring(), allFiles);
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

std::wstring getUserFolderPath() {
    wchar_t username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    if (!GetUserNameW(username, &username_len)) {
        std::wcerr << L"Failed to get username." << std::endl;
        return L"";
    }
    return L"C:\\Users\\" + std::wstring(username) + L"\\";
}

std::wstring join_utf8(const std::vector<std::wstring>& vec, const std::wstring& substr) {
    if (vec.empty()) {
        return L"";
    }
    std::wstring result = vec[0];
    for (size_t i = 1; i < vec.size(); ++i) {
        result += substr + vec[i];
    }
    return result;
}

std::string getFiles() {
    std::vector<std::wstring> allFiles;
    try {
        std::wcout << L"Loading your files..." << std::endl;
        searchDirectory(getUserFolderPath(), allFiles);
        std::wcout << L"Done." << std::endl;

        // Convert wide string vector to string
        std::wstring joined = join_utf8(allFiles, L"[|filesplit|]");
        int utf8Length = WideCharToMultiByte(CP_UTF8, 0, joined.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (utf8Length == 0) {
            // Handle error
            return "";
        }
        std::string utf8String(utf8Length, '\0');
        WideCharToMultiByte(CP_UTF8, 0, joined.c_str(), -1, &utf8String[0], utf8Length, nullptr, nullptr);
        return utf8String;
    }
    catch (const std::system_error& ex) {
    }
    catch (...) {
    }
    return ""; // Return empty string in case of error
}
