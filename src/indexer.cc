#include <csignal>
#include "indexer.h"

using namespace std;
using boost::filesystem::recursive_directory_iterator;

void signal_callback_handler(int signum) {
  cout << "closing..." << endl;
  exit(0);
}

int main(int argc, char *argv[])
{
  signal(SIGINT, signal_callback_handler);
  signal(SIGKILL, signal_callback_handler);

  if (argc == 1) {
    ui::print_about();
  }
  if (argc == 2) {
    if (string(argv[1]) == "clear") {
      utils::clear_ramdisk(Config::ramdisk_path());
      ui::print_info("1");
    }
    if (string(argv[1]) == "remove") {
      utils::exec("sudo umount "+Config::ramdisk_path());
      utils::exec("sudo rm -rf "+Config::ramdisk_path());
      ui::print_info("1");
    }
    if (string(argv[1]) == "install") {
      utils::create_ramdisk(Config::ramdisk_path(), Config::ramdisk_size());
    }
  }
  if (argc >= 3) {
    if (string(argv[1]) == "index") {
      for (int i=0; i<argc-2; i++)
        if (!indexer(argv[i+2]))
          return -1;
    }
    if (string(argv[1]) == "load") {
      string base_path = string(argv[2]);
      vector<string> saved_paths = utils::load_file(base_path+"/.indexer");
      cout << "paths to load: " << saved_paths.size() << endl;
      for (int i=0; i<saved_paths.size(); i++) {
        string path_to_index = base_path + "/" + saved_paths[i];
        if (!indexer(path_to_index))
          return -1;
      }
    }
  }
  if (argc > 1 && string(argv[1]) == "info") {
    string tree_depth = "4";
    if (argv[2]) tree_depth = argv[2];
    ui::print_info(tree_depth);
  }
  if (argc > 1 && string(argv[1]) == "refresh") {
    string cmd = "find "+Config::ramdisk_path()+" -name '.sha'";
    string sha_files_list = utils::exec(cmd);

    string indexed_paths = utils::replace_string(sha_files_list, Config::ramdisk_path(), "");
    indexed_paths = utils::replace_string(indexed_paths, "/.sha", "");
    vector<string> paths_to_check = utils::split_string(indexed_paths, "\n");

    for (int i=0; i<paths_to_check.size(); i++) {
      string root = paths_to_check[i];
      cout << "currently checking: " << root << endl;
      if (utils::is_new_sha(paths_to_check[i])) {
        cout << "sha has changed - " << root << endl;
        if (!indexer(root))
          return -1;
      }
    }
    cout << "everything is synchronized" << endl;
  }
  if (argc > 1 && string(argv[1]) == "grep") {
    string args = "-lRF";
    string needle = string(argv[2]);

    string cmd = "grep "+args+" \""+needle+"\" "+Config::ramdisk_path()+"/ 2>/dev/null";
    cout << cmd << endl;

    string grep = utils::exec(cmd);
    grep = utils::replace_string(grep, Config::ramdisk_path(), "");

    cout << grep << endl;
  }
  return 0;
}

bool indexer(std::string root) {
  cout << "start indexer in " << root << endl;
  if (!boost::filesystem::is_directory(root)) {
    cout << "no directory specified";
    return false;
  }
  utils::copy_to_ramdisk(root);

  vector<string> binary_files = utils::find_binaries(Config::ramdisk_path()+root);

  cout << "files to delete (binary): " << binary_files.size() << endl;
  utils::mass_delete(binary_files);

  string sha = utils::create_sha(root);
  utils::exec("echo "+sha+" > "+Config::ramdisk_path()+root+"/.sha");

  return true;
}
