// server.h

#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <vector>

std::wstring utf8_to_utf16(const std::string& utf8);
std::string utf16_to_utf8(const std::wstring& utf16);
void runServer();

#endif