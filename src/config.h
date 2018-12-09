#ifndef CONFIG_H_
#define CONFIG_H_

#include <iostream>
#include <fstream>
#include <vector>
#include "utils.h"
#include <boost/algorithm/string.hpp>

class Config{
public:
  static std::string ramdisk_size();
  static std::string ramdisk_path();
private:
  static void load_cfg();
  static std::string ramdisk_path_string;
  static std::string ramdisk_size_string;
};

#endif