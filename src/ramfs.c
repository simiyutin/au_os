#include <stddef.h>
#include <stdlib.h>
#include "../inc/ramfs.h"
#include "../inc/alloc.h"
#include "../inc/print.h"
#include "../inc/throw.h"



int get_empty_file_slot() {
    int i = 0;
    for (;FILE_TABLE[i].state != DELETED; ++i){} //todo handle out of range
    return i;
}

void create(const char * pathname){
    struct fsnode * first_file_node = (struct fsnode *) mem_alloc(sizeof(struct fsnode));
    first_file_node->prev = NULL;
    first_file_node->next = NULL;
    int empty_file_slot_index = get_empty_file_slot();
    FILE_TABLE[empty_file_slot_index].state = CLOSED;
    FILE_TABLE[empty_file_slot_index].byte_size = 0;
    FILE_TABLE[empty_file_slot_index].pathname = pathname;
    FILE_TABLE[empty_file_slot_index].start = first_file_node;
}

int find_file(const char * pathname) {
    int i = 0;
    for (;strcmp(FILE_TABLE[i].pathname, pathname) != 0
          && FILE_TABLE[i].state == DELETED; ++i){}

    return i;
}

struct FILE * open(const char * pathname) {

    int i = find_file(pathname);

    //todo handle out of range

    FILE_TABLE[i].state = OPENED;

    return &FILE_TABLE[i];
}

void close(struct FILE * file) {
    file->state = CLOSED;
}




struct fsnode * find_node_by_position(struct FILE * file, int * pos) {
    struct fsnode * node = file->start;

    printf("*pos - BLOCK_SIZE: %d", *pos - BLOCK_SIZE);
    while (*pos - BLOCK_SIZE > 0 && node->next != NULL) {
        printf("cycle\n");
        node = node->next;
        *pos -= BLOCK_SIZE;
    }
    printf("exit find_node\n");
    return node;
}

char readchar(struct FILE * file, int shift) {
    if (file->state != OPENED) throw_ex("trying to open closed file");
    if (file->byte_size <= shift) throw_ex("trying to read file out of range");

    //todo add current_reading to improve sequential reading

    printf("find node\n");
    struct fsnode * block_shift = find_node_by_position(file, &shift);

    return block_shift->data[shift];
}


void writechar(struct FILE * file, char value) {
    if (file->state != OPENED) throw_ex("trying to write to closed file");
    int pos = file->byte_size;
    printf("pos: %d\n", pos);
    struct fsnode * block_shift = find_node_by_position(file, &pos);

    if ((pos - BLOCK_SIZE ) > 0) {
        printf("create new node\n");
        struct fsnode * prev_next = block_shift->next;
        block_shift->next = (struct fsnode *) mem_alloc(sizeof(struct fsnode));
        block_shift->next->next = prev_next;
        block_shift->next->prev = block_shift;
        block_shift = block_shift->next;
        pos -= BLOCK_SIZE;
    }

    block_shift->data[pos] = value;
    ++file->byte_size;

    printf("passed\n");
}

void writestring(struct FILE * file, const char * string_to_write) {
    size_t length = strlen(string_to_write);
    for (size_t i = 0; i < length; ++i) {
        writechar(file, string_to_write[i]);
    }
}

const char * read_file_to_string(struct FILE * file) {
    char * result = mem_alloc(file->byte_size + 1);
    for (size_t i = 0; i < file->byte_size; ++i) {
        result[i] = readchar(file, i);
    }
    result[file->byte_size] = '\0';
    return result;
}









