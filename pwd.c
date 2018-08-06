#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "pwd.h"

void pwd()
{
	char pwd[1024];
	if(getcwd(pwd, sizeof(pwd)) != NULL){		//Gets the current path
	  printf("%s\n", pwd);
	}
}
