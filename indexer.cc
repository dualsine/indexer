#include "indexer.h"

using namespace std;
using boost::filesystem::recursive_directory_iterator;

int main(int argc, char *argv[])
{
  utils::create_ramdisk(Config::ramdisk_path, Config::ramdisk_size);

  if (argc == 1) {
    gui::print_about();
  }
  if (argc == 2) {
    if (string(argv[1]) == "info") {
      gui::print_info();
    }
    if (string(argv[1]) == "gui") {
      
    }
  }
  if (argc == 3) {
    if (string(argv[1]) == "index")
      if (!indexer(argv[2]))
        return -1;
  }
  return 0;
}

bool file_to_ram(string path, string filename) {
  string fullpath = path + "/" + filename;
  if (utils::is_binary(fullpath)) {
    // omit this file
  } else {
    string line;
    set<string> unique;
    ifstream document(fullpath);
    if (document.is_open()) {
      while (getline(document, line)) {
        vector<string> line_splitted = utils::split_string(line, "[^.,a-zA-Z0-9_+\\/\\-!@#$%\\^&*\"'()\\]\\[\\\\|]");

        for (int i = 0; i < line_splitted.size(); i++) {
          unique.insert(line_splitted[i]);
        }
      }
      string out_path = Config::ramdisk_path + fullpath;
      utils::exec("mkdir -p "+Config::ramdisk_path+path);
      ofstream output_file(out_path);
      if (output_file.is_open()) {
          ostream_iterator<string> output_iterator(output_file, " ");
          copy(unique.begin(), unique.end(), output_iterator);
      } else {
          cout << "error saving file to ramdisk:" << endl << out_path << endl;
          return false;
      }
    } else {
      return false;
    }
  }
  return true;
}

bool indexer(std::string root) {
  cout << "start indexer in " << root << endl;
  long int last_time = static_cast<long int>(time(NULL));
  if (!boost::filesystem::is_directory(root)) {
    cout << "no directory specified";
    return false;
  }
  string cmd("find "+root+" -type f | wc -l");
  int all_files_nb = stoi(utils::exec(cmd.c_str()));
  int current_file_nb = 0;
  for ( recursive_directory_iterator end, dir(root); dir != end; dir++ ) {
    if (boost::filesystem::is_regular_file(dir->path())) {
      string filename = dir->path().filename().string();
      string path = dir->path().parent_path().string();
      string fullpath = dir->path().string();

      if (!file_to_ram(path, filename)) {
          return false;
      }

      current_file_nb++;
      if (current_file_nb%50 == 0) {
        long int current_time = static_cast<long int>(time(NULL));
        int time_diff = current_time - last_time;
        cout << " " << current_file_nb << "/" << all_files_nb << "  "<< current_file_nb*100/all_files_nb<<"%" <<endl;
      }
    }
    else if (boost::filesystem::is_directory(dir->path())) {
    }
  }
  return true;
}
