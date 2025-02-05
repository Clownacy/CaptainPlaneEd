#pragma once

#include <filesystem>
#include <ostream>

long ReadPlain(const std::filesystem::path &srcfile, std::ostream &dstfile, const long Pointer, int length);
bool CheckCreateBlankFile(const std::filesystem::path &srcfile, std::ostream &dstfile, const long Pointer, const int length);
