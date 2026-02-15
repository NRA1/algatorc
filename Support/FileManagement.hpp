#ifndef ALGATORC_FILEMANAGEMENT_HPP
#define ALGATORC_FILEMANAGEMENT_HPP
#include <filesystem>
#include <utility>
#include <vector>

#include "Error.hpp"

std::string readTextFile(const std::filesystem::path& path);
std::pair<char*, unsigned int> readBinaryFile(const std::filesystem::path& path);
void writeBinaryFile(const std::filesystem::path& path, const char* buffer, unsigned int size);
void writeSuccessStatusFile(const std::vector<long int>& times);

#endif //ALGATORC_FILEMANAGEMENT_HPP
