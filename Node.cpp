#include"Node.h"
#include<string>

void init_inode(Inode inode)
{
	inode.i_size = 0;
	inode.i_ptr = 0;
}

GroupDesc::GroupDesc()
{
	this->freeBlock = 0;
	this->freeNode = 0;
}

SuperBlock::SuperBlock()
{
	this->group_count = 1;
	this->blocks_per_group = 1024;
	this->inodes_per_group = N_Inodes;
	this->nblocks = N_Blocks;
}
