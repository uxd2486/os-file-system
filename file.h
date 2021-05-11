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
    int id;
    int bytes; // number of bytes written already
    int blocks[NUM_BLOCKS];
} File;

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
