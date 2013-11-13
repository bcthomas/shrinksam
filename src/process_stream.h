#ifndef PROCESS_STREAM_H
#define PROCESS_STREAM_H

#include <fstream>

// write the shrunk file
void process_shrunk(std::ifstream &in, std::ofstream &shr_out);
// write the shrunk file from unpaired input
void process_shrunk_unpaired(std::ifstream &in, std::ofstream &shr_out);

#endif

