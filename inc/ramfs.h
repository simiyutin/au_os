#include "concurrency.h"
#ifndef SRC_RAMFS_H
#define SRC_RAMFS_H
#define BLOCK_SIZE 512
#define LINK_SIZE sizeof(struct link)
#define FILE_TABLE_SIZE 128
struct fsnode {
    struct fsnode * next;
    struct fsnode * prev;
    char data[BLOCK_SIZE];
};

#define FILE_STATE int
#define DELETED 0
#define OPENED 1
#define CLOSED 2

#define FILE_TYPE int
#define FILE_TYPE_FILE 0
#define FILE_TYPE_DIR 1


struct link {
    const char * name;
    struct FILE * target;
};



struct FILE {

    const char * pathname;
    struct fsnode * start;

    // sequential reading optimisation
    int current_reading_byte;
    int current_reading_pos;
    struct fsnode * current_reading_node;

    // sequential writing optimisation
    int current_writing_byte;
    int current_writing_pos;
    struct fsnode * current_writing_node;

    int byte_size;
    FILE_STATE state;

    FILE_TYPE type;

    struct spinlock lock;

};


extern struct FILE FILE_TABLE[];
extern struct spinlock ramfs_lock;

void create(const char * pathname);
struct FILE * open(const char * pathname);
void close(struct FILE * file);

char readchar(struct FILE * file, int shift);
void writechar(struct FILE * file, int shift, char value);

void writestring(struct FILE *, const char *);
const char * read_file_to_string(struct FILE * file);

void mkdir(const char * pathname);
char * readdir(const char * pathname);


#endif //SRC_RAMFS_H
