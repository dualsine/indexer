#include "ui.h"

using namespace std;

void ui::print_info(string depth) {
  string all_files = utils::exec("find "+Config::ramdisk_path()+" -type f | wc -l");
  string df_info = utils::exec("df -h | grep "+Config::ramdisk_path());
  cout << "Indexer informations:\n" << df_info << endl;
  cout << "all files in disk: " << all_files << endl;

  string cmd = "tree -L "+depth+" "+Config::ramdisk_path();
  cout << utils::exec(cmd) << endl;
}

void ui::print_about() {
  cout << "$ ./indexer COMMAND OPTIONS:" << endl;
  cout << "COMMANDS:" << endl;
  cout << " install - creates ramdisk from settings in indexer.cfg" << endl;
  cout << " remove - removes ramdisk" << endl;
  cout << " index - copy directory to ramdisk and remove all non text files, arguments:" << endl;
  cout << "       PATH - (required) absolute path to root directory of project to index" << endl;
  cout << " info [tree_depth] - print info about indexer status" << endl;
  cout << " load PATH  - load .index file from given path (in .gitignore format). See README.md for more help. Generally it's for mass load directories to ramdisk." << endl;
  cout << " grep TEXT - starts grep in ramdisk" << endl;
  cout << " refresh - checks for changes in files (date, size, name)" << endl;
}