#include "utils.h"

using namespace std;

std::string utils::exec(string cmd) {
  array<char, 128> buffer;
  string result;
  unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
  if (!pipe) throw std::runtime_error("popen() failed!");
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return result;
}

bool utils::is_binary(string fullpath) {
  std::streampos fsize = 0;
  int n = 500;
  ifstream f;
  f.open(fullpath, ios::in | ios::binary);
  if (f.is_open()) {
    // determine max n size
    fsize = f.tellg();
    f.seekg( 0, std::ios::end );
    fsize = (f.tellg() - fsize);
    if (n > (int)fsize) n = (int)fsize;
    f.seekg( 0, ios::beg);
    // read n chars from file
    char buffer[n];
    f.read(buffer, n);
    // return true if ASCII char > 126
    for (int i=0; i<n; i++) {
      unsigned int c = (unsigned int) buffer[i];
      if (c > 0 && c < 9) {
        return true;
      }
    }
  }
  return false;
}

bool utils::create_ramdisk(string name, string size) {
  utils::exec("echo \"#!/bin/sh\" > /tmp/create_ramdisk");
  utils::exec("echo \"sudo mkdir -p "+name+" > /dev/null\" >> /tmp/create_ramdisk");
  utils::exec("echo \"sudo chmod -R 777 "+name+" > /dev/null\" >> /tmp/create_ramdisk");
  utils::exec("echo \"df -h | grep "+name+" > /dev/null\" >> /tmp/create_ramdisk");
  utils::exec("echo \"sudo mount -t tmpfs -o size="+size+" tmpfs "+name+"\" >> /tmp/create_ramdisk");
  utils::exec("chmod +x /tmp/create_ramdisk");
  utils::exec(string("sudo /tmp/create_ramdisk"));

  string df_info = utils::exec("df -h");
  df_info = utils::replace_string(df_info, Config::ramdisk_path(), Config::ramdisk_path()+" <<< NEW RAMDISK");
  cout << df_info << endl;
  return true;
}

bool utils::clear_ramdisk(string path) {
  if (path.length() < 5) {
    cout << "path in clear_ramdisk below 5 chars" << endl << path << endl;
    return false;
  }
  utils::exec("rm -rf "+path+"/*");
  return true;
}

string utils::replace_string(std::string subject, const std::string& search,
                          const std::string& replace) {
  size_t pos = 0;
  while ((pos = subject.find(search, pos)) != std::string::npos) {
    subject.replace(pos, search.length(), replace);
    pos += replace.length();
  }
  return subject;
}

long int utils::get_time() {
  struct timeval tp;
  gettimeofday(&tp, NULL);
  return tp.tv_sec * 1000 + tp.tv_usec / 1000;
}

vector<string> utils::load_file(string path) {
  ifstream f(path);
  if (!f.is_open()) {
    cout << "utils::load_file -- error opening file " << path << endl;
  }
  istream_iterator<string> start(f), end;
  return vector<string>(start, end);
}

void utils::mass_delete(vector<string> files) {
  string multiple_paths("");
  cout << "deleting " << files.size() << " binary files" << endl;
  for (int d=0; d<files.size(); d++) {
    multiple_paths += " "+files[d];
    if (d%20 == 0) {
      utils::exec("rm -f "+multiple_paths+" 2>/dev/null");
      multiple_paths = "";
    }
  }
  utils::exec("rm -f "+multiple_paths+" 2>/dev/null");
}

void utils::copy_to_ramdisk(std::string root) {
  boost::filesystem::path boost_path(root);
  string parent_path = boost_path.parent_path().parent_path().string();
  utils::exec("mkdir -p "+Config::ramdisk_path()+parent_path);
  string ramdisk_destination = Config::ramdisk_path()+root;
  string cmd = "cp -rfp "+root+" "+ramdisk_destination+" > /dev/null 2>&1";
  utils::exec(cmd);
}

vector<string> utils::find_binaries(string ramdisk_destination) {
  vector<string> files_to_delete;
  for ( boost::filesystem::recursive_directory_iterator end, dir( ramdisk_destination );
        dir != end; ++dir ) {
    string file_path = dir->path().string();
    if (boost::filesystem::is_regular_file(file_path)) {
      if (utils::is_binary(file_path)) {
        files_to_delete.push_back(file_path);
      }
    }
  }
  return files_to_delete;
}

vector<string> utils::split_string(const string& stringToSplit, const string& regexPattern)
{
  string empty_str = "";
  vector<string> result;
  const regex rgx(regexPattern);
  sregex_token_iterator iter(stringToSplit.begin(), stringToSplit.end(), rgx, -1);
  for (sregex_token_iterator end; iter != end; ++iter)
  {
    if (iter->str().size() > 1)
      result.push_back(iter->str());
  }
  return result;
}

string utils::create_sha(string root) {
  string cmd = "find "+root+" -type f -printf \"%s %p %TY-%Tm-%Td %TH:%TM:%TS %Tz\n\" | sha1sum";
  string sha = utils::exec(cmd);
  sha = utils::replace_string(sha, "\n", "");
  sha = utils::replace_string(sha, "-", "");
  sha = utils::replace_string(sha, " ", "");
  return sha;
}

string utils::load_sha(string root) {
  string sha = utils::exec("cat "+Config::ramdisk_path()+root+"/.sha 2>/dev/null");
  boost::trim(sha);
  return sha;
}

bool utils::is_new_sha(string root) {
  string ram_sha = utils::load_sha(root);
  string root_sha = utils::create_sha(root);
  boost::trim(root_sha);

  return root_sha != ram_sha;
}