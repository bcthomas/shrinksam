#include <string>
#include <iostream>
#include <fstream>
#include <locale>
#include <libgen.h>  // for locale stuff (comma-formatted numbers)
#include <cerrno>
#include <cstdio>
#include <getopt.h>

#include "global.h"             // global variables
#include "shrinksam.h"
#include "process_stream.h"

using namespace std;

bool Shrinksam::verbose_flag = false;
char const *Shrinksam::version = "0.1";
char const *Shrinksam::progname;

void show_usage(int status) {
	cerr << Shrinksam::progname << " - shrinks and/or summarizes a SAM file" << endl;
	cerr << endl << "(Written by bct - 2013)" << endl;
	//  cerr << "search method: %s",PULLSEQ_SORTMETHOD);
	cerr << endl << "Usage:" << endl;
	cerr << Shrinksam::progname << " -i <sam infile name> -k <shrunk sam outfile name> -s <summarized sam outfile name>" << endl << endl;
	cerr << Shrinksam::progname << " -k <shrunk sam outfile name> -s <summarized sam outfile name> <stdin>" << endl << endl;
	cerr << Shrinksam::progname << " -k <shrunk sam outfile name> -i <sam infile name>" << endl << endl;
	cerr << Shrinksam::progname << " -s <summarized sam outfile name> -i <sam infile name>" << endl << endl;

	cerr << "  Options:" << endl;
	cerr << "    -i, --input,        Optional - if not given, will read from stdin" << endl;
	cerr << "    -k, --shrunk,       Name for shrunk SAM output file" << endl;
	cerr << "    -s, --summary,      Name for summarized SAM output file" << endl;
	cerr << "    -d, --delete-input, Delete the input sam file (requires -i argument)" << endl;
	cerr << "    -h, --help,         Display this help and exit" << endl;
	cerr << "    -v, --verbose,      Print extra details during the run" << endl;
	cerr << "    --version,          Output version information and exit" << endl;

	exit(status);
}

int main(int argc, char *argv[]) 
{
	locale loc("en_US");
	cout.imbue(loc);

	int c;                            // for getopt
	string input_name;                // input file name
	string shrunk_name, summary_name; // output file names
	bool delete_input = false;        // default to no delete
	bool using_stdin = true;          // default to true

	string line; // for file processing

	Shrinksam::progname = argv[0];

	if (argc < 2) {
		show_usage(EXIT_FAILURE);
	}

	while(1) {
		static struct option long_options[] =
		{
			{"verbose",      no_argument, 0, 'v'},
			{"version",      no_argument, 0, 'V'},
			{"help",         no_argument, 0, 'h'},
			{"delete-input", no_argument, 0, 'd'},
			{"input",        required_argument, 0, 'i'},
			{"shrunk",       required_argument, 0, 'k'},
			{"summary",      required_argument, 0, 's'},
			{0, 0, 0, 0}
		};

		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long (argc, argv, "vVh?di:k:s:", long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
			break;

		switch (c) {
			case 'v':
				Shrinksam::verbose_flag = true;
				break;

			case 'V':
				/* version */
				cerr << "Version is " << Shrinksam::version << endl;
				return EXIT_FAILURE;
				break;

			case 'h':
				show_usage(EXIT_FAILURE);
				break;

			case '?':
				/* getopt_long already printed an error message. */
				break;

			case 'i':
				input_name = optarg;
				using_stdin = false;
				break;

			case 'k':
				shrunk_name = optarg;
				break;

			case 's':
				summary_name = optarg;
				break;

			case 'd':
				delete_input = true;
				break;

			default:
				abort ();
		}
	}

	/* check validity of given argument set */
	if (summary_name.empty() && shrunk_name.empty()) {
		cerr << "Error: one of -k or -s is required" << endl;
		return EXIT_FAILURE;
	}

	if (delete_input) {
		if (input_name.empty()) {
			cerr << "Error: Input file name (-i) is required when using -d option." << endl;
			return EXIT_FAILURE;
		}
	}

	if (Shrinksam::verbose_flag) {
		if (using_stdin)
			cerr << "Input is coming from stdin" << endl;
		else
			cerr << "Input is from " << input_name << endl;
		cerr << "Summarized SAM output will be in " << summary_name << endl;
		cerr << "Shrunk SAM output will be in " << shrunk_name << endl;
		if (delete_input)
			cerr << "Input file will be deleted" << endl;
	}

	ifstream input;
	if (using_stdin)
		input.open("/dev/stdin"); // I know - using this code will only work on unix systems
	else
		input.open(input_name);

	if (!summary_name.empty() && !shrunk_name.empty()) // create both shrunk and summary outputs
		process_both(input,shrunk_name,summary_name);
	else if (!summary_name.empty())                    // just create summary output
		process_summary(input,summary_name);
	else                                               // just create shrunk output
		process_shrunk(input,shrunk_name);

	// delete original file
	if (delete_input) {
		if (Shrinksam::verbose_flag)
			cerr << "Deleting " << input_name << "... " << endl;
		if (remove(input_name.c_str()) != 0)
			cerr << "ERROR: could not delete input file!" << endl;
		else {
			if (Shrinksam::verbose_flag)
				cerr << "Success!" << endl;
		}
	}

	fclose(stderr);
	fclose(stdout);
	fclose(stdin);
	return EXIT_SUCCESS;
}