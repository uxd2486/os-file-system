/**
** @file block.h
**
** @author Utkarsh Dayal CSCI-452 class of 20205
**
** Function definitions for block usage.
*/

#ifndef BLOCK_H_
#define BLOCK_H_

#include "file.h"

/*
** General (C and/or assembly) definitions
**
** This section of the header file contains definitions that can be
** used in either C or assembly-language source code.
*/
#define BLOCK_SIZE 1024
#define NUM_SECTORS 2

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
** Stores info about each block in the disk
*/
typedef struct block_node {
    int id;
    hddDevice_t device;
    uint32_t startl;
    uint32_t starth;
} block_t;

/*
** Globals
*/

/*
** Prototypes
*/

/**
** Name:  _blk_init
**
** Initializes all the blocks in the disk
**
*/
void _blk_init();

/**
** Name:  _blk_alloc
**
** Allocates a given number of continous disk blocks using the bit-map
**
** @param num    The number of disk blocks to be allocated
**
** @return id of the first disk block 
*/
int _blk_alloc( int num );

/**
** Name:  _blk_free
**
** Frees a single disk blocks using the bit-map, given the id
**
** @param index    The id of the block to be freed
**
*/
void _blk_free( int index );

/**
** Name:  _blk_load_file
**
** Given the block of the i-node, loads the file i-node from the disk
**
** @param id          The id of the i-node block of the file
** @param file        Pointer where i-node needs to be stored
**
** @return 0 if successful, -1 if not
*/
int _blk_load_file( int id, file_t *file );

/**
** Name:  _blk_save_file
**
** Given the i-node of a file and the block it, saves it to the disk
**
** @param id          The id of the i-node block of the file
** @param file        Pointer where i-node needs to is stored
**
** @return 0 if successful, -1 if not
*/
int _blk_save_file( int id, file_t *file );

/**
** Name:  _blk_load_filecontents
**
** Given the contents of a file and the starting block, loads the file 
** contents from the disk
**
** @param id          The id of the starting block of the file
** @param contents    Buffer where contents are to be written
** @param num_blocks  The number of blocks to be read
**
** @return 0 if successful, -1 if not
*/
int _blk_load_filecontents( int id, char *buf, int num_blocks );

/**
** Name:  _blk_save_filecontents
**
** Given the contents of a file and the starting block, stores the file 
** contents onto the disk
**
** @param id          The id of the starting block of the file
** @param contents    Buffer containing contents of the file
** @param num_blocks  The number of blocks to be written
**
** @return 0 if successful, -1 if not
*/
int _blk_save_filecontents( int id, char *contents, int num_blocks );

#endif
/* SP_ASM_SRC */

#endif
