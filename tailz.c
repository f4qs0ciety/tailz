#include "stdio.h"
#include "stdlib.h"
#include <stdbool.h>
#include <string.h>

int cnHandler(char argv[], bool *mode){
	int num_start = 0;
	switch (argv[0]){
		case '+':
			*mode = true;
			num_start++;
			break;
		case '-':
			num_start++;
			break;
	}
	int num_end = 0;
	while (argv[num_end]){num_end++;} //Find the length of argv
	int input = 0;
	int symb;
	int decs = 1; //Decimals modifier
	for (int j = num_end - 1; j >= num_start; j--){
		symb = argv[j] - '0';
		if (symb < 0 || symb > 9){
			printf("%s\n", "Bad lines/bytes number!");
			return -1;
		}
		input += symb * decs;
		decs = decs * 10;
	}
	return input;
}

int main(int argc, char* argv[]){
	bool verbose = false;
	bool out_mode = false; //if false - outputs lines, if true - outputs bytes
	bool custom = false; //Sets true if -n or -c is used
	int lines = 10; //Number of lines (or bytes) output
	bool plus_mode = false; //if false - outputs the last NUM lines, if true - outputs starting with line NUM
	int file_ctr = 0;
	FILE* temp = NULL;
	for (int i = 1; i < argc; i++){ //Cycle for arguments parcing
		if (argv[i][0] == '-'){
			int c = 1;
			while (argv[i][c]){
				switch (argv[i][c]){
				case 'c':
					out_mode = true;
					custom = true;
					//printf("%s", "Bytes set true\n"); //Debug
					break;
				case 'n':
					out_mode = false;
					custom = true;
					//printf("%s", "Lines set true\n"); //Debug
					break;
				case 'v':
					verbose = true;
					//printf("%s", "Verbose set true\n"); //Debug
					break;
				default:
					char *tmp;
					lines = strtol(argv[i], &tmp, 10) * -1;
					
				}
				c++;
			}
			if (custom){
				//Resolving the NUM input
				if (!argv[i+1]){
					char mdchr[] = "-n";
					if (out_mode){
						mdchr[1] = 'c';
					} //else { mdchr = "-n"; }
					printf("%s %s %s", "Option", mdchr, "requires an argument!\n");
					return 1;
				}
				lines = cnHandler(argv[i+1], &plus_mode);
				if (lines == -1) return 1;
				file_ctr--;
				custom = false;
			}
		} else file_ctr++;
	}
	char buf[1024];
	bool std = false;
	if (file_ctr > 1) verbose = true;
	if (file_ctr == 0){
		std = true;
		argc = 2;
		argv[1] = "tailztemp";
		temp = fopen("tailztemp", "a");
		if (temp == NULL){
			perror("tailztemp");
		}
		while (fgets(buf, sizeof(buf), stdin)){
			fputs(buf, temp);
		}
		fclose(temp);
	}
	int offset;
	for (int i = 1; i < argc; i++){ //Cycle for filenames parcing
		offset = 1;
		if (argv[i][0] != '-'){
			FILE* fl = fopen(argv[i], "r");
			if (fl == NULL){
				perror(argv[i]);
			} else {
				if (verbose){
					if (i > 1){ printf("\n"); }
					if (std) {
						printf("%s\n", "==> standard input <==");	
					} else { printf("%s %s %s\n", "==> ", argv[i], " <=="); }
				}
				int l = 0;
				if (!out_mode){
					while (l <= lines){
						do {
							if (plus_mode) { fseek(fl, offset, SEEK_SET); }
							else { 
								fseek(fl, 0 - offset, SEEK_END); 
								if (ftell(fl) < 1) break;
							}
								offset++;
						} while (fgetc(fl) != '\n');
						l++;
					} 
					char *ptr;
					while (fgets(buf, sizeof(buf), fl)){
						ptr = strchr(buf, '\n');
						if (ptr != NULL){
							*ptr = '\0';
						}
						puts(buf);
					}
				} else {
					while (l <= lines){
						if (plus_mode) { fseek(fl, offset, SEEK_SET); }
						else { fseek(fl, 0 - offset, SEEK_END); }
						offset++;
						l++;
					}
					char byte = fgetc(fl);
					while (byte != EOF){
						putchar(byte);
						byte = fgetc(fl);
					}
				}
			}
			fclose(fl);
		} else {
			i++;
		}
	}
	if (temp){
		remove("tailztemp");
	}
	return 0;
}
