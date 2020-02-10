/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description: main file for inverse index file search program 
 *
 *        Version:  1.0
 *        Created:  09/28/2019 23:38:47
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Junha Hyung (), sharpeeee@kaist.ac.kr
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include "util.h"


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main ( int argc, char *argv[] )

{
    DIR *d;
    struct dirent *dir = NULL;
    char* curdir = ".";
    char *prevdir = "..";
    char *targetdir = argv[1];
    char command[100] = "";
    char targetword[100] = "";
    char *temp, *temp2 = NULL;
    char *docDic[1024] = {0,};
    int fd = 0;
    int docID = 0;
    int len = stringlen(targetdir);
    struct hashtable *hash_t = NULL;
    char progname[100] = {0,};

    stringncat(progname, argv[0]+2, stringlen(argv[0])-2);

    if((d = opendir(targetdir)) == NULL)
    {  
        perror("cannot access directory");
        exit(EXIT_FAILURE);
    }
    if(d)
    {
        while((dir = readdir(d)) != NULL)
        {
            char *dname = dir->d_name;
            docDic[docID] = dname;
            if(stringncmp(dname, curdir, 2) && stringncmp(dname, prevdir, 3))
            {
                int strlen1 = stringlen(targetdir);
                int strlen2 = stringlen(dname);
                char *fullpath = joinpath(targetdir, dname);

                if((fd = open(fullpath, O_RDONLY)) == -1)
                {
                    perror("cannot open file\n");
                    exit(EXIT_FAILURE);
                }
                hash_t = readntokenize(fd, docID, hash_t); 
                docID ++;
                close(fd);
            }
        }
        closedir(d);
        //printall(hash_t);
    }
    printf("finished bootstrapping\n");
    //printall(hash_t);
    printf("=======================\n");
    printf("*** Starting CLI ***\n");
    while(1)
    {
        printf("%s> ", progname); 
        scanf("%s %s", command, targetword);
        if(!stringncmp(command, "search", 6))
            search(hash_t, targetword, docDic);
        else
            continue;
    }
	return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */

