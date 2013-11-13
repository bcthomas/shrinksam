shrinksam - shrinks a SAM file while maintaining mate pair information

The shrinksam program is a C++ program that will shrink a SAM file by
removing sequences that failed to map to the reference.  Most useful
after read mapping, shrinksam will keep reads where both members of
the mate pair map, and it will keep both members of the mate pair
where one or the other mate pair mapped but not the partner.

This is extrememly useful in shrinking SAM output files, and saving
storage space.

shrinksam - shrinks a SAM file while maintaining mate pair information

(Written by bct - 2013)

Usage:
shrinksam -i <sam infile name> -k <shrunk sam outfile name>

shrinksam -k <shrunk sam outfile name> <stdin>

shrinksam < <stdin> > out

  Options:
    -i, --input,        Optional - if not given, will read from stdin
    -k, --shrunk,       Name for shrunk SAM output file
    -d, --delete-input, Delete the input sam file (requires -i argument)
    -u, --unpaired,     SAM file was from an unpaired mapping
    -h, --help,         Display this help and exit
    -v, --verbose,      Print extra details during the run
    --version,          Output version information and exit

