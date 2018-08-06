#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <getopt.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

int main(int argc, char *argv[]) {
	char loc[256];
	if(getcwd(loc, sizeof(loc)) == NULL) printf("Error in myls\n");
	DIR *mydir = opendir(loc);

	struct dirent *file;	
	//printf("%d\n\n", argc);
	if (argc == 1)
	{
		/*
		char loc[256];
		if(getcwd(loc, sizeof(loc)) == NULL) printf("Error in myls\n");
		DIR *mydir = opendir(loc);
		//printf("%s\n", loc);
		
		struct dirent *file;
		*/
		while((file = readdir(mydir)) != NULL)
		{
			if(strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0)
				printf("%s\n", file->d_name);
		}

	//	closedir(mydir);

	}
	else if (argc == 2 && strcmp(argv[1], "-l") != 0)
	{
		while((file = readdir(mydir)) != NULL)
		{
			if(strcmp(file->d_name, argv[1]) == 0)
				printf("%s\n", file->d_name);
		}

	}

	else
	{
		int arg = getopt(argc, argv, "l");
		switch(arg)
		{
			case 'l' :
				printf("Directory contents:\n");
				struct stat fileS;
		//		struct passwd pass;

//				if(stat(loc, &fileS) < 0 )
//					return 1;
			
				if (argc == 3)
				{
				//	printf("argv[2] is: %s\n\n", argv[2]);
					while((file = readdir(mydir)) != NULL)
					{
				//		printf("file->d_name is : %s\n\n", file->d_name);
						if(strcmp(argv[2], file->d_name) == 0)
						{
				//			printf("good ole test print");
							if(stat(file->d_name, &fileS) < 0)
								return 1;
							struct passwd *pass = getpwuid(fileS.st_uid);
							//printf("%s ", pass->pw_name);
							struct group *grp = getgrgid(fileS.st_gid);


							struct tm time;
							char modtime[100];
							time_t t = fileS.st_mtime;
							localtime_r(&t, &time);
							strftime(modtime, sizeof(modtime), "%c", &time);


							printf((fileS.st_mode & S_IRUSR) ? "r" : "-");
							printf((fileS.st_mode & S_IWUSR) ? "w" : "-");
							printf((fileS.st_mode & S_IXUSR) ? "x" : "-");
							printf((fileS.st_mode & S_IRGRP) ? "r" : "-");
							printf((fileS.st_mode & S_IWGRP) ? "w" : "-");
							printf((fileS.st_mode & S_IXGRP) ? "x" : "-");
							printf((fileS.st_mode & S_IROTH) ? "r" : "-");
							printf((fileS.st_mode & S_IWOTH) ? "w" : "-");
							printf((fileS.st_mode & S_IXOTH) ? "x" : "-");
					
							printf(" ");
					
							printf("%lu %s %s %ld %s %s\n", fileS.st_nlink, pass->pw_name, grp->gr_name, fileS.st_size, modtime, file->d_name);

						}
					}
				}


				else
				{
	
					while((file = readdir(mydir)) != NULL)
					{
						if(stat(file->d_name, &fileS) < 0)
							return 1;	
			
						struct passwd *pass = getpwuid(fileS.st_uid);
						//printf("%s ", pass->pw_name);
						struct group *grp = getgrgid(fileS.st_gid);

						struct tm time;
						char modtime[100];
						time_t t = fileS.st_mtime;
						localtime_r(&t, &time);
						strftime(modtime, sizeof(modtime), "%c", &time);
	
						printf((fileS.st_mode & S_IRUSR) ? "r" : "-");
						printf((fileS.st_mode & S_IWUSR) ? "w" : "-");
						printf((fileS.st_mode & S_IXUSR) ? "x" : "-");
						printf((fileS.st_mode & S_IRGRP) ? "r" : "-");
						printf((fileS.st_mode & S_IWGRP) ? "w" : "-");
						printf((fileS.st_mode & S_IXGRP) ? "x" : "-");
						printf((fileS.st_mode & S_IROTH) ? "r" : "-");
						printf((fileS.st_mode & S_IWOTH) ? "w" : "-");
						printf((fileS.st_mode & S_IXOTH) ? "x" : "-");
				
						printf(" ");
						printf("%lu %s %s %ld %s %s\n", fileS.st_nlink, pass->pw_name, grp->gr_name, fileS.st_size, modtime, file->d_name);
					
					}
				}
			break;
			
		}
	}

	closedir(mydir);
	
}
