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

int __recursive_search(int prev_id, char * pathname) {
    char * filename = strsep(&pathname, "/");
    if (filename == NULL) return prev_id;
    struct FILE prev_file = FILE_TABLE[prev_id];
    if (prev_file.type != FILE_TYPE_DIR) throw_ex("no such file or directory"); //todo may be return FILE_TABLE_SIZE


    int NUMBER_OF_LINKS = prev_file.byte_size / sizeof (struct link); //todo case when file and directory names are equal

    // TODO VERY IMPORTANT HANDLE CASE WHEN DIRECTORY HAS MORE FILES THAN FIT IN ONE BLOCK
    // TODO OR RESTRICT NUMBER OF FILES IN DIR BY SIZE OF BLOCK
    struct link * links = (struct link *) prev_file.start->data;


    int cur_id = 0;
    for (;cur_id < NUMBER_OF_LINKS &&
          (links[cur_id].target->state == DELETED ||
           links[cur_id].name == NULL ||
           strcmp(links[cur_id].name, filename) != 0
          ); ++cur_id);

    if (cur_id == NUMBER_OF_LINKS) return FILE_TABLE_SIZE; // not found

    return __recursive_search(cur_id, pathname);

}

int __find_file(const char *pathname) {

    char * duplicated_pathname = strdup(pathname);
    char * filename = strsep(&duplicated_pathname, "/");

    //find entry point to recursion
    int i = 0;
    for (;i < FILE_TABLE_SIZE &&
          (FILE_TABLE[i].state == DELETED ||
           FILE_TABLE[i].pathname == NULL ||
           strcmp(FILE_TABLE[i].pathname, filename) != 0
          ); ++i);

    if (i == FILE_TABLE_SIZE) return i;
    return __recursive_search(i, duplicated_pathname);
}

int __create_file(const char * pathname) {

//    const char * dir_name = extract_dir(pathname);
//    const char * file_name = extract_name(pathname);

    const char * dir_name = NULL;
    const char * file_name = "/testfile";

    int dir = __find_file(dir_name);

    struct fsnode * first_file_node = (struct fsnode *) mem_alloc(sizeof(struct fsnode));
    first_file_node->prev = NULL;
    first_file_node->next = NULL;
    int empty_file_slot_index = get_empty_file_slot();
    FILE_TABLE[empty_file_slot_index].state = CLOSED;
    FILE_TABLE[empty_file_slot_index].byte_size = 0;
    FILE_TABLE[empty_file_slot_index].pathname = file_name;
    FILE_TABLE[empty_file_slot_index].start = first_file_node;
    FILE_TABLE[empty_file_slot_index].type = FILE_TYPE_FILE;


    //todo аааай вааай апасна
    struct link * new_link = (struct link *) &FILE_TABLE[dir].start->data + FILE_TABLE[dir].byte_size;
    new_link->target = &FILE_TABLE[empty_file_slot_index];
    new_link->name = file_name;

    return empty_file_slot_index;
}

int __create_dir(const char* pathname) {
    int result = __create_file(pathname);
    FILE_TABLE[result].type = FILE_TYPE_DIR;
    return result;
}

void create(const char * pathname){

    if(__find_file(pathname) != FILE_TABLE_SIZE) throw_ex("trying to create file which already exists");
    printf("\n\n\nSUCCESS\n\n\n");
    __create_file(pathname);

}

struct FILE * open(const char * pathname) {

    int i = __find_file(pathname);
    if (i == FILE_TABLE_SIZE) throw_ex("file not found");


    FILE_TABLE[i].state = OPENED;

    return &FILE_TABLE[i];
}

void close(struct FILE * file) {
    file->state = CLOSED;
}




struct fsnode * find_node_by_position(struct fsnode * node, int * pos) {

    while (*pos - BLOCK_SIZE > 0 && node->next != NULL) {
        node = node->next;
        *pos -= BLOCK_SIZE;
    }
    return node;
}

char readchar(struct FILE * file, int shift) {

    if (file->state != OPENED) throw_ex("trying to read closed file");
    if (file->byte_size <= shift) throw_ex("trying to read file out of range");

    struct fsnode * block_base;
    int pos = shift;
    if (file->current_reading_byte == pos - 1) {
        block_base = file->current_reading_node;
        pos = file->current_reading_pos + 1;
    } else {
        block_base = (file->start);
    }
    struct fsnode * block_shift = find_node_by_position(block_base, &pos);

    file->current_reading_node = block_shift;
    file->current_reading_pos = pos;
    file->current_reading_byte = shift;

    return block_shift->data[pos];
}


void writechar(struct FILE * file, int shift, char value) {

    if (file->state != OPENED) throw_ex("trying to write to closed file");
    if (file->byte_size < shift) throw_ex("trying to write leaving gap of trash");

    int pos = shift;
    struct fsnode * block_base;
    if (file->current_writing_byte == pos - 1) {
        block_base = file->current_writing_node;
        pos = file->current_writing_pos + 1;
    } else {
        block_base = file->start;
    }
    struct fsnode * block_shift = find_node_by_position(block_base, &pos);

    if ((pos - BLOCK_SIZE ) > 0) {
        struct fsnode * prev_next = block_shift->next;
        block_shift->next = (struct fsnode *) mem_alloc(sizeof(struct fsnode));
        block_shift->next->next = prev_next;
        block_shift->next->prev = block_shift;
        block_shift = block_shift->next;
        pos -= BLOCK_SIZE;
    }

    block_shift->data[pos] = value;

    file->byte_size++;
    file->current_writing_node = block_shift;
    file->current_writing_byte = shift;
    file->current_writing_pos = pos;
}

void writestring(struct FILE * file, const char * string_to_write) {

    size_t length = strlen(string_to_write);
    for (size_t i = 0; i < length; ++i) {
        writechar(file, file->byte_size, string_to_write[i]);
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


void mkdir(const char * pathname) {
    if(__find_file(pathname) != FILE_TABLE_SIZE) throw_ex("trying to create directory which already exists");
    int file_id = __create_file(pathname);
}









