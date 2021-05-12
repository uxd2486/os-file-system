/**
** @file file.h
**
** @author Utkarsh Dayal CSCI-452 class of 20205
**
** Function definitions for reading/writing one file 
*/

#ifndef FILE__H_
#define FILE_H_

/*
** General (C and/or assembly) definitions
**
** This section of the header file contains definitions that can be
** used in either C or assembly-language source code.
*/

#define NUM_BLOCKS 8

#ifndef SP_ASM_SRC
/*
** Start of C-only definitions
**
** Anything that should not be visible to something other than
** the C compiler should be put here.
*/

/*
** Types
*/
typedef struct i_node {
    uint32_t id;    // unique file id
    uint32_t bytes; // number of bytes written to the file
    uint32_t block; // first of 8 blocks allocated to this file
} File;

typedef struct file_block {
    int file_id;
    int block_id;
} file_map;

/*
** Globals
*/

/*
** Prototypes
*/

void _fl_init();

/*
** makes a new file, saves it
*/
int create_file( int id );

File *open_file( int id );

int delete_file( int id );

int close_file( File *file );

int read_file( File *file, char *buf);

int write_file( File *file, char *buf, int buf_size );

#endif
/* SP_ASM_SRC */

#endif
