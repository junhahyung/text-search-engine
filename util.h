/*
 * =====================================================================================
 *
 *       Filename:  util.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/29/2019 16:27:18
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Junha Hyung (), sharpeeee@kaist.ac.kr
 *   Organization:  
 *
 * =====================================================================================
 */
#define HASHLEN 4096 

int stringncmp(char* str1, char* str2, int n);
int stringlen(char *str);

char *stringncat(char *dest, char *src, int n);
char *joinpath(char *str1, char *str2);
struct hashtable *readntokenize(int fd, int docID, struct hashtable *hash_t);
int tokenize(char *buf, int docID, int line, struct hashtable *hash_t);
int isalphabet(char word);
unsigned long hash(unsigned char *str);
void printall(struct hashtable *hash_t);
void search(struct hashtable *hash_t, char* targetword, char* docDic[]);
unsigned long hash(unsigned char *str);
struct termstruct
{
    char *term;
    int total_freq;
    struct pos* next_pos;
    struct termstruct* next_term;
};

struct pos
{
    int position[2];
    struct pos* next_pos;
};

struct hashtable
{
    struct termstruct* term_t[HASHLEN];
};
