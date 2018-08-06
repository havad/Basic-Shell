#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/limits.h>


void cpFile(char* srcFile, char* destFile);
void cpDirectory(char* option, char* srcDirectory, char* destDirectory);
int count = 0;

int main(int argc, char* argv[]) {

    if(argc == 3) {
        cpFile(argv[1], argv[2]); // if file copy
    }
    else if(argc == 4) {
        cpDirectory(argv[1], argv[2], argv[3]); // if directory copy

    }
    else {
        printf("Usage: mycp (-R) 'src_file' 'dest_file'\n");
    }
}

void cpFile(char* srcFile, char* destFile) {

    FILE *fp = fopen(srcFile, "r");
    char* buffer;
    int stringSize, readSize;

    if(fp == NULL) {
        printf("ERROR: Could not copy %s, either the file does not exist, or you do not have the correct permissions.\n", srcFile);
        return;
    }

    fseek(fp, 0, SEEK_END);
    stringSize = ftell(fp);

    rewind(fp);

    buffer = (char*) malloc(sizeof(char) * (stringSize + 1));
    readSize = fread(buffer, sizeof(char), stringSize, fp);
    buffer[stringSize] = '\0';

    fclose(fp);

    fp = fopen(destFile, "w");

    if(fp == NULL) {
        printf("ERROR: Could not copy to %s, either the disk is full, or you do not have the correct permissions.\n", destFile);
    }

    fprintf(fp, "%s", buffer);

    fclose(fp);
}

void cpDirectory(char* option, char* srcDirectory, char* destDirectory)
{


    if(strncmp(option, "-R", strlen("-R")) == 0) // checking to see if the right option was used
    {
        DIR* dir1 = opendir(srcDirectory);
        DIR* dir2 = opendir(destDirectory);
        if (dir1 == NULL || dir2 == NULL) // checking to make sure both are directories
        {
            printf("Both files need to be directories \nUsage: mycp -R 'directory1' 'directory2'\n");
        }
        else
        {
            char dest_pathstart[PATH_MAX];
            if(count == 0) { // if first directory, copy src directory in destdir to start
               
                sprintf(dest_pathstart, "%s/%s", destDirectory, srcDirectory);
                mkdir(dest_pathstart, 0775);
                count++;
            }
            else {
                
                strcpy(dest_pathstart, destDirectory);
                printf("%s/\n", dest_pathstart); 
            }

            struct dirent *file;
            while ((file = readdir(dir1)) != NULL) {    // while there is still files left in the directory
                if ( !strcmp(file->d_name, ".") || !strcmp(file->d_name, "..") ) // checking for upper and lower directory indicators
                    continue;
                if(file->d_type == DT_REG) // if the file is a regular file
                {
                    printf("%s/%s\n", dest_pathstart, file->d_name);
                    char dest_path[PATH_MAX], src_path[PATH_MAX];
                    sprintf(dest_path, "%s/%s", dest_pathstart, file->d_name);
                    sprintf(src_path, "%s/%s", srcDirectory, file->d_name);
                    cpFile(src_path, dest_path);
                }
                else if (file->d_type == DT_DIR) { // if the file is a directory
                    char dest_path[PATH_MAX], src_path[PATH_MAX];
                    sprintf(dest_path, "%s/%s", dest_pathstart, file->d_name);
                    sprintf(src_path, "%s/%s", srcDirectory, file->d_name);
                    mkdir(dest_path, 0775);
                    cpDirectory(option, src_path, dest_path); // recursive call

                }

            }

            closedir(dir1);
            closedir(dir2);

        }
    }
    else {
        printf("Need to use '-R' option \nUsage: mycp -R 'directory1' 'directory2'\n");
    }

}
