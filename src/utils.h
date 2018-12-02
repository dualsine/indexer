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
#include <sys/time.h>
#include "config.h"

namespace utils {
  std::string exec(std::string cmd);
  bool is_binary(const std::string path);
  bool create_ramdisk(std::string name, std::string size);
  bool clear_ramdisk(std::string path);
  std::string replace_string(std::string subject, const std::string& search, const std::string& replace);
  long int get_time();
  std::vector<std::string> load_file(std::string path);
  void mass_delete(std::vector<std::string> files);
  void copy_to_ramdisk(std::string root);
  std::vector<std::string> find_binaries(std::string ramdisk_destination);
  std::vector<std::string> split_string(const std::string& stringToSplit, const std::string& regexPattern);
  std::string create_sha(std::string root);
  bool is_new_sha(std::string root);
}

#endif