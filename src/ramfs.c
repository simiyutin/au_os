#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../inc/ramfs.h"
#include "../inc/alloc.h"
#include "../inc/print.h"
#include "../inc/throw.h"

int get_empty_file_slot() {
    int i = 0;
    for (;FILE_TABLE[i].state != DELETED; ++i){
        if (i == FILE_TABLE_SIZE) throw_ex("no more empty slots for files!");
    }
    return i;
}

int get_number_of_links(struct FILE * file) {
    int NUMBER_OF_LINKS = file->byte_size / sizeof (struct link);
    return NUMBER_OF_LINKS;
}

int __recursive_search(int prev_id, char * pathname) {

    char * filename = strsep(&pathname, "/");
    if (filename == NULL){
        printf("next iteration aborted, file found\n");
        return prev_id;
    }
    printf("recursive search: filename: %s\n", filename);
    struct FILE prev_file = FILE_TABLE[prev_id];
    if (prev_file.type != FILE_TYPE_DIR) throw_ex("no such directory");
    printf("recursive search: passed first step\n");


    // TODO VERY IMPORTANT HANDLE CASE WHEN DIRECTORY HAS MORE FILES THAN FIT IN ONE BLOCK
    // TODO OR RESTRICT NUMBER OF FILES IN DIR BY SIZE OF BLOCK
    int NUMBER_OF_LINKS = get_number_of_links(&prev_file);
    printf("prev dir number of links: %d\n", NUMBER_OF_LINKS);
    struct link * links = (struct link *) prev_file.start->data;

    printf("start search in directory\n");
    int cur_id = 0;
    for (;cur_id < NUMBER_OF_LINKS &&
          (links[cur_id].target->state == DELETED ||
           links[cur_id].name == NULL ||
           strcmp(links[cur_id].name, filename) != 0
          ); ++cur_id);

    printf("cycle ended\n");
    if (cur_id == NUMBER_OF_LINKS) return FILE_TABLE_SIZE;

    printf("%d\n", links[cur_id].target);
    printf("%d\n", FILE_TABLE);
    int file_index_in_table = ((uint64_t) links[cur_id].target - (uint64_t) FILE_TABLE) / sizeof(struct FILE);
    printf("next file index: %d\n", file_index_in_table);
    return __recursive_search(file_index_in_table, pathname);

}

int __find_file(const char *pathname) {

    printf("__find_file\n");
    if (pathname == NULL) return FILE_TABLE_SIZE;

    char * duplicated_filename = strdup(pathname); // todo free

    char * filename = strsep(&duplicated_filename, "/");
    printf("extracted filename: %s\n", filename);

    //find entry point to recursion
    int i = 0;
    for (;i < FILE_TABLE_SIZE &&
          (FILE_TABLE[i].state == DELETED ||
           FILE_TABLE[i].pathname == NULL ||
           strcmp(FILE_TABLE[i].pathname, filename) != 0
          ); ++i);

    if (i == FILE_TABLE_SIZE) return i;

    printf("entering recursive search.. start file/dir: %d\n", i);
    return __recursive_search(i, duplicated_filename);
}

int find_last_slash(const char * pathname) {

    int lastslash = strlen(pathname) - 1;
    while (lastslash-- > 0){
        if(pathname[lastslash] == '/') break;
    }

    return lastslash;
}

int __create_file(const char * pathname) {

    printf("__create_file: %s\n", pathname);
    int last_slash = find_last_slash(pathname);
    int len = strlen(pathname) + 1;
    char * dir_name = mem_alloc(last_slash + 1);//todo free
    char * file_name = mem_alloc(len - last_slash);

    if (last_slash == -1) {
        dir_name = NULL;
        memcpy(file_name, pathname, len);
    } else {
        memcpy(dir_name, pathname, last_slash);
        dir_name[last_slash] = NULL;
        memcpy(file_name, pathname + last_slash + 1, len);
        printf("dir name: %s\n", dir_name);
        printf("file name: %s\n", file_name);
    }

    int dir = __find_file(dir_name);
    printf("dir num: %d\n", dir);

    struct fsnode * first_file_node = (struct fsnode *) mem_alloc(sizeof(struct fsnode));
    first_file_node->prev = NULL;
    first_file_node->next = NULL;
    int empty_file_slot_index = get_empty_file_slot();
    FILE_TABLE[empty_file_slot_index].state = CLOSED;
    FILE_TABLE[empty_file_slot_index].byte_size = 0;
    FILE_TABLE[empty_file_slot_index].pathname = file_name;
    FILE_TABLE[empty_file_slot_index].start = first_file_node;
    FILE_TABLE[empty_file_slot_index].type = FILE_TYPE_FILE;


    if (dir != FILE_TABLE_SIZE) {
        printf("\n\n\n\nAND I AM DANGEROUS! \n\n\n\n");
        struct link * links = (struct link *) &FILE_TABLE[dir].start->data;
        int new_link_index = FILE_TABLE[dir].byte_size / sizeof(struct link);
        struct link * new_link = &links[new_link_index];
        FILE_TABLE[dir].byte_size += sizeof(struct link);
        new_link->target = &FILE_TABLE[empty_file_slot_index];
        new_link->name = file_name; //можно не освобождать, так как все равно удалять файлы не умеем
        printf("debug, new_link->target->pathname: %s\n", links[new_link_index].target->pathname);
        printf("debug, new_link->name: %s\n", links[new_link_index].name);
    }


    printf("__create_file end\n");
    return empty_file_slot_index;
}

void create(const char * pathname){

    printf("create\n");
    if(__find_file(pathname) != FILE_TABLE_SIZE) throw_ex("trying to create file which already exists");
    printf("file is not already created, ok..\n");
    __create_file(pathname);
    printf("create end\n");

}

struct FILE * open(const char * pathname) {

    printf("trying to open.. %s\n", pathname);
    int found_file_id = __find_file(pathname);
    printf("found file id: %d\n", found_file_id);
    if (found_file_id == FILE_TABLE_SIZE) throw_ex("file not found");


    FILE_TABLE[found_file_id].state = OPENED;

    return &FILE_TABLE[found_file_id];
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
    FILE_TABLE[file_id].type = FILE_TYPE_DIR;
}

char * readdir(const char * pathname) {

    struct FILE * dir = &FILE_TABLE[__find_file(pathname)];

    if (dir->type != FILE_TYPE_DIR) throw_ex("wrong argument - directory is awaited");
    int concatenated_str_length = 0;
    int NUMBER_OF_LINKS = get_number_of_links(dir);
    struct link * links = (struct link *) dir->start->data;

    printf("start search in directory\n");

    for (int cur_id = 0; cur_id < NUMBER_OF_LINKS; ++cur_id) {

        concatenated_str_length += strlen(links[cur_id].name) + 1;

    };

    concatenated_str_length += 1; // null terminator

    char * result_string = mem_alloc(concatenated_str_length);
    int filled_length = 0;
    for (int cur_id = 0; cur_id < NUMBER_OF_LINKS; ++cur_id) {
        printf("filename: %s\n", links[cur_id].name);
        int name_length = strlen(links[cur_id].name);
        memcpy(result_string + filled_length, links[cur_id].name, name_length);
        result_string[filled_length + name_length] = ' ';
        filled_length += name_length + 1;
    };

    printf("filled length: %d , concatenated length: %d\n", filled_length, concatenated_str_length);

    result_string[filled_length] = NULL;

    return result_string;
}









