#include "config.h"

using namespace std;

void Config::load_cfg() {
  utils::exec("touch ~/.indexer.cfg");
  string config = utils::exec("cat ~/.indexer.cfg");
  vector<string> lines = utils::split_string(config, "\n");
  for (int i=0; i<lines.size(); i++) {
    string line = lines[i];
    vector<string> item = utils::split_string(line, "[:]");
    string value = item[1];
    boost::trim(value);
    if (item[0] == "ramdisk_size") Config::ramdisk_size_string = value;
    if (item[0] == "ramdisk_path") Config::ramdisk_path_string = value;
  }
  // set default values if something wrong with config
  if (Config::ramdisk_size_string.empty()) {
    cout << "insert ramdisk_size to ~/.index.cfg - 'ramdisk_size: 7096M'" << endl;
    Config::ramdisk_size_string = "7096M";
  }
  if (Config::ramdisk_path_string.empty()) {
    cout << "insert ramdisk_path to ~/.index.cfg - 'ramdisk_path: /media/indexer_ramdisk'" << endl;
    Config::ramdisk_path_string = "/media/indexer_ramdisk";
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
