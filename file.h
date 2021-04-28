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

#define NUM_BLOCKS 10

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
    int id,
    int *blocks,
    int indirect_block
} File;

typedef struct file_map {
    int file_id,
    int block_id
} FileMap;

/*
** Globals
*/

/*
** Prototypes
*/

/*
** makes a new file, saves it
*/
int create_file( int id );

File *open_file( int id );

int close_file( File *file );

int read_file( File file, char *buf);

int write_file( File file, char *buf, int buf_size );

#endif
/* SP_ASM_SRC */

#endif
