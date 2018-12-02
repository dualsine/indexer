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
  int n = 100;
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
      if (c > 126) {
        return true;
      }
    }
  }
  return false;
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

bool utils::create_ramdisk(string name, string size) {
  string df_info = utils::exec(string("sudo ./create_ramdisk.sh "+name+" "+size));
  cout << df_info << endl;
  return true;
}