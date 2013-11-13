#include <string>
#include <iostream>
#include <fstream>
#include <cerrno>
#include <vector>
#include <unordered_map>
#include <memory>

#include "process_stream.h"
#include "split.h"
#include "global.h"
#include "sam.h"

using namespace std;
using namespace Shrinksam;

// parse_read_name
// checks for "/1" or "/2" and returns the truncated name
std::string parse_read_name(const string &oldname) {
	if ( (oldname.rfind("/1") == string::npos) && (oldname.rfind("/2") == string::npos) )
		return(oldname);
	else {
		std::string::size_type len = oldname.length();
		return(oldname.substr(0,len-2));
	}
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
				Sam *s = new Sam(line);
				if (s->get_flag() & BAM_FPAIRED) {
					if ((s->get_flag() & BAM_FUNMAP) && (s->get_flag() & BAM_FMUNMAP)) {
						// both mates of the pair are unmapped
						++discarded_reads;
					} else {
						shr_out << *s << endl;
						++output_reads;
					}
				} else {
					++discarded_reads;
				}
				delete s;
			}
		} // end of while() loop
	}
	
	if (Shrinksam::verbose_flag) {
		float percent = (1.0 - ((float) output_reads / (float) total_reads)) * 100;
		cerr << "--- Shrink summary ---" << endl;
		cerr << "Total Reads: " << total_reads << endl;
		cerr << "Discarded Reads: " << discarded_reads << endl;
		cerr << "Output Reads: " << output_reads << endl;
		cerr << percent << "% shrunk" << endl;
	}

   // close files
    shr_out.close();
}

// write shrunk file from unpaired mapping
void process_shrunk_unpaired(std::ifstream &in, std::ofstream &shr_out) {
	string line;
	unordered_map<std::string, Shrinksam::Sam*> reads; // string -> Sam object (ptr)

	int total_reads = 0;
	int output_reads = 0;
	int discarded_reads = 0;
	if (in.is_open()) {
		while(getline(in,line)) {
			if (line[0] == '@') {           // header so just write to output
				shr_out << line << endl;
			} else {
				++total_reads;
				Sam *s = new Sam(line);

				if ( (s->get_qname().rfind("/1") != std::string::npos) ||
					 (s->get_qname().rfind("/2") != std::string::npos)) {
					// found a "/1" read or a "/2" read
					string read_base = parse_read_name(s->get_qname());
					auto it = reads.find(read_base);
					if (it == reads.end()) { // not in reads
						reads[read_base] = s; // store the read for later
					} else { // have already seen mate, so print this pair and remove from reads
						if (reads[read_base]->unmapped() && s->unmapped()) {
							discarded_reads += 2;
						} else { // one or the other read mapped, so keep both
							output_reads += 2;
							shr_out << *reads[read_base] << endl;
							shr_out << *s << endl;
						}
						delete reads[read_base]; // free memory for stored Sam object
						delete s;                // free memory for current Sam object
						reads.erase(read_base);  // remove key
					}
				} else {
					// Found nothing to pair on! Only include if sam
					// flag does NOT have bit 4 set
					if (!s->unmapped()) {
						++output_reads;
						shr_out << *s << endl;
						delete s; // free memory for current Sam object
					} else {
						delete s; // free memory for current Sam object
						++discarded_reads;
					}
			    }
			}
		} // end of while() loop
	}
	
	if (Shrinksam::verbose_flag) {
		float percent = (1.0 - ((float) output_reads / (float) total_reads)) * 100;
		cerr << "--- Shrink summary ---" << endl;
		cerr << "Total Reads: " << total_reads << endl;
		cerr << "Discarded Reads: " << discarded_reads << endl;
		cerr << "Output Reads: " << output_reads << endl;
		cerr << percent << "% shrunk" << endl;
	}

    // close files
    shr_out.close();
	// delete anything left in reads map
	for(auto it = reads.begin(); it != reads.end(); ++it)
		delete it->second;
	reads.empty();
}
