#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mycd.h"
//#include "pwd.h"

void mycd(char *dir){
	int n = chdir(dir);

	if(n != 0)			//If directory doesn't exist
		printf("ERROR: No such directory\n");
	//else
		//pwd();		//Calls & display pwd to check
}
