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
#define NUM_PAGES 3

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
typedef union block_container {
    
} block_contents;

typedef struct block_node {
    int id,
    int indirect,
    block_contents *contents
} Block;

/*
** Globals
*/

/*
** Prototypes
*/

Block load_block( int id );

int save_block( Block block );

#endif
/* SP_ASM_SRC */

#endif
