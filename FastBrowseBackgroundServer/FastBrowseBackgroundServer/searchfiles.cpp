#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <Windows.h>
#include <cctype>
#include <algorithm>
#include <fstream>
#include "searchfiles.h"
#include <lmcons.h>
#include "server.h"
namespace fs = std::filesystem;
std::string toLowerCase(const std::string& str) {
    std::string result;
    std::locale loc;
    for (char c : str) {
        result += std::tolower(c, loc);
    }
    return result;
}

std::vector<std::string> searchFiles(std::vector<std::string>& allFiles, std::string searchTerm) {
    std::cout << "Searching your device, please wait..." << std::endl;
    std::cout << "Searching for " << searchTerm << " from " << allFiles.size() << " files." << std::endl;
    std::cout << allFiles[0] << std::endl;
    std::string searchTermN = toLowerCase(searchTerm);
    // Check if the term is in each file name crawled
    size_t found;
    std::string lowerAllFiles;
    int numFound = 0;
    std::vector<std::string> foundFiles;
    for (int i = 0; i < allFiles.size(); i++) {
        lowerAllFiles = toLowerCase(allFiles[i]);
        found = toLowerCase(lowerAllFiles).find(searchTermN);
        if (found != std::string::npos) {
            foundFiles.push_back(allFiles[i]);
            numFound++;
        }
    }
    if (numFound == 0) {
        std::wcout << L"No instances of the search term were found. Try adjusting your search." << std::endl;
    }
    std::cout << numFound << " results have been found." << std::endl;
    return foundFiles;
}