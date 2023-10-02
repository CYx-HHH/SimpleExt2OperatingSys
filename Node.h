#pragma once
#define N_Blocks 1024
#define N_Inodes 1024*32 
#define MAX_NAME_LEN 11


typedef struct{
	int i_size;// �ļ���С;
	int i_ptr; /// ����;
}Inode;

class GroupDesc {
public:
	GroupDesc();
	int freeBlock;  ///�׸� δʹ�����ݿ�λ��
	int freeNode;
};

class SuperBlock {
public:
	SuperBlock();
	int nblocks; /// blocks ��Ŀ
	int blocks_per_group;///  1024
	int inodes_per_group; ///1024*32
	int group_count;     ///  1
};


//FILE* buffer;

typedef struct MyFile {
	char name[MAX_NAME_LEN];
	int size;
	int blockloc; //  block ��
	int nodeloc; // inode ��
	struct MyFile* nextFile;
	char* content;        ///*** �ļ���С����� 256 inodes   8 blocks
}MyFile;

///*** 11+20+...(�ļ�����)

typedef struct MyDir {
	char name[MAX_NAME_LEN];
	int size;
	MyDir* nextdir;  //���Ŀ¼               ***�ֵ�
	MyDir* predir;   //ǰ��Ŀ¼               ***����          
	MyFile* filePTR;//��Ŀ¼�µ��ļ�����ָ��  ***����
	MyDir* dirPTR;  //��Ŀ¼�µ�Ŀ¼����ָ��  ***����
	int nodeloc;
	int filecount;
	int dircount;
}MyDir;

///*** һ��Ŀ¼  2 node, 64 byte. 11+12+16+...= 39+...(�����д���ӵ�Ŀ¼)
///***  ÿ���ļ�����ռ 2 node  ���� ... 1 block   ??????
