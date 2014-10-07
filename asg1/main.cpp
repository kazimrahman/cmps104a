// $Id: main.cpp,v 1.1 2014-10-03 18:45:23-07 - - $

#include <string>
using namespace std;

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "stringset.h"

#define BUFSIZE 4096


int main (int argc, char **argv) {
	int c;
	bool lflag = false;
	bool yflag = false;
	string dflag;
	string atflag;
	FILE* pipe;
	string ocfname;

	while((c = getopt(argc-1, argv, "lyd:@:")) != -1)
		switch(c){
			case 'l':
				lflag = true;
				break;
			case 'y':
				yflag = true;
				break;
			case 'd':
				dflag = optarg;
				break;
			case '@':
				atflag = optarg;
				break;
			default:
				fprintf(stderr, "Unknown option `%s'\n", optarg);
		}
	for(int i=0; i<argc; i++){
		if (strstr(argv[i], "oc") != NULL){
			ocfname = string(argv[i]);
		}
	}
	if (fopen(ocfname.c_str(), "r") == NULL){
		fprintf(stderr, "File not found\n");
		exit(1);
	}
	
	//run c preprocessor
	string cmd = "cpp ";
	cmd += ocfname;

	//read in code and run preprocessor
	pipe = popen(cmd.c_str(), "r");
	//read preprocessed data into hashtable
	char input_buffer[BUFSIZE];
	while(fgets(input_buffer, BUFSIZE, pipe) != NULL){
		printf("%s\n", input_buffer)	;
	}


//   for (int i = 1; i < argc; ++i) {
//      const string* str = intern_stringset (argv[i]);
//      printf ("intern (\"%s\") returned %p->\"%s\"\n",
//              argv[i], str->c_str(), str->c_str());
//   }
//   dump_stringset (stdout);
   return EXIT_SUCCESS;

}



