#pragma once
#include"Node.h"
#include<fstream>
using namespace std;



class Disk {
public:
	Disk();

	void write(SuperBlock* block);
	void write(int s_byte,Inode* inode, int nodeNum, int bufsize);
	void write(int nodeNum, GroupDesc* group);
	void stop(bool removeDisk);
	void write(int nodeNum, bool* bitmap, int len);
	void write(int s_byte, char* buffer, int nodeNum, int bufsize);

	void read(SuperBlock* block);
	void read(int s_byte,int nodeNum, Inode* inode, int bufsize);
	void read(int nodeNum, GroupDesc* group);
	void read(int nodeNum, bool* bitmap, int len);
	void read(int s_byte, char* buffer, int nodeNum, int bufsize);

	void write(int s_byte, MyDir* d, int nodeNum);
	void write(int s_byte, MyFile* f, int nodeNum);
	void read(int s_byte, MyDir* d, int nodeNum);
	void read(int s_byte, MyFile* f, int nodeNum);


	static const int MEM_SIZE = 1024 * 1024;
	static const int BLOCK_SIZE = 1024;
	static const int NUM_BLOCKS = 1024;
	static const int NODE_SIZE = 32;
	bool ifread;
	fstream* disk_int;
	//byte buf_int[BLOCK_SIZE];
};
