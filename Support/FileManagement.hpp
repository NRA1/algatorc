#ifndef ALGATORC_FILEMANAGEMENT_HPP
#define ALGATORC_FILEMANAGEMENT_HPP
#include <utility>
#include <vector>

std::pair<char*, unsigned int> readInputFile();
void writeOutputFile(const char* buffer, unsigned int size);
void writeSuccessStatusFile(const std::vector<long int>& times);

#endif //ALGATORC_FILEMANAGEMENT_HPP
