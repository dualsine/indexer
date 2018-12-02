#include "indexer.h"

using namespace std;
using boost::filesystem::recursive_directory_iterator;

int main(int argc, char *argv[])
{
//  connect_psql();
//  open_sentences();

  create_ramdisk("/media/indexer_ramdisk", "4096M");

  if (argc == 1) {
    cout << "$ indexer OPTION ARGUMENTS" << endl;
    cout << "OPTIONS:" << endl;
    cout << "info     - get info about current status" << endl;
    cout << "truncate - deletee everything (database and sentences file)" << endl;
    cout << "index - start indexing path, must specify path as argument" << endl;
  }

  if (argc==2 && string(argv[1]) == "info") {
    cout << "indexed-------------:" << endl;
    cout << "sentences: " << sentences.size() << endl;
    cout << "documents: " << count_rows("documents") << endl;
  }

  if (argc==2 && string(argv[1]) == "truncate") {
    truncate_table("documents");
    sentences.clear();
    save_sentences();
  }

  if (argc == 3 && string(argv[1]) == "index") {
    if (!indexer(string(argv[2]))) {
      return -1;
    } else {
      save_sentences();
    }
  }

  return 0;
}

bool create_ramdisk(string name, string size) {
  string df_info = exec(string("./create_ramdisk.sh "+name+" "+size));
  cout << df_info << endl;
  return true;
}

std::string escape_sql(string str) {
  boost::replace_all(str, "'", "''");
  return str;
}

void truncate_table(string tables) {
  string cmd = string("TRUNCATE "+tables+";");
  psql_res = PQexec(conn, cmd.c_str());
  if (PQresultStatus(psql_res) != PGRES_COMMAND_OK) {
    cout << "Truncate failed::: " << PQresultErrorMessage(psql_res)
         << endl << cmd << endl;
  }
}

int insert_document(string filename, string path) {
  filename = escape_sql(filename);
  path = escape_sql(path);
  string cmd = string("insert into documents(filename, path) values('"+filename+"', '"+path+"'); SELECT currval(pg_get_serial_sequence('documents','id'));");
  psql_res = PQexec(conn, cmd.c_str());
  if (PQresultStatus(psql_res) != PGRES_TUPLES_OK) { // PGRES_COMMAND_OK
    cout << "Insert document into table failed::: " << PQresultErrorMessage(psql_res) << endl << cmd << endl;
    return -1;
  }
  int doc_id = stoi(string(PQgetvalue(psql_res, 0, 0)));
  PQclear(psql_res);
  return doc_id;
}

bool connect_psql() {
  const char conninfo[] =
          "postgresql://chromium_indexer:orange3@localhost?port=5432&dbname=chromium_indexer";
  conn = PQconnectdb(conninfo);
  if (PQstatus(conn) != CONNECTION_OK) {
    std::cout << "Connection to database failed: " << PQerrorMessage(conn)
              << endl;
    PQfinish(conn);
    return false;
  }
  return true;
}


void split_file_to_sentences(string fullpath, int doc_id) {
  if (is_binary(fullpath)) {
    string cmd = string("update documents set is_binary=true where id=") + to_string(doc_id);
    psql_res = PQexec(conn, cmd.c_str());
    if (PQresultStatus(psql_res) != PGRES_COMMAND_OK) { // PGRES_COMMAND_OK
      cout << "Update document.is_binary failed::: " << PQresultErrorMessage(psql_res) << endl << cmd << endl;
    }
  } else {
    string line;
    ifstream document(fullpath);
    if (document.is_open()) {
      while (getline(document, line)) {
        vector<string> linesplitted = split_string(line, "[^.,a-zA-Z0-9_+\\/\\-!@#$%\\^&*\"'()\\]\\[\\\\|]");

        for (int i = 0; i < linesplitted.size(); i++) {
          sentences.insert(linesplitted[i]);
        }
      }
    }
  }
}

int count_rows(string tablename) {
  string cmd = string("select count(*) from "+tablename+";");
  psql_res = PQexec(conn, cmd.c_str());
  if (PQresultStatus(psql_res) != PGRES_TUPLES_OK) { // PGRES_COMMAND_OK
    cout << "count_rows failed::: " << PQresultErrorMessage(psql_res) << endl << cmd << endl;
    return -1;
  }
  int documents_nb = stoi(string(PQgetvalue(psql_res, 0, 0)));
  return documents_nb;
}

vector<string> split_string(const string& stringToSplit, const string& regexPattern)
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

bool is_binary(string fullpath) {
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

std::string exec(string cmd) {
  array<char, 128> buffer;
  string result;
  unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
  if (!pipe) throw std::runtime_error("popen() failed!");
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
      result += buffer.data();
  }
  return result;
}

bool save_sentences() {
  ofstream output_file(sentences_path);
  ostream_iterator<std::string> output_iterator(output_file, "\n");
  copy(sentences.begin(), sentences.end(), output_iterator);
  output_file.close();
  return true;
}

bool open_sentences() {
  string line;
  ifstream file(sentences_path);
  if (file.is_open()) {
    while (getline(file, line)) {
      sentences.insert(line);
    }
    file.close();
  } else {
    cout << "sentences data not found" << endl;
    return false;
  }
  return true;
}

bool indexer(std::string root) {
  long int last_time = static_cast<long int>(time(NULL));
  if (!boost::filesystem::is_directory(root)) {
    cout << "no directory specified";
    return false;
  }
  string cmd("find "+root+" -type f | wc -l");
  int all_files_nb = stoi(exec(cmd.c_str()));
  int current_file_nb = 0;
  for ( recursive_directory_iterator end, dir(root); dir != end; dir++ ) {
    if (boost::filesystem::is_regular_file(dir->path())) {
      string filename = dir->path().filename().string();
      string path = dir->path().parent_path().string();
      string fullpath = dir->path().string();
      int doc_id = insert_document(filename, path);

      if (doc_id < 0) {
        cout << "error with doc_id";
        return false;
      }

      split_file_to_sentences(fullpath, doc_id);

      current_file_nb++;
      if (current_file_nb%100 == 0) {
        long int current_time = static_cast<long int>(time(NULL));
        int time_for_hundred = current_time - last_time;
        last_time = static_cast<long int>(time(NULL));
        cout << "progress: " << current_file_nb << "/" << all_files_nb << " " << current_file_nb*100/all_files_nb
              << "%" << (all_files_nb-current_file_nb)/100*time_for_hundred << "s" << endl;
      }
    }
    else if (boost::filesystem::is_directory(dir->path())) {
    }
  }
  return true;
}
