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

/*
** Stores file meta-data, AKA the i-node
*/
typedef struct i_node_s {
    uint32_t id;    // unique file id
    uint32_t bytes; // number of bytes written to the file
    uint32_t block; // first of 8 blocks allocated to this file
} file_t;

/*
** Maps file ids to the block that stores its i-node
*/
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

/**
** Name:  _fl_init
**
** Initializes global variables and calls the block init function
**
*/
void _fl_init( void );

/**
** Name:  _fl_create
**
** Creates a new file, given an id to assign to it
**
** @param id   The id of the file
**
** @return 0 if successful, -1 if not
*/
int _fl_create( int id );

/**
** Name:  _fl_open
**
** Opens a file so it can be used 
**
** @param id   The id of the file
**
** @return 0 if successful, -1 if not
*/
file_t *_fl_open( int id );

/**
** Name:  _fl_delete
**
** Deletes a file from the disk
**
** @param id   The id of the file
**
** @return 0 if successful, -1 if not
*/
int _fl_delete( int id );

/**
** Name:  _fl_close
**
** Saves an open file to the disk
**
** @param file      The i-node of the file
**
** @return 0 if successful, -1 if not
*/
int _fl_close( file_t *file );

/**
** Name:  _fl_read
**
** Reads contents of a file to a buffer
**
** @param file      The i-node of the file
** @param buf       The buffer to be written to
**
** @return Number of characters written to the buffer
*/
int _fl_read( file_t *file, char *buf);

/**
** Name:  _fl_write
**
** Writes contents of the buffer to a file
**
** @param file      The i-node of the file
** @param buf       The buffer containing stuff to write 
** @param buf_size  Number of characters in the buffer
**
** @return 0 if successful, -1 if not
*/
int _fl_write( file_t *file, char *buf, int buf_size );

#endif
/* SP_ASM_SRC */

#endif
