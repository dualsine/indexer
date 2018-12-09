#ifndef MAIN_H_
#define MAIN_H_
#include "config.h"
#include "utils.h"
#include "ui.h"
#include <set>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <regex>
#include <streambuf>
#include "boost/filesystem.hpp"
#include <boost/algorithm/string/replace.hpp>
#include <pthread.h>
#include <unistd.h>

int main(int argc, char *argv[]);
bool file_to_ram(std::string fullpath, std::string path);
bool indexer(std::string root);
void* remove_bin_files_thread(void *root);
void remove_bin_files(std::string root);

#endif
