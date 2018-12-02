#ifndef MAIN_H_
#define MAIN_H_
#include "config.h"
#include "utils.h"
#include "gui.h"
#include <set>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <regex>
#include <streambuf>
#include "boost/filesystem.hpp"
#include <boost/algorithm/string/replace.hpp>

int main(int argc, char *argv[]);
bool file_to_ram(std::string path, std::string filename);
bool indexer(std::string root);

#endif
