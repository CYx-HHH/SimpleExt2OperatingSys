#pragma once
#define N_Blocks 1024
#define N_Inodes 1024*32 
#define MAX_NAME_LEN 11


typedef struct{
	int i_size;// 文件大小;
	int i_ptr; /// 索引;
}Inode;

class GroupDesc {
public:
	GroupDesc();
	int freeBlock;  ///首个 未使用数据块位置
	int freeNode;
};

class SuperBlock {
public:
	SuperBlock();
	int nblocks; /// blocks 数目
	int blocks_per_group;///  1024
	int inodes_per_group; ///1024*32
	int group_count;     ///  1
};


//FILE* buffer;

typedef struct MyFile {
	char name[MAX_NAME_LEN];
	int size;
	int blockloc; //  block 号
	int nodeloc; // inode 号
	struct MyFile* nextFile;
	char* content;        ///*** 文件大小最多是 256 inodes   8 blocks
}MyFile;

///*** 11+20+...(文件内容)

typedef struct MyDir {
	char name[MAX_NAME_LEN];
	int size;
	MyDir* nextdir;  //后继目录               ***兄弟
	MyDir* predir;   //前继目录               ***父亲          
	MyFile* filePTR;//该目录下的文件链表指针  ***儿子
	MyDir* dirPTR;  //该目录下的目录链表指针  ***儿子
	int nodeloc;
	int filecount;
	int dircount;
}MyDir;

///*** 一层目录  2 node, 64 byte. 11+12+16+...= 39+...(其他有待添加的目录)
///***  每个文件至少占 2 node  至多 ... 1 block   ??????
