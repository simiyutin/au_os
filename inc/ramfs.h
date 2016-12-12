#ifndef SRC_RAMFS_H
#define SRC_RAMFS_H
#define BLOCK_SIZE 512
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
    // sequential reading optimisation
    int current_reading_byte;
    int current_reading_pos;
    struct fsnode * current_reading_node;
    // sequential writing optimisation
    int current_writing_pos;
    struct fsnode * current_writing_node;
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
void writestring(struct FILE *, const char *);
const char * read_file_to_string(struct FILE * file);

//todo void mkdir(const char * pathname);
//todo readdir


#endif //SRC_RAMFS_H
