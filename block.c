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

void _blk_init(){
    
    // get the hdd devices
    _hddDeviceList_t list = _get_device_list();

    // go through devices and count the sectors
    int sector_count = 0
    for ( int i = 0; i < list.count; i++ ){
        hddDevice_t device = list[i];
	sector_count += device.sector_count;
    }

    // calculate number of blocks that can be made
    block_count = sector_count / NUM_SECTORS;

    // allocate memory to store block info
    int block_mem = block_count * sizeof( Block );
    int num_pages = block_mem / PAGE_SIZE;
    block_list = ( Block * ) _km_page_alloc( num_pages );

    // assign sectors to blocks
    int count = 0;
    for ( int i = 0; i < list.count; i++ ){
        hddDevice_t device = list[i];
        for ( int j = 0; j < device.sector_count; j += NUM_SECTORS ){
	    // create the block
	    Block *block = ( Block * ) _km_slice_alloc( 1 );
	    block->id = count;
	    block->device = device;
	    block->startl = (uint32_t) (j & 0xff);
	    block->starth = (uint32_t) (j >> 8);

	    // put the block in the list
	    block_list[count] = *block;

	    // free the memory used
	    _km_slice_free( block );
	}
    }

    // calculate size of the bitmap
    int map_mem = ( ( block_count / 32 ) + ( ( block_count % 32 ) != 0 ) * 4);
    // calcualte number of pages for the bitmap
    int num_pages = ( map_mem / PAGE_SIZE ) + ( ( map_mem % PAGE_SIZE ) != 0);
    // allocate the bitmap
    bit_map = ( uint32_t * ) _km_page_alloc( num_pages );
}
    

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
    bool_t result = readDisk( block.device, block.startl, block.starth,\
    NUM_SECTORS, buf );

    // check result of write
    if ( !result ){
        return E_FAILURE;
    }

    file = ( File *) buf;

    return SUCCESS; 
}

int load_filecontents( int id, char *buf, int num_blocks ){
    
    // for passing in to the disk driver
    char *buf_ptr = buf;

    // go through each block
    for( int i = id; i < id + num_blocks; i++ ){
        
	// get the block
        Block block = block_list[i];

	// read block from the disk
        bool_t result = readDisk( block.device, block.startl, block.starth,\
        NUM_SECTORS, ( uint16_t *) buf_ptr );
        
	// check result of write
        if ( !result ){
            return E_FAILURE;
        }
	
	// this now points to the next part of buffer to be filled
	buf_ptr += BLOCK_SIZE;
    }

    return SUCCESS;
}

int save_filecontents( int id, char *contents, int num_blocks ){
    
    // for passing in to the disk driver
    char *buf_ptr = buf;

    // go through each block
    for( int i = id; i < id + num_blocks; i++ ){
        
	// get the block
        Block block = block_list[i];

	// write block to the disk
        bool_t result = writeDisk( block.device, block.startl, block.starth,\
        NUM_SECTORS, ( uint16_t *) buf_ptr );
        
	// check result of write
        if ( !result ){
            return E_FAILURE;
        }
	
	// this now points to the next part of buffer to be written
	buf_ptr += BLOCK_SIZE;
    }

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
