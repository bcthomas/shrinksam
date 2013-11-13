#ifndef SAM_H
#define SAM_H

#include <string>
#include <vector>

#include "split.h"

using namespace std;

namespace Shrinksam {
	class Sam {
		public:
			Sam() = default;
			// default for processing a SAM line
			Sam(const std::string &_line);
			std::string find_cigar(const std::string s);
			bool unmapped();
			
			// accessors
			std::string get_qname() const { return qname; };
			int get_flag() const { return flag; }
			std::string get_rname() const { return rname; };
			int get_pos() const { return pos; }
			int get_mapq() const { return mapq; }
			std::string get_cigar() const { return cigar; };
			std::string get_rnext() const { return rnext; };
			int get_pnext() const { return pnext; }
			int get_tlen() const { return tlen; }
			std::string get_seq() const { return seq; };
			std::string get_qual() const { return qual; };
			std::vector<std::string> get_other() const { return other; };

			friend std::ostream& operator<< (std::ostream &o, const Sam &i) {
				o << i.qname   << "\t"
				  << i.flag    << "\t"
				  << i.rname   << "\t"
				  << i.pos     << "\t"
				  << i.mapq    << "\t"
				  << i.cigar   << "\t"
				  << i.rnext   << "\t"
				  << i.pnext   << "\t"
				  << i.tlen    << "\t"
				  << i.seq     << "\t"
				  << i.qual    << "\t";
				for( auto it = i.other.begin(); it != i.other.end(); ++it) {
					if (++it != i.other.end()) { // incr to see what next element is
						--it;                    // next element is not end(), so decr
						o << *it << "\t";
					} else {
						--it;                    // decr to go back to right pos
						o << *it;
					}
				}
				return o;
			}

		private:
			std::string qname;              // query
			int flag = -1;                  // sam flag
			std::string rname;              // sbjct/reference name
			int pos = -1;                   // position on rname
			int mapq = -1;                  // mapping quality
			std::string cigar;              // cigar string
			std::string rnext;              // ref. name of the mate/next read
			int pnext = -1;                 // pos name of the mate/next read
			int tlen = -1;                  // observed template length
			std::string seq;                // read sequence
			std::string qual;               // read quality
			std::vector<std::string> other; // vector of strings with optional fields
	};

	// return true if UNMAPPED, i.e. flag 0x0004 is set
	inline bool Sam::unmapped() {
		return	(flag & BAM_FUNMAP) != 0 ? true : false;
	}

	inline Sam::Sam(const std::string &line) {
		std::vector<std::string> temp = split(line, '\t'); // from split.h
		qname = temp[0];
		flag = std::stoi(temp[1]);
		rname = temp[2];
		pos = std::stoi(temp[3]);
		mapq = std::stoi(temp[4]);
		cigar = temp[5];
		rnext = temp[6];
		pnext = std::stoi(temp[7]);
		tlen = std::stoi(temp[8]);
		seq = temp[9];
		qual = temp[10];
		// all the rest in the vector
		for(auto it = temp.begin() + 11; it < temp.end(); ++it)
			other.push_back(*it); 
	}

	// Sam::find_cigar
	// locates a code in the other vector given a string, e.g. "MD*"
	std::string Sam::find_cigar(const std::string s) {
		for(std::vector<std::string>::iterator it = other.begin(); it != other.end(); ++it) {
			if ((*it).find(s) == 0)
				return(*it);
		}
		return("");
	}
}
#endif
