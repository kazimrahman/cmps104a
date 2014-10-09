// $Id: main.cpp,v 1.1 2014-10-03 18:45:23-07 - - $

#include <string>
using namespace std;

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <vector>

#include "stringset.h"

#define BUFSIZE 4096

/*Returns a pipe to a preprocessor which has been fed ocfname*/
FILE* preprocess(string ocfname, string options){
	//run c preprocessor
	string cmd = "cpp ";
	cmd += ocfname + " ";
	if (!options.empty())
		cmd += "-D" + options;

	//read in code and run preprocessor
	return(popen(cmd.c_str(), "r"));
	
}

void fill_string_table(FILE* pipe){
	//read preprocessed data into hashtable
	char input_buffer[BUFSIZE];
	char* saveptr;
//	int i = 0;
	while(fgets(input_buffer, BUFSIZE, pipe) != NULL){
		//throw away first three lines of preprocessor, it's not part of file
//		if (i++ < 3)
//			continue;
		char* token = strtok_r(input_buffer, " \t\n", &saveptr);
		if (token != NULL){
			intern_stringset(strdup(token));
		}
		while(1){
			token = strtok_r(NULL, " \t\n", &saveptr);
			if (token == NULL)
				break;
			else{
				intern_stringset(strdup(token));
			}
			
		}
	}
}

int main (int argc, char **argv) {
	int c;
	bool lflag = false;
	bool yflag = false;
	string dflag;
	string atflag;
	vector<string> ocfnames;

	while((c = getopt(argc-1, argv, "lyD:@:")) != -1)
		switch(c){
			case 'l':
				lflag = true;
				break;
			case 'y':
				yflag = true;
				break;
			case 'D':
				dflag = optarg;
				break;
			case '@':
				atflag = optarg;
				break;
			default:
				fprintf(stderr, "Unknown option `%s'\n", optarg);
		}
	char* ocfname = argv[argc-1];
	if (!strstr(ocfname, ".oc")){
		fprintf(stderr, "File must be an .oc file\n");
	}
	FILE* tmp = fopen(ocfname, "r");
	if (tmp == NULL){
		fprintf(stderr, "File not found\n");
		exit(1);
	}
	fclose(tmp);
	FILE* preproc_pipe = preprocess(ocfname, dflag);	
	fill_string_table(preproc_pipe);
	fclose(preproc_pipe);
	char out_fname[BUFSIZE];
	strcpy(out_fname, basename(ocfname));
	auto dot_index = strrchr(out_fname, '.');
	*dot_index = '\0';
	strcat(out_fname, ".str");
	printf("fname: %s", out_fname);
	FILE* outfile = fopen(out_fname, "w");
	dump_stringset(outfile);
	fclose(outfile);
   return EXIT_SUCCESS;
}
