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

void _blk_init();

int _blk_alloc( int num );

int _blk_free( int id );

int _blk_load_file( int id, file_t *file );

int _blk_save_file( int id, file_t *file );

int _blk_load_filecontents( int id, char *buf, int num_blocks );

int _blk_save_filecontents( int id, char *contents, int num_blocks );

#endif
/* SP_ASM_SRC */

#endif
