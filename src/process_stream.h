#ifndef PROCESS_STREAM_H
#define PROCESS_STREAM_H

#include <iostream>
#include <fstream>
#include <string>

typedef struct _SamData {
	int         read1_samflag;
	std::string sbjct1;
	double      sbjct1_start;
	std::string read1_sequence;
	std::string read1_cigar;

	int         read2_samflag;
	std::string sbjct2;
	double      sbjct2_start;
	std::string read2_sequence;
	std::string read2_cigar;

	friend std::ostream& operator<< (std::ostream &o, const _SamData &i) {
		return o << i.read1_samflag   << "\t" 
				 << i.sbjct1          << "\t"
				 << i.sbjct1_start    << "\t"
				 << i.read1_sequence  << "\t"
				 << i.read1_cigar     << "\t"
				 << i.read2_samflag   << "\t" 
				 << i.sbjct2          << "\t"
				 << i.sbjct2_start    << "\t"
				 << i.read2_sequence  << "\t"
				 << i.read2_cigar ;
//				 << std::endl;
	}

} SamData;

// write just the shrunk file
void process_shrunk(std::ifstream &in, std::ofstream &shr_out);

#endif

