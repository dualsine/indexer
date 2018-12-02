#ifndef MAIN_H_
#define MAIN_H_
#include <set>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <regex>
#include <streambuf>
#include "boost/filesystem.hpp"
#include <postgresql/libpq-fe.h>
#include <boost/algorithm/string/replace.hpp>

std::set<std::string> sentences;
std::string sentences_path = "./indexer.data";

PGconn *conn;
PGresult *psql_res = NULL;

int main(int argc, char *argv[]);
std::vector<std::string> split_string(const std::string& stringToSplit, const std::string& regexPattern);
bool is_binary(const std::string path);
//void split_file(std::string path, int doc_id);
bool connect_psql();
int insert_document(std::string filename, std::string path);
void truncate_table(std::string tables);
void split_file_to_sentences(std::string fullpath, int doc_id);
std::string exec(std::string cmd);
bool save_sentences();
bool open_sentences();
bool indexer(std::string root);
int count_rows(std::string tablename);
bool create_ramdisk(std::string name, std::string size);

#endif
