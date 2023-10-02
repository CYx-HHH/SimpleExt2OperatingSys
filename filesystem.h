#pragma once
#include"Disk.h"
#include"Node.h"

using namespace std;




class FileSystem {
public:
	FileSystem();

	void format();
	MyFile* readfile(MyFile*f);

	bool login();
	bool check(char* uname, char* passwd);
	void logout();

	void update_bitmap();
	void update_writein(int nodeloc);
	void node_in_use(int nodeloc);

	int newFile(char *ch);
	int newDir(char *ch);

	int del_file(MyFile* file);
	int delFile(char* ch);
	int del_dir(MyDir* dir);
	int delDir(char* ch);

	int write_file(MyFile* file, int nodeloc);
	int Write_File(MyFile* file, char* ch);

	MyDir* Find_path(char* path);
	MyDir* Find_subpath(char* path);
	MyDir* find_dir(MyDir* d, char* dir);

	//int renameDir(char* dir, char* ch); ///*** ��Ŀ¼�ش���
	int renameFile(char* file, char* ch);///*** ���ļ��ش���

	MyFile* copy_file(MyFile* file);
	MyFile* copyFile(char* file);
	int pasteFile(MyDir* path);

	void remove(char* file, char* path1, char* path2);

	void echo(char* s, char* file);
	void more(char* file);

	int import_file(char* file, char* path);///*** -->���������ļ����Ƶ���ǰĿ¼
	int export_file(char* file, char* path);///*** -->��ǰĿ¼�ļ����Ƶ���������·��
	

	string showCurrentDir();
	void ls();  
	bool cd(char* path);


	void remove_disk(bool isremove);


	MyDir* currentDir;
	MyDir root;
	char username[32]; ///*** �޳�32
	char pwd[32];   ///*** �޳�32
private:
//	int allocate();

	Disk disk;
	SuperBlock super_block; /// 1node
	GroupDesc group_desc;   /// 1node       --- 2
	bool block_bitmap[N_Blocks]; ///4   1024/32 = 32node--- 34 
	bool inode_bitmap[N_Inodes]; ///4*32 1024*32 = 1024node ---1058
	bool bitmap[N_Blocks][32];    ///     1024*32 = 1024node ---2082
	Inode Node[1024 * 32];        ///node table
	int file_count;              ///     
	int dir_count;               ///        16byte 
	MyDir Dir[3052];             ///*** ��� 3052 ��Ŀ¼�ṹ  10280--- 16386
	MyFile File[4095];           ///***��� 4095 ��Ŀ¼�ṹ   16387--- 24576      768 blocks
	MyFile* copytempfile;

};