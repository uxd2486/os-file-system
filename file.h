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
typedef struct i_node_s {
    uint32_t id;    // unique file id
    uint32_t bytes; // number of bytes written to the file
    uint32_t block; // first of 8 blocks allocated to this file
} file_t;

typedef struct file_block_s {
    int file_id;
    int block_id;
} filemap_t;

/*
** Globals
*/

/*
** Prototypes
*/

void _fl_init( void );

/*
** makes a new file, saves it
*/
int _fl_create_( int id );

File *_fl_open( int id );

int _fl_delete( int id );

int _fl_close( file_t *file );

int _fl_read( file_t *file, char *buf);

int _fl_write( file_t *file, char *buf, int buf_size );

#endif
/* SP_ASM_SRC */

#endif
