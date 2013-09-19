#include <string>
#include <iostream>
#include <fstream>
#include <cerrno>
#include <vector>
#include <map>
#include <memory>

#include "process_stream.h"
#include "split.h"
#include "global.h"

using namespace std;

// parse_read_name
// checks for "/1" or "/2" and returns the truncated name
std::string parse_read_name(string &oldname) {
	if ( (oldname.rfind("/1") == string::npos) && (oldname.rfind("/2") == string::npos) )
		return(oldname);
	else {
		std::string::size_type len = oldname.length();
		return(oldname.substr(0,len-2));
	}
}

// find_cigar
// locates the "MD*" string in the split
std::string find_cigar(std::vector<std::string> &t) {
	for(std::vector<std::string>::reverse_iterator it = t.rbegin(); it != t.rend(); ++it) {
		if ((*it).find("MD") == 0)
			return(*it);
	}
	return("");
}

// write just the shrunk file
void process_shrunk(std::ifstream &in, std::ofstream &shr_out) {
	string line;

	int total_reads = 0;
	int output_reads = 0;
	int discarded_reads = 0;
	if (in.is_open()) {
		while(getline(in,line)) {
			if (line[0] == '@') {           // header so just write to output
				shr_out << line << endl;
			} else {
				++total_reads;
				std::vector<std::string> temp = split(line, '\t'); // from split.h
				const int flag = atoi(temp[1].c_str());
				if (flag & BAM_FPAIRED) {
					if ((flag & BAM_FUNMAP) && (flag & BAM_FMUNMAP)) {
						// both mates of the pair are unmapped
						++discarded_reads;
					} else {
						shr_out << line << endl;
						++output_reads;
					}
				} else {
					// unpaired read - discarded
					++discarded_reads;
				}
			}
		} // end of while() loop
	}
	
	if (Shrinksam::verbose_flag) {
		float percent = (float) output_reads / (float) total_reads;
		cerr << "--- Shrink summary ---" << endl;
		cerr << "Total Reads: " << total_reads << endl;
		cerr << "Discarded Reads: " << discarded_reads << endl;
		cerr << "Output Reads: " << output_reads << endl;
		cerr << percent << " percent shrunk" << endl;
	}

   // close files
    shr_out.close();
}
