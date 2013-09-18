#ifndef SPLIT_H
#define SPLIT_H
#include <string>
#include <sstream>
#include <vector>
// update stl with "split" using a delimiter
// Called like thus...
// std::vector<std::string> x = split("one:two::three", ':');
using namespace std;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);

#endif
