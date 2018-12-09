#include "ui.h"

using namespace std;

void ui::print_info(string depth) {
  cout << "Indexer informations:" << endl;
  boost::filesystem::path ramdisk(Config::ramdisk_path());
  if (boost::filesystem::is_directory(ramdisk)) {
    string all_files = utils::exec("find "+Config::ramdisk_path()+" -type f | wc -l");
    string df_info = utils::exec("df -h | grep "+Config::ramdisk_path());
    cout << df_info << endl;
    cout << "all files in disk: " << all_files << endl;

    string cmd = "tree -d -L " + depth + " " + Config::ramdisk_path();
    cout << utils::exec(cmd) << endl;
  } else {
    cout << "no ramdisk in system" << endl;
  }
}

void ui::print_about() {
  cout << "$ ./indexer COMMAND OPTIONS:" << endl;
  cout << "COMMANDS:" << endl;
  cout << " install               - creates ramdisk from settings in indexer.cfg" << endl;
  cout << " remove                - removes ramdisk" << endl;
  cout << " index    PATH         - copy directory to ramdisk and remove all non text files, arguments:" << endl;
  cout << " info     [tree_depth] - print info about indexer status" << endl;
  cout << " load     PATH         - load .index file from given path. Generally it's for mass load directories to ramdisk." << endl;
  cout << " grep     TEXT         - starts grep in ramdisk" << endl;
  cout << " refresh               - checks for changes in files (date, size, name)" << endl;
  cout << " deletebins            - remove all bins from remdisk" << endl;
}