#include "gui.h"

using namespace std;

void gui::print_info() {
  string all_files = utils::exec("find "+Config::ramdisk_path+" -type f | wc -l");
  string df_info = utils::exec("df -h | grep "+Config::ramdisk_path);
  cout << "Indexer informations:\n" << df_info << endl;
  cout << "all files in disk: " << all_files << endl;
}

void gui::print_about() {
  cout << "$ ./indexer COMMAND OPTIONS:" << endl;
  cout << "COMMANDS:" << endl;
  cout << " index - starts indexing directory, arguments:" << endl;
  cout << "         PATH - (required) absolute path to root directory of project to index" << endl;
  cout << " info  - print info about indexer status" << endl;
  cout << " gui   - starts gui" << endl;
}