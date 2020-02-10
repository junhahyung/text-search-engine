/*
 * =====================================================================================
 *
 *       Filename:  util.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/29/2019 12:16:19
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
#include <unistd.h>
#include "util.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  stringncmp
 *  Description:  
 * =====================================================================================
 */
int stringncmp (char *str1, char *str2, int n)
{
    int r = 0;
    for(int i=0; i<n; i++)
    {
        if(*str1 != *str2)
        {
            r = 1;
            break;
        }
        str1++;
        str2++;
    }
    return r;
}		/* -----  end of function stringncmp  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  pathjoin
 *  Description:  
 * =====================================================================================
 */

char *stringncat (char *dest, char *src, int n)
{
    char *ret = dest;
    while(*dest)
        dest++;

    while(n--)
        *dest++ = *src++;
    *dest = 0;
    
    return ret;
}		/* -----  end of function stringncat  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  stringlen
 *  Description:  
 * =====================================================================================
 */
   
int stringlen (char *str)
{
    int n=0;
    while(*str++)
        n++;
    return n;
}		/* -----  end of function stringlen  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  joinpath
 *  Description: should use free explicitly!!! 
 * =====================================================================================
 */

char *joinpath (char *str1, char *str2)
{
    int strlen1 = stringlen(str1);
    int strlen2 = stringlen(str2);
    char *fullpath = calloc(strlen1 + strlen2 + 2, sizeof(char));
    stringncat(fullpath, str1, strlen1);
    if(*(fullpath + strlen1 - 1) != '/')
        stringncat(fullpath, "/", 1);
    stringncat(fullpath, str2, strlen2);

    return fullpath; 
}		/* -----  end of function joinpath  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  readfile
 *  Description: need explicit free!!! 
 * =====================================================================================
 */

struct hashtable *readntokenize (int fd, int docID, struct hashtable *hash_t)
{
    char buf[4096] = {0,};
    int bytes_read = 0;
    int total_bytes = 0;
    int line = 1;
    char *word_t = NULL;
    
    if (!hash_t)
        hash_t = (struct hashtable*)calloc(1, sizeof(struct hashtable));

    do
    {
        if ((bytes_read = read(fd, buf, 4096)) == -1)
        {
            perror("cannot read file\n");
            exit(EXIT_FAILURE);
        }
        //check if word is split
        if (isalphabet(buf[4095]))
        {
            word_t = buf + 4095;
            while(isalphabet(*word_t))
            {
                *word_t = 0;
                word_t--;
                bytes_read--;
            }
            total_bytes += bytes_read;
            line = tokenize(buf, docID, line, hash_t);

            lseek(fd, total_bytes, SEEK_SET);
        }
        else
        {
            line = tokenize(buf, docID, line, hash_t);
        }

        //memset(buf, 0, 4096);
        for (int i=0; i<4096; i++)
            buf[i] = 0;

    } while(bytes_read != 0);

    return hash_t;

}		/* -----  end of function readntokenize  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  tokenize
 *  Description:  
 * =====================================================================================
 */
int
tokenize (char *buf, int docID, int line, struct hashtable *hash_t)
{
    char *temp_head = buf;
    char *temp_lag = buf;
    int wordlen = 0;
    unsigned long hashn = 0;
    struct termstruct *term_t = NULL;
    while(*temp_head)
    {
        if(isalphabet(*temp_head))
            temp_head++;
        else
        {
                wordlen = temp_head - temp_lag; 
                if (wordlen == 0)
                {
                    if(*temp_head == '\n')
                        line++;
                    temp_head++;
                    temp_lag = temp_head;
                    continue;
                }
                term_t = (struct termstruct *)malloc(sizeof(struct termstruct));
                term_t->term = (char*)calloc(wordlen + 1, sizeof(char));
                stringncat(term_t->term, temp_lag, wordlen);
                term_t->total_freq = 1;
                term_t->next_pos = (struct pos*)malloc(sizeof(struct pos));
                term_t->next_pos->position[0] = docID;
                term_t->next_pos->position[1] = line;
                term_t->next_pos->next_pos = NULL;
                term_t->next_term = NULL;

                //insert term_t into hash
                hashn = hash((unsigned char*)term_t->term);
                hashn = hashn % HASHLEN;
                struct termstruct *termloc = hash_t->term_t[hashn];
                struct termstruct *termprev = hash_t->term_t[hashn];
                if (termloc == NULL)
                    hash_t->term_t[hashn] = term_t;
                else
                {
                    while(termloc)
                    {
                        termprev = termloc;
                        if ((stringlen(termloc->term) == wordlen) && !stringncmp(termloc->term, term_t->term, wordlen))
                        {
                            termloc->total_freq += 1;
                            struct pos *posloc = termloc->next_pos;
                            struct pos *prev = termloc->next_pos;
                            while(posloc)
                            {
                                if(term_t->next_pos->position[0] == posloc->position[0] && term_t->next_pos->position[1] == posloc->position[1])
                                {
                                    free(term_t->next_pos);
                                    break;
                                }

                                prev = posloc;
                                posloc = posloc->next_pos;
                            }
                            if (!posloc)
                                prev->next_pos = term_t->next_pos;

                            free(term_t);
                            break;
                        }
                        termloc = termloc->next_term;
                        if(!termloc)
                            termprev->next_term = term_t;
                    }
                }

            if(*temp_head == '\n')
                line++;

            temp_head++;
            temp_lag = temp_head;
        }
    }
    return line;
}		/* -----  end of function tokenize  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  printall
 *  Description:  
 * =====================================================================================
 */

void
printall (struct hashtable *hash_t)
{
    struct termstruct *next_term = NULL;
    struct  pos *next_pos = NULL;
    struct termstruct *cur_t = NULL;
    char *term = NULL;
    if(hash_t)
    {
        for (int i = 0; i<HASHLEN; i++)
        {
            cur_t = hash_t->term_t[i];
            if(cur_t)
            {
                printf("==========\n");
                printf("hash modulo HASHLEN = %d\n", i);

                do 
                {
                    term = cur_t->term;
                    printf("term : %s\n",term);
                    printf("total freq : %d\n", cur_t->total_freq);
                    next_pos = cur_t->next_pos;
                    printf("----------\n");
                    do 
                    {
                        printf("docID : %d\n", next_pos->position[0]);
                        printf("line : %d\n", next_pos->position[1]);
                        next_pos = next_pos->next_pos;
                    } while(next_pos);
                    printf("----------\n");
                    cur_t = cur_t->next_term;
                } while(cur_t);
                printf("==========\n\n");
            }
        }
    }
}		/* -----  end of function printall  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  search
 *  Description:  
 * =====================================================================================
 */

void
search (struct hashtable* hash_t, char* targetword, char* docDic[])
{
    int hashn = 0;
    int wordlen, targetlen, linenum, docID;
    struct termstruct *cur_t = NULL;
    struct pos *cur_pos = NULL;

    hashn = hash((unsigned char *)targetword) % HASHLEN;
    cur_t = hash_t->term_t[hashn];
    targetlen = stringlen(targetword);
    while(cur_t)
    {
        wordlen = stringlen(cur_t->term);
        if((wordlen == targetlen) && (!stringncmp(cur_t->term, targetword, wordlen)))
        {
            cur_pos = cur_t->next_pos;
            while(cur_pos)
            {
                docID = cur_pos->position[0];
                linenum = cur_pos->position[1];
                printf("%s: line #%d\n", docDic[docID], linenum);
                cur_pos = cur_pos->next_pos;
            }
            break;
        }
        cur_t = cur_t->next_term;
    }
    
    return;
}		/* -----  end of function search  ----- */
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  isalphabet
 *  Description:  
 * =====================================================================================
 */

int
isalphabet (char word)
{
    if((word >= 'a' && word <= 'z') || (word >= 'A' && word <= 'Z'))
        return 1;
    return 0;
}		/* -----  end of function isalphabet  ----- */
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  hash
 *  Description: hash function from http://www.cse.yorku.ca/~oz/hash.html  
 * =====================================================================================
 */
unsigned long
hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /*  hash * 33 + c */

    return hash;
}
/* -----  end of function hash  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
/*
int main ( int argc, char *argv[] )
{
   char* str1 = "/Users/junahyung"; 
   char* str2 = "csprojects";
   char* str3 = joinpath(str1, str2);
   printf("%s\n", str3);

   return EXIT_SUCCESS;
}
*/
