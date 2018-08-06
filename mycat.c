#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {

	FILE *fp;
	char* buffer;
	int stringSize, readSize;
	
	// If no arguments were passed, check if stdio is empty
	// If it's not empty, print contents of stdio	
	if(argc == 1) 
	{
		if(!feof(stdin)) 
		{
			fseek(stdin, 0, SEEK_END);
			stringSize = ftell(stdin);

			rewind(stdin);

			buffer = (char*) malloc(sizeof(char) * (stringSize + 1));
			readSize = fread(buffer, sizeof(char), stringSize, stdin);
			buffer[stringSize] = '\0';

			printf("%s", buffer);
		
		}else{
			printf("Usage: mycat 'inputfilenames' '>outputfilename'");
		}
	}else{

		// Loop over arguments
		for(int i = 1; i < argc; i++) 
		{

			char* filename = argv[i];

			// Open each argument as a file
			fp = fopen(filename, "r");

			if(fp == NULL) {
				printf("ERROR: Could not open %s, either the file does not exist, or you do not have the correct permissions.\n", filename);
				return 1;
			}

			// Seek to end of file, store length of file
			fseek(fp, 0, SEEK_END);
			stringSize = ftell(fp);

			// Seek back to beginning of file
			rewind(fp);
		
			// Allocate memory for file in buffer array, and add null
			// terminator on to end
			buffer = (char*) malloc(sizeof(char) * (stringSize + 1));
			readSize = fread(buffer, sizeof(char), stringSize, fp);
			buffer[stringSize] = '\0';

			printf("%s", buffer);

			fclose(fp);
		}
	}

	return (0);
}
