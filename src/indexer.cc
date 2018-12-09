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
    if (string(argv[1]) == "deletebin" || string(argv[1]) == "deletebins") {
      remove_bin_files("");
    }
    if (string(argv[1]) == "clear") {
      utils::clear_ramdisk(Config::ramdisk_path());
      ui::print_info("1");
    }
    if (string(argv[1]) == "remove") {
      if (getuid()) {
        cout << "please run as sudo" << endl;
        return 0;
      }
      utils::exec("sudo umount "+Config::ramdisk_path());
      utils::exec("sudo rm -rf "+Config::ramdisk_path());
      ui::print_info("1");
    }
    if (string(argv[1]) == "install") {
      if (getuid()) {
        cout << "please run as sudo" << endl;
        return 0;
      }
      utils::create_ramdisk(Config::ramdisk_path(), Config::ramdisk_size());
    }
  }
  if (argc >= 3) {
    if (string(argv[1]) == "index") {
      for (int i=0; i<argc-2; i++)
        if (!indexer(argv[i+2]))
          return -1;

      cout << "everything loaded" << endl;
      ui::print_info("4");
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
      cout << "everything loaded" << endl;
      ui::print_info("4");
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
    string needle = string(argv[2]);

    string cmd = "grep -RFC 5 "+needle+" /media/indexer_ramdisk > /tmp/indexer_grep && cat /tmp/indexer_grep | sed -i '/^.\\{2048\\}./d' | sed -i 's/\\/media\\/indexer_ramdisk\\/electron_base\\/electron-gn//g' /tmp/indexer_grep && subl /tmp/indexer_grep";
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
  utils::exec("rm -f "+Config::ramdisk_path()+root+"/.sha");

  pthread_t thread;
  pthread_create(&thread, NULL, &remove_bin_files_thread, &root);

  utils::copy_to_ramdisk(root);

  string sha = utils::create_sha(root);
  utils::exec("echo "+sha+" > "+Config::ramdisk_path()+root+"/.sha");

  pthread_join(thread, NULL);

  return true;
}

void remove_bin_files(string root) {
  vector<string> binary_files = utils::find_binaries(Config::ramdisk_path() + root);

  utils::mass_delete(binary_files);
}

void* remove_bin_files_thread(void *root) {
  string *root_path = (string*) root;

  boost::filesystem::path boost_path(*root_path);
  string parent_path = boost_path.parent_path().parent_path().string();

  utils::exec("mkdir -p "+Config::ramdisk_path()+(parent_path)); // if starts before anything was copied
  utils::exec("chmod -R 777 "+Config::ramdisk_path()+"/*");
  while ( utils::load_sha(*root_path).length() < 40 ) {
    remove_bin_files(*root_path);

    sleep(5);
  }
  return 0;
}