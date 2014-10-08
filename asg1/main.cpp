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

/*Returns a pipe to a preprocessor which has been fed ocfname*/
FILE* preprocess(string ocfname){
	//run c preprocessor
	string cmd = "cpp ";
	cmd += ocfname;

	//read in code and run preprocessor
	return(popen(cmd.c_str(), "r"));
	
}

void fill_string_table(FILE* pipe){
	//read preprocessed data into hashtable
	char input_buffer[BUFSIZE];
	char* saveptr;
	int i = 0;
	while(fgets(input_buffer, BUFSIZE, pipe) != NULL){
		//throw away first three lines of preprocessor, it's not part of file
		if (i++ < 3)
			continue;
		char* token = strtok_r(input_buffer, " \t\n", &saveptr);
		if (token != NULL){
			intern_stringset(strdup(token));
			printf("%s\n", token);	
		}
		while(1){
			token = strtok_r(NULL, " \t\n", &saveptr);
			if (token == NULL)
				break;
			else{
				printf("%s\n", token);	
				intern_stringset(strdup(token));
			}
			
		}
	}
	FILE* f = fopen("output", "a+");
	dump_stringset(f);

}

int main (int argc, char **argv) {
	int c;
	bool lflag = false;
	bool yflag = false;
	string dflag;
	string atflag;
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
	FILE* preproc_pipe = preprocess(ocfname);	
	fill_string_table(preproc_pipe);

//   for (int i = 1; i < argc; ++i) {
//      const string* str = intern_stringset (argv[i]);
//      printf ("intern (\"%s\") returned %p->\"%s\"\n",
//              argv[i], str->c_str(), str->c_str());
//   }
//   dump_stringset (stdout);
   return EXIT_SUCCESS;

}





