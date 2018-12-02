#include "config.h"

using namespace std;

void Config::load_cfg() {
  ifstream file("./indexer.cfg");
  if (file.is_open()) {
    string line;
    while(getline(file, line)) {
      vector<string> item = utils::split_string(line, "[:]");
      string value = item[1];
      boost::trim(value);
      if (item[0] == "ramdisk_size") Config::ramdisk_size_string = value;
      if (item[0] == "ramdisk_path") Config::ramdisk_path_string = value;
    }
  } else {
    cout << "error when opening config file";
  }
}
string Config::ramdisk_path(){
  if (Config::ramdisk_path_string.empty()) {
    Config::load_cfg();
  }
  return Config::ramdisk_path_string;
}
string Config::ramdisk_size(){
  if (Config::ramdisk_size_string.empty()) {
    Config::load_cfg();
  }
  return Config::ramdisk_size_string;
}

std::string Config::ramdisk_size_string;
std::string Config::ramdisk_path_string;
