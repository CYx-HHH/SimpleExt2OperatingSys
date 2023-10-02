#pragma warning(disable:4996)
#include"Disk.h"

#include<stdlib.h>
#include<fstream>
#include<string>
#include<iostream>

using namespace std;

extern int start_nodeloc;

Disk::Disk()
{
	disk_int = new fstream();
	disk_int->open("DISK", ios::binary | ios::in);
	
	if (!disk_int->is_open())
	{
		disk_int->clear();
		ifread = false;

	}
	else
	{
		ifread = true;
		disk_int->close();
	}
}

void Disk::write(SuperBlock* block)
{
	disk_int->open("DISK", ios::binary | ios::out|ios::app);
	char* ch = new char[9];
//	disk_int->seekp(0, ios::beg);
	
	ch = itoa(block->nblocks, ch, 10);
	disk_int->write(reinterpret_cast<char*>(ch), sizeof(int));
	memset(ch, 0, 9);
	ch = itoa(block->inodes_per_group, ch, 10);
	disk_int->write(reinterpret_cast<char*>(ch), 5);
	memset(ch, 0, 9);
	ch = itoa(block->blocks_per_group, ch, 10);
	disk_int->write(reinterpret_cast<char*>(ch), sizeof(int));
	memset(ch, 0, 9);
	ch = itoa(block->group_count, ch, 10);
	disk_int->write(reinterpret_cast<char*>(ch), sizeof(int));
	
	char c[] = " ";
	disk_int->write(reinterpret_cast<char*>(&c), 15);
	
	disk_int->close();
	delete[]ch;
}

void Disk::write(int s_byte,Inode* inode,int nodeNum,int bufsize)
{
	disk_int->open("DISK", ios::binary | ios::out | ios::in);

	disk_int->seekp(nodeNum * NODE_SIZE + s_byte, ios::beg);

	disk_int->write(reinterpret_cast<char*>(inode), sizeof(Inode));

	disk_int->close();
}

void Disk::write(int nodeNum, GroupDesc *group)
{
	disk_int->open("DISK", ios::binary | ios::out | ios::in);
	char* ch = new char[9]{ 0 };
	disk_int->seekp(nodeNum * NODE_SIZE, ios::beg);

	memset(ch, 0, 9);
	ch = itoa(group->freeBlock, ch, 10);
	disk_int->write(reinterpret_cast<char*>(ch), sizeof(int));

	memset(ch, 0, 9);
	ch = itoa(group->freeNode, ch, 10);
	disk_int->write(reinterpret_cast<char*>(ch), 5);
	delete[]ch;
	char c[] = " ";
	disk_int->write(reinterpret_cast<char*>(&c), 14);
	disk_int->close();
}

void Disk::write(int nodeNum, bool* bmap,int len)
{
	disk_int->open("DISK", fstream::binary | ios::out | ios::in);
	disk_int->seekp(nodeNum * NODE_SIZE, ios::beg);
	
	disk_int->write(reinterpret_cast<char*>(bmap), sizeof(bool) * len);

	int t = 0;
	if (len % 32 == 0)
		t = len / 32;
	else t = len / 32 + 1;
	char c[] = " ";
	disk_int->write(reinterpret_cast<char*>(&c), t * 32 - len);
	disk_int->close();
}

void Disk::write(int s_byte, char* buffer, int nodeNum, int bufsize)
{
	disk_int->open("DISK", ios::binary | ios::out | ios::in);
	disk_int->seekp(nodeNum * NODE_SIZE + s_byte, ios::beg);

	disk_int->write(reinterpret_cast<char*>(buffer), bufsize);

	disk_int->close();
}

void Disk::stop(bool removeDisk)
{
	if (disk_int->is_open())
		disk_int->close();

	if (removeDisk)
		remove("DISK");
}

void Disk::read(SuperBlock* block)
{
	disk_int->open("DISK", ios::binary | ios::in);
	block->group_count = 0;
	char* ch = new char[9]{ 0 };
	int t = 0;
	disk_int->seekg(0, ios::beg);
	disk_int->read(reinterpret_cast<char*>(ch), sizeof(int));
	t = atoi(ch);
	block->nblocks = t;
	memset(ch, 0, 9);
	disk_int->read(reinterpret_cast<char*>(ch), 5);
	t = atoi(ch);
	block->inodes_per_group = t;
	memset(ch, 0, 9);
	disk_int->read(reinterpret_cast<char*>(ch), sizeof(int));
	t = atoi(ch);
	block->blocks_per_group = t;
	memset(ch, 0, 9);
	disk_int->read(reinterpret_cast<char*>(ch), sizeof(int));
	t = atoi(ch);
	block->group_count = t;
	disk_int->close();
	delete[]ch;
}

void Disk::read(int s_byte,int nodeNum, Inode* inode,int bufsize)
{
	disk_int->open("DISK", ios::binary | ios::in);
	int t = 0;
	disk_int->seekg(nodeNum * NODE_SIZE + s_byte, ios::beg);
	disk_int->read(reinterpret_cast<char*>(inode), bufsize);

	disk_int->close();
}

void Disk::read(int nodeNum, GroupDesc* group) 
{
	disk_int->open("DISK", ios::binary | ios::in);
	char* ch = new char[9]{ 0 };
	int t = 0;

	disk_int->seekg(nodeNum * NODE_SIZE, ios::beg);

	memset(ch, 0, 9);
	disk_int->read(reinterpret_cast<char*>(ch), sizeof(int));
	t = atoi(ch);
	group->freeBlock = t;
	memset(ch, 0, 9);
	disk_int->read(reinterpret_cast<char*>(ch), 5);
	t = atoi(ch);
	group->freeNode = t;
	
	disk_int->close();
	delete[]ch;
}

void Disk::read(int nodeNum, bool* bmap, int len)
{
	disk_int->open("DISK", ios::binary | ios::in);

	disk_int->seekg(nodeNum * NODE_SIZE, ios::beg);

	disk_int->read(reinterpret_cast<char*>(bmap), sizeof(bool) * len);
	/*
	for (int i = 0; i < len; ++i)
	{
		disk_int->read(reinterpret_cast<char*>(ch), sizeof(bool));
		t = atoi(ch);
		bmap[i] = t;
	}
	*/
	disk_int->close();
}

void Disk::read(int s_byte, char* buffer, int nodeNum, int bufsize)
{
	disk_int->open("DISK", ios::binary | ios::in | ios::out);
	disk_int->seekg(nodeNum * NODE_SIZE + s_byte, ios::beg);
	disk_int->read(reinterpret_cast<char*>(buffer), sizeof(char) * bufsize);
	disk_int->close();
}






void Disk::write(int s_byte, MyDir* d, int nodeNum)
{
	disk_int->open("DISK", ios::binary | ios::out | ios::in);
	disk_int->seekp(nodeNum * NODE_SIZE + s_byte, ios::beg);
//	cout << disk_int->tellp() << " dd" << endl;
	disk_int->write(reinterpret_cast<char*>(d), sizeof(MyDir));

	int t = 0;
	int len = sizeof(MyDir);
	if (len % 32 == 0)
		t = len / 32;
	else t = len / 32 + 1;
	char c = '\0';
	disk_int->write(reinterpret_cast<char*>(&c), t * 32 - len);
//	cout << disk_int->tellp() << " ddd" << endl;
	disk_int->close();
}

void Disk::write(int s_byte, MyFile* f, int nodeNum)
{
	disk_int->open("DISK", ios::binary | ios::out | ios::in);
//	cout << disk_int->tellp() << " ff" << endl;
	disk_int->seekp(nodeNum * NODE_SIZE + s_byte, ios::beg);
//	cout << disk_int->tellp() <<" ff" <<endl;

	disk_int->write(reinterpret_cast<char*>(f),sizeof(MyFile));

	int t = 0;
	int len = sizeof(MyFile);
	if (len % 32 == 0)
		t = len / 32;
	else t = len / 32 + 1;
	char c = '\0';
	disk_int->write(reinterpret_cast<char*>(&c), t * 32 - len);
//	cout << disk_int->tellp() << endl << endl;
	disk_int->close();
}

void Disk::read(int s_byte, MyDir* d, int nodeNum)
{
	disk_int->open("DISK", ios::binary | ios::in);
	//cout << disk_int->tellg() <<"dd"<< endl;
	disk_int->seekg(nodeNum * NODE_SIZE + s_byte, ios::beg);
	//cout << disk_int->tellg() << endl;
	disk_int->read(reinterpret_cast<char*>(d), sizeof(MyDir));
	//cout << disk_int->tellg() << endl << endl << endl;
	disk_int->close();
}

void Disk::read(int s_byte, MyFile* f, int nodeNum)
{
	disk_int->open("DISK", ios::binary | ios::in);
	disk_int->seekg(nodeNum * NODE_SIZE + s_byte, ios::beg);
	//cout << disk_int->tellg() << "  fff"<<endl;
	disk_int->read(reinterpret_cast<char*>(f), sizeof(MyFile));
	//cout << disk_int->tellg() << endl << endl;
	disk_int->close();
}




