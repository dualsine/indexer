#ifndef UTILS_H_
#define UTILS_H_

#include <set>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <regex>
#include <streambuf>
#include "boost/filesystem.hpp"
#include <postgresql/libpq-fe.h>
#include <boost/algorithm/string/replace.hpp>

namespace utils {
  std::string exec(std::string cmd);
  bool is_binary(const std::string path);
  std::vector<std::string> split_string(const std::string& stringToSplit, const std::string& regexPattern);
  bool create_ramdisk(std::string name, std::string size);
}

#endif