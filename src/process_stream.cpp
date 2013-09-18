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

// write both files, shrunk and summary
void process_both(std::ifstream &in, string &shrunk_name, string &summary_name) {
    ofstream shr_out(shrunk_name);  // shrunk output file
	ofstream sum_out(summary_name); // summary output file
	string line;
	map<string, SamData> read2data;

	int total_reads = 0;
	int output_reads = 0;
	int discarded_reads = 0;
	if (in.is_open()) {
		while(getline(in,line)) {
			if (line[0] == '@') {           // header line
				shr_out << line << endl;
			} else {
				++total_reads;
				std::vector<std::string> temp = split(line, '\t');        // split on tabs (from split.h)
				const int flag = atoi(temp[1].c_str());
				if (flag & BAM_FPAIRED) {                                  // only work with PAIRED reads
					if ( (flag & BAM_FUNMAP) && (flag & BAM_FMUNMAP) ) {  // make sure at least one pair mapped
						// both mates of the pair are unmapped, so skip it
						++discarded_reads;
					} else {
						shr_out << line << endl;                           // print line to shrunk output
						++output_reads;

						string name = parse_read_name(temp[0]);            // grab read name

						if (flag & BAM_FREAD1) {
							shared_ptr<SamData> d = make_shared<SamData>();    // create a new shared ptr to struct
							// read 1
							d->read1_samflag = flag;
							d->sbjct1 = temp[2];
							d->sbjct1_start = atoi(temp[3].c_str());
							d->read1_sequence = temp[9];
							d->read1_cigar = find_cigar(temp);
							read2data.insert(make_pair(name,*d));
						} else {
							// read 2
							auto it = read2data.find(name);
							if (it != read2data.end()) {
								it->second.read2_samflag = flag;
								it->second.sbjct2 = temp[2];
								it->second.sbjct2_start = atoi(temp[3].c_str());
								it->second.read2_sequence = temp[9];
								it->second.read2_cigar = find_cigar(temp);
							}
						}
					}
				} else {
					++discarded_reads;
				}
			}
		} // end of while() loop
	} // end of if (in.is_open())

	// output the summary file
	auto it = read2data.begin();
	while(it != read2data.end()) {
		sum_out << it->first << "\t" << it->second << endl;
		++it;
	}

	if (Shrinksam::verbose_flag) {
		float percent = (float) output_reads / (float) total_reads;
		cerr << "--- Shrink summary ---" << endl;
		cerr << "Total Reads: " << total_reads << endl;
		cerr << "Discarded Reads: " << discarded_reads << endl;
		cerr << "Output Reads: " << output_reads << endl;
		cerr << 1.0 - percent << " percent shrunk" << endl;
	}

    sum_out.close();
    shr_out.close();
}

// write just the summary file
void process_summary(std::ifstream &in, string &ofilename) {
	ofstream sum_out(ofilename); // summary output file
	string line;
	map<string, SamData> read2data;

	if (in.is_open()) {
		while(getline(in,line)) {
			if (line[0] != '@') {           // skip headers
				std::vector<std::string> temp = split(line, '\t');        // split on tabs (from split.h)
				const int flag = atoi(temp[1].c_str());
				if (flag & BAM_FPAIRED) {                                  // only work with PAIRED reads
					if ( (flag & BAM_FUNMAP) && (flag & BAM_FMUNMAP) ) {  // make sure at least one pair mapped
						// both mates of the pair are unmapped, so skip it
					} else {
						string name = parse_read_name(temp[0]);            // grab read name

						if (flag & BAM_FREAD1) {
							shared_ptr<SamData> d = make_shared<SamData>();    // create a new shared ptr to struct
							// read 1
							d->read1_samflag = flag;
							d->sbjct1 = temp[2];
							d->sbjct1_start = atoi(temp[3].c_str());
							d->read1_sequence = temp[9];
							d->read1_cigar = find_cigar(temp);
							read2data.insert(make_pair(name,*d));
						} else {
							// read 2
							auto it = read2data.find(name);
							if (it != read2data.end()) {
								it->second.read2_samflag = flag;
								it->second.sbjct2 = temp[2];
								it->second.sbjct2_start = atoi(temp[3].c_str());
								it->second.read2_sequence = temp[9];
								it->second.read2_cigar = find_cigar(temp);
							}
						}
					}
				}
			}
		} // end of while() loop
	}

	auto it = read2data.begin();
	while(it != read2data.end()) {
		sum_out << it->first << "\t" << it->second << endl;
		++it;
	}

    sum_out.close();
}

// write just the shrunk file
void process_shrunk(std::ifstream &in, string &name) {
	ofstream shr_out(name); // shrunk output file
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
