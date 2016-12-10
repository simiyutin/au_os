#ifndef SRC_RAMFS_H
#define SRC_RAMFS_H
#define BLOCK_SIZE 2
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


struct FILE {
    const char * pathname;
    struct fsnode * start;
    //int current_reading_byte;
    //struct fsnode * current_reading_node;
    int byte_size;
    FILE_STATE state;
};

struct FILE FILE_TABLE[FILE_TABLE_SIZE];

int get_empty_file_slot();
void create(const char * pathname);
struct FILE * open(const char * pathname); //todo w+, w, r
void close(struct FILE * file);

char readchar(struct FILE * file, int shift);
void writechar(struct FILE * file, char value);

void mkdir(const char * pathname);
//readdir


#endif //SRC_RAMFS_H
