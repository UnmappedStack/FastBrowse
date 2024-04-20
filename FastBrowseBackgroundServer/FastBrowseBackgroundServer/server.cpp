#include "server.h"
#include <httplib.h>
#include <iostream>
#include <string>
#include <vector>
#include <lmcons.h>
#include <locale>
#include <fstream>
#include <windows.h>
#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <codecvt>
#include "searchfiles.h"
#include "loadfiles.h"
#include <shobjidl.h>
#include <shlwapi.h>

#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Shlwapi.lib")

using namespace httplib;
namespace fs = std::filesystem;

std::wstring join(const std::vector<std::wstring>& vec, const std::wstring& separator) {
    if (vec.empty())
        return L""; // Return an empty string if the vector is empty
    std::wstring result = vec[0]; // Initialize the result with the first element
    // Iterate over the vector starting from the second element
    for (size_t i = 1; i < vec.size(); ++i) {
        result += separator + vec[i]; // Concatenate the separator and the current element
    }
    return result;
}

std::vector<std::string> splitString(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> substrings;
    size_t pos = 0;
    size_t found;
    while ((found = str.find(delimiter, pos)) != std::string::npos) {
        substrings.push_back(str.substr(pos, found - pos));
        pos = found + delimiter.length();
    }
    // Add the last substring (after the last delimiter)
    substrings.push_back(str.substr(pos));
    return substrings;
}

std::wstring utf8_to_utf16(const std::string& utf8) {
    int requiredSize = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, nullptr, 0);
    std::wstring utf16(requiredSize, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, &utf16[0], requiredSize);
    return utf16;
}

std::string utf16_to_utf8(const std::wstring& utf16) {
    int requiredSize = WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8(requiredSize, 0);
    WideCharToMultiByte(CP_UTF8, 0, utf16.c_str(), -1, &utf8[0], requiredSize, nullptr, nullptr);
    return utf8;
}

std::string join_utf8(const std::vector<std::string>& vec, const std::string& substr) {
    if (vec.empty()) {
        return "";
    }

    std::string result = vec[0];
    for (size_t i = 1; i < vec.size(); ++i) {
        result += substr + vec[i];
    }
    return result;
}

bool checkSubstringCount(const std::string& str) {
    size_t pos = 0;
    size_t count = 0;
    while ((pos = str.find('\\', pos)) != std::string::npos) {
        ++count;
        if (count > 1 || pos != str.length() - 1) {
            return false;
        }
        pos += 1; // Move past the found '\'
    }
    return true;
}

std::string checkFolder(auto& folder) {
    try {
        std::vector<std::pair<std::string, std::filesystem::file_time_type>> allFDs;

        // Populate allFDs with file paths and their last write times
        for (const auto& entry : fs::directory_iterator(folder)) {
            try {
                if (fs::is_directory(entry.status())) {
                    allFDs.emplace_back(entry.path().string() + "\\", fs::last_write_time(entry));
                }
                else if (fs::is_regular_file(entry.status())) {
                    allFDs.emplace_back(entry.path().string(), fs::last_write_time(entry));
                }
            }
            catch (const fs::filesystem_error& ex) {
                // Handle filesystem errors if needed
            }
        }

        // Sort allFDs based on the type (folder or file) and last write times
        std::sort(allFDs.begin(), allFDs.end(), [](const auto& a, const auto& b) {
            if (fs::is_directory(a.first) && !fs::is_directory(b.first)) {
                return true; // a is a directory, prioritize it
            }
            else if (!fs::is_directory(a.first) && fs::is_directory(b.first)) {
                return false; // b is a directory, prioritize it
            }
            else {
                // Both are either directories or files, sort by last write time
                return a.second > b.second; // Sort in descending order (most recent first)
            }
            });

        // Extract only file paths from the sorted vector
        std::vector<std::string> sortedPaths;
        for (const auto& entry : allFDs) {
            sortedPaths.push_back(entry.first);
        }

        return join_utf8(sortedPaths, "[|filesplit|]");
    }
    catch (const fs::filesystem_error& ex) {
        return "ERROR";
    }
    catch (const std::system_error& ex) {
        return "ERROR";
    }
}

std::string retrieveFolder(auto& folder) {
    return checkFolder(folder); // I know this is a terrible way to do things but I can't bother changing it
}

/*std::string retrieveFolder(auto& folder) {
    std::ifstream inputFile("filescache.txt");
    std::string statusRead;
    if (inputFile.is_open()) {
        std::string contents;
        if (std::getline(inputFile, contents)) {
            // For each file, check if it's in the folder AND there's no more than 1 \ after that point
            // First, split the file by [|filesplit|]
            std::vector<std::string> filesVec = splitString(contents, "[|filesplit|]");
            // Go through each file and check it's in the correct folder and isn't a child
            std::vector<std::string> correctFiles;
            size_t found;
            size_t pos;
            std::string altered;
            for (int i = 0; i < filesVec.size(); i++) {
                found = filesVec[i].find(folder);
                if (found != std::string::npos) {
                    // Check if it's a child
                    // First, remove `folder` from the path
                    pos = filesVec[i].find(folder);
                    altered = filesVec[i];
                    altered.erase(pos, folder.length());
                    // Now check if it has more than two \s
                    if (checkSubstringCount(altered)) {
                        // It's not a child and it is in the correct directory. Add to correctFiles.
                        correctFiles.push_back(filesVec[i]);
                    }
                }
            }
            // Return correctFiles joined by [|filesplit|]
            statusRead = join_utf8(correctFiles, "[|filesplit|]");
        }
        else {
            statusRead = "ERROR: Cannot open cache file.";
        }
    }
    else {
        statusRead = "ERROR: Cannot read from cache file.";
    }
    return statusRead;
}*/

void removeSubstring(std::string& str, const std::string& sub) {
    if (str.substr(0, sub.length()) == sub) {
        str = str.substr(sub.length());
    }
}

void handle_search(const Request& req, Response& res) {
    auto k = req.get_param_value("k"); // Query
    auto d = req.get_param_value("d"); // Directory to search
    std::string txtfiles = retrieveFolder(d);
    std::vector<std::string> allfiles = splitString(txtfiles, "[|filesplit|]");
    for (std::string& str : allfiles) {
        removeSubstring(str, d);
    }
    std::vector<std::string> allResults = searchFiles(allfiles, k);
    // Join allResults into a single string which can be returned
    std::string toReturn = join_utf8(allResults, "[|filesplit|]");
    res.set_content(toReturn, "text/html");
}

void handle_searchdevice(const Request& req, Response& res) {
    auto k = req.get_param_value("k"); // Query
    std::string txtfiles;
    std::ifstream inputFile("filescache.txt");
    if (inputFile.is_open()) {
        if (std::getline(inputFile, txtfiles)) {
            std::vector<std::string> allfiles = splitString(txtfiles, "[|filesplit|]");
            std::vector<std::string> allResults = searchFiles(allfiles, k);
            // Join allResults into a single string which can be returned
            std::string toReturn = join_utf8(allResults, "[|filesplit|]");
            std::cout << "Number of results found: " << allResults.size() << std::endl;
            res.set_content(toReturn, "text/html");
        }
        else {
            res.set_content("ERROR", "text/html");
        }
    }
    else {
        res.set_content("ERROR", "text/html");
    }
}

void handle_getall(const Request& req, Response& res) {
    wchar_t username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    std::wstring wpath = L"C:\\Users\\";
    if (GetUserNameW(username, &username_len)) {
        wpath = L"C:\\Users\\" + std::wstring(username) + L"\\";
    }
    else {
        std::cout << "Failed to get user. Using C:\\Users\\ path instead." << std::endl;
    }
    std::vector<std::wstring> allFiles = getFiles(wpath);
    // Now join it all into a string split by [|filesplit|]
    std::wstring joined = join(allFiles, L"[|filesplit|]");
    std::string content = utf16_to_utf8(joined);
    std::ofstream outputFile("filescache.txt");
    std::string statusSave;
    if (outputFile.is_open()) {
        outputFile << content;
        outputFile.close();
        statusSave = "Successfully saved to cache file.";
    }
    else {
        statusSave = "ERROR: Couldn't save cache.";
    }
    res.set_content(statusSave, "text/html");
}

void handle_retrieveFolder(const Request& req, Response& res) {
    auto folder = req.get_param_value("folder");
    std::string statusRead = retrieveFolder(folder);
    res.set_content(statusRead, "text/html");
}

void handle_getuser(const Request& req, Response& res) {
    wchar_t username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    if (GetUserNameW(username, &username_len)) {
        std::wcout << L"User recieved from Windows API: " << std::wstring(username) << std::endl;
    }
    else {
        std::cout << "Failed to get user" << std::endl;
    }
    res.set_content(utf16_to_utf8(std::wstring(username)), "text/html");
}

void openfile_choose(auto& filename) {
    // Convert filename from UTF-8 to UTF-16 (wide character string)
    std::wstring wideFilename;
    wideFilename.resize(MultiByteToWideChar(CP_UTF8, 0, filename.c_str(), -1, nullptr, 0));
    MultiByteToWideChar(CP_UTF8, 0, filename.c_str(), -1, &wideFilename[0], static_cast<int>(wideFilename.size()));
    // Prepare the SHELLEXECUTEINFO structure
    SHELLEXECUTEINFO shex;
    ZeroMemory(&shex, sizeof(SHELLEXECUTEINFO));
    shex.cbSize = sizeof(SHELLEXECUTEINFO);
    shex.lpVerb = L"openas";
    shex.lpFile = wideFilename.c_str();
    shex.nShow = SW_SHOW;
    shex.fMask = SEE_MASK_INVOKEIDLIST;
    // Execute the shell command
    if (!ShellExecuteEx(&shex)) {
        std::cerr << "Failed to open file with 'Open With' dialog" << std::endl;
    }
}

bool isExecutable(const std::string& str) {
    if (str.length() < 4)
        return false;
    std::string lastFour = str.substr(str.length() - 4);
    return lastFour == ".exe" || lastFour == ".msi" || lastFour == ".lnk";
}

void handle_openfile(const Request& req, Response& res) {
    auto filename = req.get_param_value("filename");
    if (isExecutable(filename)) {
        std::string command = "start /b \"\" \"" + filename + "\"";
        system(command.c_str());
    }
    else {
        openfile_choose(filename);
    }
}

void handle_terminalopencd(const Request& req, Response& res) {
    auto cd = req.get_param_value("cd");
    std::cout << "Opening " << cd << " in terminal." << std::endl;
    // Command to open folder in a new terminal window
    const char* command = "start cmd /k cd /d \"%s\"";
    // Convert std::string to C-style string
    const char* folderPathCStr = cd.c_str();
    // Construct the full command
    char fullCommand[1024];
    snprintf(fullCommand, sizeof(fullCommand), command, folderPathCStr);
    std::cout << command << std::endl;
    // Execute the command
    std::system(fullCommand);
}

void handle_createfile(const Request& req, Response& res) {
    auto fn = req.get_param_value("fname");
    std::cout << "Trying to open file: " << fn << std::endl;
    std::ofstream outputFile(fn);
    if (outputFile.is_open()) {
        std::cout << "File opened." << std::endl;
        outputFile.close();
    }
    else {
        std::cout << "Failed to create file" << std::endl;
    }
}

void handle_createfolder(const Request& req, Response& res) {
    auto dn = req.get_param_value("dname");
    if (!fs::exists(dn)) {
        fs::create_directory(dn);
    }
}

void handle_deletefile(const Request& req, Response& res) {
    auto fn = req.get_param_value("fname");
    std::wstring wfn(fn.begin(), fn.end()); // Convert narrow-character string to wide-character string
    if (fn.back() == '\\') {
        RemoveDirectory(wfn.c_str()); // Use the wide-character string
    }
    else {
        std::remove(fn.c_str());
    }
}

bool copyFileToDirectory(const std::string& sourceFilePath, const std::string& destinationDirectory) {
    try {
        // Check if the source file exists
        if (!fs::exists(sourceFilePath)) {
            std::cerr << "Source file does not exist." << std::endl;
            return false;
        }

        // Check if the destination directory exists, create if not
        if (!fs::exists(destinationDirectory)) {
            fs::create_directories(destinationDirectory);
        }

        // Get the filename from the source file path
        std::string filename = fs::path(sourceFilePath).filename().string();

        // Construct the destination file path
        std::string destinationFilePath = destinationDirectory + "/" + filename;

        // Open source file for binary reading
        std::ifstream sourceFile(sourceFilePath, std::ios::binary);
        if (!sourceFile) {
            std::cerr << "Failed to open source file." << std::endl;
            return false;
        }

        // Open destination file for binary writing
        std::ofstream destinationFile(destinationFilePath, std::ios::binary);
        if (!destinationFile) {
            std::cerr << "Failed to create destination file." << std::endl;
            return false;
        }

        // Copy contents from source file to destination file
        destinationFile << sourceFile.rdbuf();

        // Close files
        sourceFile.close();
        destinationFile.close();

        std::cout << "File copied successfully to " << destinationDirectory << std::endl;
        return true;
    }
    catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return false;
    }
}

void handle_pastefile(const Request& req, Response& res) {
    auto from = req.get_param_value("from");
    auto to = req.get_param_value("to");
    copyFileToDirectory(from, to);
}

void handle_renamefile(const Request& req, Response& res) {
    auto from = req.get_param_value("from");
    auto to = req.get_param_value("to");
    if (std::rename(from.c_str(), to.c_str()) != 0) {
        res.set_content("ERROR", "text/html");
    }
}

void runServer() {
    Server svr;

    // Define routes
    svr.Get("/renamefile", handle_renamefile);
    svr.Get("/pastefile", handle_pastefile);
    svr.Get("/deletefile", handle_deletefile);
    svr.Get("/createfolder", handle_createfolder);
    svr.Get("/search", handle_search);
    svr.Get("/createfile", handle_createfile);
    svr.Get("/getall", handle_getall);
    svr.Get("/terminalopencd", handle_terminalopencd);
    svr.Get("/retrievefolder", handle_retrieveFolder);
    svr.Get("/searchdevice", handle_searchdevice);
    svr.Get("/openfile", handle_openfile);
    svr.Get("/getuser", handle_getuser);

    // Start the server
    if (svr.listen("localhost", 1104)) {
        std::cout << "Server started at http://localhost:1104" << std::endl;
    }
    else {
        std::cout << "Failed to start server. Exiting..." << std::endl;
        exit(EXIT_FAILURE);
    }
}
