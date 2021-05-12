/**
** @file block.c
**
** @author Utkarsh Dayal CSCI-452 class of 20205
**
** File to handle block operations
*/

#define	SP_KERNEL_SRC

#include "common.h"

/*
** PRIVATE DEFINITIONS
*/
// size of file inode in bytes
#define FILE_INODE_SIZE 12

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/
uint32_t *bit_map;
Block *block_list;
int block_count;

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/
int is_allocated( int index ){
    return (bit_map[index / 32] & (1 << (index % 32))) != 0;
}

void alloc_block( int index ){
    bit_map[index / 32] |= 1 << (index % 32);
}

/*
** PUBLIC FUNCTIONS
*/

int free_block( int index ){
    bit_map[index / 32] &= ~(1 << (index % 32));
    return SUCCESS;
}

int alloc_blocks( int num ){
    
    // index of block being processed
    int idx = 0;
    // number of consecutive free blocks found
    int free_blocks = 0; 

    // go through all the blocks
    while ( index < block_count ){
    
        // check if block is allocated
        if ( !is_allocated( idx ) ){
	    free_blocks++;

	    // check if we've found enough consecutive free blocks
	    if ( free_blocks == block_count ){

	        // index at which the consecutive blocks start
	        int start_index = index - block_count + 1;
		// allocate the blocks
		int count = 0;
		while( count < block_count ){
		    alloc_block( start_index + count );
		    count++;
		}
		// return the block at which the blocks start
		return start_index;
	    }
	} else {
	    // consecutive free blocks go down to zero
	    free_blocks = 0;
	}
	idx++;
    }

    // Out of blocks?????????
    return E_FAILURE;
}

int save_file( int id, File *file ){
    
    // get the block
    Block block = block_list[id];

    // write it to the disk
    bool_t result = writeDisk( block.device, block.startl, block.starth, NUM_SECTORS, (uint16_t *) file );

    // check result of write
    if ( !result ){
        return E_FAILURE;
    }

    return SUCCESS; 
}

int load_file( int id, File *file ){
    
    // get the block
    Block block = block_list[id];

    uint16_t *buf = _km_slice_alloc( 1 );

    // read it from the disk
    bool_t result = readDisk( block.device, block.startl, block.starth, NUM_SECTORS, buf );

    // check result of write
    if ( !result ){
        return E_FAILURE;
    }

    file = ( File *) buf;

    return SUCCESS; 
}

/**
** Name:  ?
**
** ?
**
** @param ?    ?
**
** @return ?
*/
