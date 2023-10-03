#pragma warning(disable:4996)
#include"filesystem.h"
#include<cstring>
#include<conio.h>
#include<iostream>
using namespace std;

bool isLogin;
int file_content_loc = 24576;


string error[] = { "/","\\", ":","<",">","|","*","&" };  //命名中的非法字符

FileSystem::FileSystem()
{
	memset(block_bitmap, 0, sizeof(bool) * N_Blocks);
	memset(inode_bitmap, 0, sizeof(bool) * N_Inodes);
	for (int i = 0; i < N_Blocks; ++i)
	{
		for (int j = 0; j < 32; ++j)
		{
			bitmap[i][j] = false;
		}
	}
	if (disk.ifread)
	{
		disk.read(&super_block);
		disk.read(1, &group_desc);
		disk.read(2, block_bitmap, N_Blocks);
		disk.read(34, inode_bitmap, N_Inodes);
		for (int i = 0; i < 1024; ++i)
			disk.read(1058, bitmap[i], 32);

		disk.read(0,username, 2082, 12);
		disk.read(12,pwd, 2082, 20);
		disk.read(0,2083, Node, 1024 * 32 * sizeof(Inode));
		///***  开始读 DIR 
		currentDir = &root;
		disk.read(0,(char*)&file_count, 10275, 10);
		disk.read(10, (char*)&dir_count, 10275, 22);
		disk.read(0, &root, 10276);
		for (int i = 0; i < 3053; ++i)
			disk.read(0, Dir+i, 10278 + i * 2);
		for (int i = 0; i < 4096; ++i)
		{
			disk.read(0, File + i, 16384 + i * 2);
		}

	}
	else
		format();
	copytempfile = NULL;
}

void FileSystem::format()
{
	disk.write(&super_block);
	group_desc.freeBlock = 768;
	group_desc.freeNode = 24576;
	disk.write(1, &group_desc);
	for (int i = 0; i < 768; ++i)
		block_bitmap[i] = 1;
	memset(inode_bitmap, 1, sizeof(bool) * 24576);
	disk.write(2, block_bitmap, N_Blocks);
	disk.write(34, inode_bitmap, N_Inodes);

	for (int i = 0; i < N_Blocks; ++i)
	{
		for (int j = 0; j < 32; ++j)
			bitmap[i][j] = 0;
	}
	for (int i = 0; i < 768; ++i)
	{
		for (int j = 0; j < 32; ++j)
			bitmap[i][j] = 1;
	}

	for (int i = 0; i < 1024; ++i)
		disk.write(1058, bitmap[i], 32);

	strcpy(username, "root");
	strcpy(pwd, "123456");
	disk.write(0,username, 2082, 12);
	disk.write(12,pwd, 2082, 20);

	disk.write(0, Node, 2083, 1024 * 32 * sizeof(Inode));

	//10276 nodes
	file_count = 0;
	dir_count = 0;
	disk.write(0, (char*)&file_count, 10275, 10);
	disk.write(10, (char*)&dir_count, 10275, 22);

	//写 DIR FILE
	root.dirPTR = nullptr;
	root.filePTR = nullptr;
	root.nextdir = nullptr;
	root.predir = nullptr;
	root.dircount = 0;
	root.filecount = 0;
	root.size = 0;
	root.nodeloc = 10276;
	strcpy(root.name, "root");
	currentDir = &root;
	disk.write(0, &root, 10276);
	                             ///***24+32*190= 6104 nodes ---512 blocks.
	for (int i = 0; i < 3053; ++i)
	{
		Dir[i].nodeloc = -1;
		disk.write(0, Dir + i, 10278 + i * 2);
	}
                                   ///***16387  -----   24576   total: 32768-1
	for (int i = 0; i < 4096; ++i) 
	{
		File[i].nodeloc = -1;
		File[i].nextFile = NULL;
		File[i].content = NULL;
		disk.write(0, File + i, 16384 + i * 2);
	}
	char ch = '\0';
	disk.write(0, &ch, 24576, 8192 * 32);
}

void inPasswd(char passwd[])	//输入密码
{
	int plen = 0;
	char c;
	printf("passwd:");
    fflush(stdout);

	while (c = getch()){
		if (c == '\r') {	//输入回车，密码确定
			passwd[plen] = '\0';
			break;
		}
		else if (c == '\b') {	//退格，删除一个字符
			if (plen > 0) {	//没有删到头
				plen--;
                printf("\b \b");
                fflush(stdout);
			}//else{ fflush(stdin);}
		}
		else {	//密码字符
			passwd[plen++] = c;
            printf("*");
            fflush(stdout);
		}
	}
}

void inUsername(char username[])	//输入用户名
{
	printf("username:");
    fflush(stdout);	//清空缓冲区

	scanf("%s", username);	//用户名
    fflush(stdin);	//清空缓冲区
}

bool FileSystem::check(char* uname, char* passwd)
{
	if (strcmp(uname, this->username) != 0)
	{
		cout << "USER NOT EXIST			-FALSE" << endl;
		return false;
	}	
	if (strcmp(passwd, this->pwd) != 0)
	{
		cout << "Login Failed			-FALSE" << endl;
		return false;
	}return true;
}

bool FileSystem::login()	//登陆界面
{
	char uname[100] = { 0 };
	char passwd[100] = { 0 };
	inUsername(uname);	//输入用户名
	inPasswd(passwd);		//输入用户密码
    cout<<endl;
	if (check(uname,passwd)) {	//核对用户名和密码
		isLogin = true;
		cout << "Login Success			-OK" << endl;
		return true;
	}
	else {
		isLogin = false;
		return false;
	}
}

void FileSystem::logout()	//用户注销
{
	//回到根目录
	currentDir = &root;
	isLogin = false;
	printf("用户注销\n");
	system("pause");
	system("cls");
}

void FileSystem::update_bitmap()
{
	int f = 0, j = 0;
	for (int i = 768; i < N_Blocks; ++i)
	{
		for (j = 0; j < 32; ++j)
		{
			if (inode_bitmap[i * 32 + j] == 0)
			{
				f = 1;
				group_desc.freeNode = i * 32 + j;
				bitmap[i][j] = 0;
				break;
			}
			else
				f = 0;
		}
		if (f)
		{
			block_bitmap[i] = 0;
			group_desc.freeBlock = i;
			break;
		}
		if (i == N_Blocks && j == 31)
		{
			cout << "OUT OF MEMORY			-FALSE" << endl;
		}
	}
	disk.write(1, &group_desc);
	return;
}
void FileSystem::update_writein(int nodeloc)
{
	disk.write(nodeloc * sizeof(bool), (char*)inode_bitmap, 34, 1);
	disk.write(nodeloc / 32 * sizeof(bool), (char*)block_bitmap, 2, 1);
	disk.write(nodeloc / 32 * sizeof(bool), (char*)block_bitmap, 2, 1);
	disk.write(1058 + nodeloc % 32, bitmap[nodeloc / 32], 1);
	return;
}

void FileSystem::node_in_use(int nodeloc)
{
	inode_bitmap[nodeloc] = 1;
	int bloc = nodeloc / 32;

	if (block_bitmap[bloc] == 0)
		group_desc.freeBlock = bloc + 1;
	block_bitmap[bloc] = 1;
	bitmap[bloc][nodeloc % 32] = 1;
	return;
}

MyDir* FileSystem::Find_path(char* path)///***   /home/etc/ 从root开始找
{                                       ///*** /home/etc
	char ch[] = "/";
	char* res = nullptr;
	MyDir* tmp = NULL;

	if (path[0] == '.' && path[1] == '.' && path[2] != '.') ///***  防止出现  .../ 或 ...../ 
	{
		tmp = currentDir->predir;
		if (!tmp)
		{
			cout << "DIR NOT EXIST			-FALSE" << endl;
			return NULL;
		}
	}
	else if (path[0] == '.' && path[1] != '.')
	{
		tmp = currentDir;
	}

	res = strtok(path, ch);

	if (tmp == NULL) {
		tmp = find_dir(&root, res);
		if (!tmp) {
			//cout << "DIR NOT EXIST			-FALSE" << endl;
			return NULL;
		}
	}

	while (res != NULL)            ///*** 验证下 ../aaa 和./ff  
	{                               ///*** 不知道dir结构是否可读  file的读取还没写
		res = strtok(NULL, ch);
		if (res)
		{
			if (res[strlen(res) - 1] == '/')
			{
				res[strlen(res) - 1] = '\0';
			}
			tmp = find_dir(tmp, res);
			if (!tmp) {
				//cout << "DIR NOT EXIST			-FALSE" << endl;
				return NULL;
			}
		}
	}
	if (tmp)
		return tmp;
	else
	{
		//cout << "DIR NOT EXIST			-FALSE" << endl;
		return NULL;
	}
}

MyDir* FileSystem::Find_subpath(char* path) ///*** 查找子目录 
{
	char ch[] = "/";
	char* res = nullptr;
	int f = 0;
	if (path[0] == '/')
		f = 1;
	if (path[0] == '.' && path[2] != '.') ///***  防止出现  .../ 或 ...../ 
	{
		path = path + 2;
	}
	
	res = strtok(path, ch);
	MyDir* tmp = NULL;

	while (res != NULL)
	{
		if (!f)
			res = strtok(NULL, ch);
		if (res)
		{
			if (res[strlen(res) - 1] == '/')
			{
				res[strlen(res) - 1] = '\0';
			}
			tmp = find_dir(currentDir, res);
			if (!tmp) {
				//cout << "DIR NOT EXIST			-FALSE" << endl;
				return NULL;
			}
		}f = 0;
	}
	if (tmp)
		return tmp;
	else
	{
		//cout << "DIR NOT EXIST			-FALSE" << endl;
		return NULL;
	}
}

MyDir* FileSystem::find_dir(MyDir* d, char* dir) /// d以及d的子目录
{
	if (!d)
	{
		return NULL;
	}
	else
	{
		if (strcmp(d->name, dir) == 0)
			return d;
		MyDir* tmp = d->dirPTR;
		if (!tmp)
		{
			return NULL;
		}
		while (tmp)
		{
			if (strcmp(tmp->name, dir) == 0)
				return tmp;
			tmp = tmp->nextdir;
		}
		return NULL;
	}
}

MyDir* find_dirloc(MyDir* dir,int num,int &loc)
{
	for (int i = 0; i < num; ++i)
	{
		if (dir[i].nodeloc == -1)
		{
			loc = i;
			return dir + i;
		}
	}
	return NULL;
}

MyFile* find_fileloc(MyFile* file, int num, int& loc)
{
	for (int i = 0; i < num; ++i)
	{
		if (file[i].nodeloc == -1)
		{
			loc = i;
			return file + i;
		}
	}
	return NULL;
}

MyFile* FileSystem::readfile(MyFile* f)
{
	Inode lastnode;
	int loc = f->nodeloc;
	int cnt = 0;
	char ch[8192 * 32]{ 0 };
	int cloc = 0;
	while (Node[loc].i_ptr != -1)
	{
		cnt++;
		loc = Node[loc].i_ptr;
		lastnode = Node[loc];
		if (cnt >= 2)
		{
			disk.read(0, ch+cloc, loc, 32);
			cloc += 32;
		}
	}
	if (cnt == 1)return f;
	if (f->content)
	{
		delete[]f->content;
		f->content = NULL;
	}
	f->content = new char[strlen(ch)];
	strcpy(f->content, ch);
	return f;
}

int FileSystem::newDir(char* ch)
{
	string tempname = ch;
	for (int i = 0; i < 8; ++i)
	{
		if (tempname.find(error[i], 0) != string::npos)
		{
			cout << "RENAME			-FALSE" << endl;
			return 0;
		}
	}
	MyDir* p, * h;
	int loc = 0;
	p = find_dirloc(Dir, 3053, loc);
	strcpy(p->name, ch);
	p->dirPTR = nullptr;
	p->size = 0;
	p->filePTR = nullptr;
	p->nextdir = nullptr;
	p->nodeloc = 10278 + loc * 2;
	p->dircount = 0;
	p->filecount = 0;
	currentDir->dircount++;
	dir_count++;

	h = currentDir->dirPTR;
	while (h != nullptr)
	{
		if (strcmp(h->name, p->name) == 0)
		{
			cout << "DIR EXISTS			-FALSE" << endl;
			return 0;
		}
		h = h->nextdir;
	}
	p->predir = currentDir;
	p->nextdir = currentDir->dirPTR;
	currentDir->dirPTR = p;
	
	cout << "CREATE			-OK" << endl;

	if (currentDir->nodeloc == root.nodeloc)
		disk.write(0, &root, 10276);
	else
		disk.write(0, Dir + (currentDir->nodeloc - 10278) / 2, currentDir->nodeloc);
	disk.write(0, Dir + loc, 10278 + loc * 2);
	disk.write(10, (char*)&dir_count, 10275, 22);

	return 1;
}

int FileSystem::newFile(char *ch)
{
	string tempname = ch;
	for (int i = 0; i < 8; ++i)
	{
		if (tempname.find(error[i], 0) != string::npos)
		{
			cout << "RENAME			-FALSE" << endl;
			return 0;
		}
	}
	int loc = 0;
	MyFile* p = nullptr;
	p = find_fileloc(File, 4095, loc);
	if (p == NULL)
	{
		cout << "CREATE			-FALSE" << endl;
		return 0;
	}
	strcpy(p->name, ch);
	MyFile* q = new MyFile;
	q = currentDir->filePTR;
	while (q != nullptr)
	{
		if (strcmp(p->name, q->name) == 0)
		{
			cout << "FILE EXISTS			-FALSE" << endl;
			return 0;
		}
		q = q->nextFile;
	}
	delete q;

	file_count++;
	p->nextFile = currentDir->filePTR;
	p->content = NULL;
	p->size = 0;
	p->nodeloc = 16384 + loc * 2;
	
	p->blockloc = p->nodeloc / 32;
	if (p->nodeloc % 32 != 0)
		p->blockloc++;
	Node[p->nodeloc].i_ptr = p->nodeloc + 1;
	Node[p->nodeloc].i_size = 32;
	Node[p->nodeloc+1].i_ptr = -1;
	Node[p->nodeloc + 1].i_size = sizeof(MyFile) - 32;
	disk.write(sizeof(Inode) * (p->nodeloc), Node + p->nodeloc, 2083, sizeof(Inode));
	disk.write(sizeof(Inode) * (p->nodeloc + 1), Node + p->nodeloc + 1, 2083, sizeof(Inode));
	currentDir->filePTR = p;
	currentDir->filecount++;

	disk.write(0, File + loc, 16384 + loc * 2);
	disk.write(0, (char*)&file_count, 10275, 10);
	
	if (currentDir == &root)
		disk.write(0, &root, 10276);
	else
		disk.write(0, Dir + (currentDir->nodeloc - 10278) / 2, currentDir->nodeloc);
	//cout << "CREATE			-OK" << endl;
	return 1;
}

int FileSystem::del_file(MyFile* file)
{
	file_count--;
	int loc = file->nodeloc;
	int cnt = 0;
	while (Node[loc].i_ptr != -1)
	{
		cnt++;
		int t = loc;
		loc = Node[loc].i_ptr;
		Node[t].i_ptr = -1;
		inode_bitmap[t] = 0;
		update_writein(t);
	}
	inode_bitmap[loc] = 0;
	Node[loc].i_ptr = -1;
	Node[loc].i_size = 0;
	if (cnt > 1)
	{
		update_bitmap();
	}
	disk.write(sizeof(Inode) * (file->nodeloc), Node + file->nodeloc, 2083, sizeof(Inode));
	disk.write(sizeof(Inode) * (file->nodeloc+1), Node + file->nodeloc+1, 2083, sizeof(Inode));
	int floc = (file->nodeloc - 16384) / 2;
	if (File[floc].content)
		delete[] File[floc].content;

	File[floc].blockloc = -1;
	File[floc].content = NULL;
	File[floc].nextFile = NULL;
	File[floc].nodeloc = -1;
	File[floc].size = 0;

	disk.write(0, (char*)&file_count, 10275, 10);
	disk.write(0, File + floc, 16384 + floc * 2);

	return 1;
}

int FileSystem::delFile(char* ch)
{
	MyFile* f, * above;
	above = nullptr;
	f = currentDir->filePTR;

	while (f != NULL)
	{
		if (!strcmp(f->name, ch))
			break;
		above = f;
		f = f->nextFile;
	}
	if (f == nullptr)
	{
		cout << "NO FILE			-FALSE" << endl;
		return 0;
	}
	int loc = 0;
	MyDir* d = currentDir;   ///*** 上级各目录大小
	while (d != 0)
	{
		d->size -= f->size;
		if (d->nodeloc == root.nodeloc)
		{
			disk.write(0, &root, 10276);
			d = d->predir;
			continue;
		}
		loc = (d->nodeloc - 10278) / 2;
		Dir[loc] = *d;
		disk.write(0, Dir + loc, d->nodeloc);
		d = d->predir;
	}
	if (f == currentDir->filePTR)
		currentDir->filePTR = currentDir->filePTR->nextFile;
	else
		above->nextFile = f->nextFile;
	del_file(f);
	cout << "DELETE			-OK" << endl;
	return 1;
}

int FileSystem::del_dir(MyDir* dir)
{
	dir_count--;
	inode_bitmap[dir->nodeloc] = 0;
	inode_bitmap[dir->nodeloc + 1] = 0;
	int dloc = (dir->nodeloc - 10278) / 2;
	int ploc = dir->predir->nodeloc;
	Dir[dloc].dircount = 0;
	Dir[dloc].dirPTR = NULL;
	Dir[dloc].filecount = 0;
	Dir[dloc].filePTR = NULL;
	Dir[dloc].nextdir = NULL;
	Dir[dloc].nodeloc = -1;     /////////###########  最关键的
	Dir[dloc].predir = NULL;
	Dir[dloc].size = 0;
	disk.write(0, Dir + ploc, 10278 + ploc * 2);
	disk.write(0, Dir + dloc, 10278 + dloc * 2);
	disk.write(10, (char*)&dir_count, 10275, 22);
	return 1;
}

int FileSystem::delDir(char* ch)
{
	MyDir* p, * pre;
	p = currentDir->dirPTR;
	pre = p;
	if (ch[0] == '/')ch += 1;
	while (p != NULL)
	{
		if (strcmp(p->name, ch) == 0)
		{
			break;
		}
		pre = p;
		p = p->nextdir;
	}
	if (p == nullptr)
	{
		cout << "DELETE			-FALSE" << endl;
		return 0;
	}
	///*******  改目录结构
	if (p == currentDir->dirPTR)
		currentDir->dirPTR = currentDir->dirPTR->nextdir;
	else
		pre->nextdir = p->nextdir;

	///*******  向上各级目录大小
	pre = currentDir;
	int loc = 0;
	while (pre != nullptr)
	{
		pre->size -= p->size;
		pre->dircount--;
		if (pre->nodeloc == root.nodeloc)
		{
			disk.write(0, &root, 10276);
			pre = pre->predir;
			continue;
		}
		loc = (pre->nodeloc - 10278) / 2;
		Dir[loc] = *pre;
		disk.write(0, Dir + loc, pre->nodeloc);
		pre = pre->predir;
	}
	///*******  删除目录下文件 
	MyDir* d = p->dirPTR;
	MyFile* f = p->filePTR;
	
	if (f != 0)
	{
		while (p->filePTR->nextFile != nullptr)
		{
			f = p->filePTR;
			while (f->nextFile->nextFile != NULL)
				f = f->nextFile;
			del_file(f->nextFile);
			f->nextFile = nullptr;
		}
		if (p->filePTR->nextFile == NULL)
		{
			del_file(p->filePTR);
			p->filePTR = NULL;
		}
	}
	if (d != 0)
	{
		while (p->dirPTR->nextdir != NULL)
		{
			d = p->nextdir;
			while (d->nextdir->nextdir != NULL)
				d = d->nextdir;
			del_dir(d->nextdir);
			d->nextdir = NULL;
		}
		if (p->dirPTR->nextdir == NULL)
		{
			del_dir(p->dirPTR);
			p->dirPTR = NULL;
		}
	}
	del_dir(p);
	cout << "DELETE			-OK" << endl;
	return 1;
}

int FileSystem::write_file(MyFile* file, int nodeloc)
{
	int bloc = group_desc.freeBlock;
	int flen = file->size / 32;
	int tsize = file->size;
	if (tsize == 0)return 1;
	if (file->size % 32 != 0)
		flen++;

	if (flen > (1023 - group_desc.freeBlock) * 32) ///***  文件大于内存空间
		return -1;
	Inode *lastnode, *tnode;
	lastnode = NULL;
	tnode = NULL;
	int loc = nodeloc;
	while (Node[loc].i_ptr!=-1)
	{
		loc = Node[loc].i_ptr;
		lastnode = Node + loc;
	}
	char* tmpc;
	tmpc = new char[strlen(file->content)];
	strcpy(tmpc, file->content);
	int flag = 0;

	for (int i = nodeloc % 32; i < 32; ++i)
	{
		if (bitmap[bloc][i] == 0)
		{
			flag = 1;
			tnode = Node + bloc * 32 + i;
			if (tsize <= 32)
				tnode->i_size = tsize;
			else if (tsize > 32)
				tnode->i_size = 32;
			tnode->i_ptr = -1;
			lastnode->i_ptr = bloc * 32 + i;

			disk.write(sizeof(Inode) * (loc), Node + (loc), 2083, sizeof(Inode));
			disk.write(sizeof(Inode) * (bloc * 32 + i), Node + (bloc * 32 + i), 2083, sizeof(Inode));

			node_in_use(bloc * 32 + i);
			update_writein(bloc * 32 + i);
			disk.write(0, tmpc, bloc * 32 + i, 32); ////****写入文件的content
			lastnode = tnode;
			loc = bloc * 32 + i;
			flen--;
			tsize -= 32;
			if (flen <= 0)break;
			tmpc += 32;
		}
	}
	file->size = tsize;
	if (flag)
	{
		update_bitmap();
	}
	if (flen > 0)return -flen;
	else
		return 1;
}
int FileSystem::Write_File(MyFile* file, char* ch)
{
	file->content = new char[strlen(ch) + 1];
	strcpy(file->content, ch);
	file->size = strlen(ch);
	int size = file->size;
	int t = file->nodeloc;

	while (write_file(file,t) < 0) {
		t = (t / 32 + 1) * 32;
	}
	file->size = size;

	MyDir* pre;  ///*** 上级各目录大小
	pre = currentDir;
	int loc = 0;
	while (pre != nullptr)
	{
		pre->size += file->size;
		if (pre->nodeloc == root.nodeloc)
		{
			disk.write(0, &root, 10276);
			pre = pre->predir;
			continue;
		}
		loc = (pre->nodeloc - 10278) / 2;
		Dir[loc] = *pre;
		disk.write(0, Dir + loc, pre->nodeloc);
		pre = pre->predir;
	}
	return 0;
}

//int FileSystem::renameDir(char* dir, char* ch)///*** 重命名 子目录 
//{
//	string tempname = ch;
//	for (int i = 0; i < 8; ++i)
//	{
//		if (tempname.find(error[i], 0) != string::npos)
//		{
//			cout << "RENAME			-FALSE" << endl;
//			return 0;
//		}
//	}
//
//	MyDir* h = currentDir->dirPTR;
//	MyDir* d = h;
//	while (h != NULL)
//	{
//		if (strcmp(h->name, ch) == 0)
//		{
//			cout << "DIR EXIST			-FALSE" << endl;
//			return 0;
//		}h = h->nextdir;
//	}
//	while (d != NULL)
//	{
//		if (strcmp(d->name, dir) == 0)
//		{
//			break;
//		}d = d->nextdir;
//	}
//	memset(d->name, 0, 11);
//	strcpy(d->name, ch);
//	d->name[strlen(ch)] = '\0';
//	
//	int loc = (d->nodeloc - 10278) / 2;
//
//	disk.write(0, Dir + loc, 10278 + loc * 2);
//	cout << "RENAME			-OK" << endl;
//	return 1;
//}
int FileSystem::renameFile(char* file, char* ch)///*** 重命名 子文件
{
	string tempname = ch;
	for (int i = 0; i < 8; ++i)
	{
		if (tempname.find(error[i], 0) != string::npos)
		{
			cout << "RENAME			-FALSE" << endl;
			return 0;
		}
	}

	MyFile* h = currentDir->filePTR;
	MyFile* f = h;
	while (h != NULL)
	{
		if (strcmp(h->name, ch) == 0)
		{
			cout << "NAME EXIST			-FALSE" << endl;
			return 0;
		}h = h->nextFile;
	}
	while (f != NULL)
	{
		if (strcmp(f->name, file) == 0)
		{
			break;
		}f = f->nextFile;
	}
	if (!f)
	{
		cout << "FILE NOT EXIST			-FALSE" << endl;
		return 0;
	}

	memset(f->name, 0, 11);
	strcpy(f->name, ch);
	f->name[strlen(ch)] = '\0';

	int loc = (f->nodeloc - 16384) / 2;

	disk.write(0, File + loc, f->nodeloc);
	cout << "RENAME			-OK" << endl;
	return 1;
}

MyFile* FileSystem::copy_file(MyFile* file)
{
	file = readfile(file);
	MyFile* f = new MyFile;
	f->size = file->size;
	strcpy(f->name, file->name);
	if (file->content)
	{
		f->content = new char[strlen(file->content) + 1];
		strcpy(f->content, file->content);
	}
	else
		f->content = NULL;
	f->blockloc = -1;
	f->nodeloc = -1;
	f->nextFile = NULL;
	return f;
}
MyFile* FileSystem::copyFile(char* file)
{
	MyFile* h = currentDir->filePTR;
	while (h != NULL)
	{
		if (strcmp(h->name, file) == 0)
			break;
		h = h->nextFile;
	}
	if (!h)
	{
		cout << "FILE NOT EXIST			-FALSE" << endl;
		return NULL;
	}
	copytempfile = copy_file(h);
	cout << "COPY			-OK" << endl;
	return copytempfile;
}

int FileSystem::pasteFile(MyDir* path)///************* more echo,copy的content需要读出来! export import 
{
	MyFile* h = path->filePTR;
	if (copytempfile == NULL)
	{
		return 0;
	}
	if (group_desc.freeNode * 32 < copytempfile->size)
	{
		cout << "NO ENOUGH SPACE		-FALSE" << endl;
		return 0;
	}

	while (h != NULL)
	{
		if (!strcmp(h->name, copytempfile->name))
		{
			cout << "FILE EXISTS			-FALSE" << endl;
			return 0;
		}h = h->nextFile;
	}
	int loc = 0;
	MyFile* p = nullptr;
	p = find_fileloc(File, 4095, loc);
	if (p == NULL)
	{
		cout << "CREATE			-FALSE" << endl;
		return 0;
	}
	path->size = copytempfile->size;
	file_count++;
	path->filecount++;
	p->nextFile = path->filePTR;
	path->filePTR = p;
	memset(p->name, 0, strlen(p->name));
	strcpy(p->name, copytempfile->name);

	if (p->content)
		delete[]p->content;
	if (copytempfile->content) {
		p->content = new char[strlen(copytempfile->content)];
		strcpy(p->content, copytempfile->content);
	}
	p->nodeloc = 16384 + loc * 2;
	p->blockloc = p->nodeloc / 32;
	if (p->nodeloc % 32 != 0)
		p->blockloc++;
	Node[p->nodeloc].i_ptr = p->nodeloc + 1;
	Node[p->nodeloc].i_size = 32;
	Node[p->nodeloc+1].i_ptr = -1;
	Node[p->nodeloc+1].i_size = 32 - sizeof(MyFile);    
	///*** 结构体写进空间
	disk.write(sizeof(Inode) * (p->nodeloc), Node + p->nodeloc, 2083, sizeof(Inode));
	disk.write(sizeof(Inode) * (p->nodeloc + 1), Node + p->nodeloc + 1, 2083, sizeof(Inode));
	disk.write(0, (char*)&file_count, 10275, 10);
	///*** 内容写进空间
	int size = p->size;
	int t = p->nodeloc;
	while (write_file(p, t) < 0) {
		t = (t / 32 + 1) * 32;
	}
	p->size = size;
	
	MyDir* pre;
	pre = currentDir;
	currentDir->filecount++;
	while (pre != nullptr)    ///***  上级个目录大小
	{
		pre->size += p->size;
		if (pre->nodeloc == root.nodeloc)
		{
			disk.write(0, &root, 10276);
			pre = pre->predir;
			continue;
		}
		loc = (pre->nodeloc - 10278) / 2;
		Dir[loc] = *pre;
		disk.write(0, Dir + loc, pre->nodeloc);
		pre = pre->predir;
	}
	cout << "PASTE			-OK" << endl;
	return 1;
}

string FileSystem::showCurrentDir()
{
	MyDir* tmp = currentDir;
	string s = "";
	while (tmp)
	{
		s = '/' +( tmp->name + s);
		tmp = tmp->predir;
	}
	return s;
}

void FileSystem::ls()
{
	MyDir* dtmp = currentDir->dirPTR;
	MyFile* ftmp = currentDir->filePTR;
	while (dtmp)
	{
		cout << "/" << dtmp->name << endl;
		dtmp = dtmp->nextdir;
	}
	while (ftmp)
	{
		cout << ftmp->name << endl;
		ftmp = ftmp->nextFile;
	}
	return;
}

bool FileSystem::cd(char* path)
{
	int len = strlen(path);
	if (len == 1)
	{
		switch (*path)
		{
		case '/':case'~':case NULL:
			currentDir = &root;
			break;
		default:
			return false;             ///*** 命令错误 
		}
		return true;;
	}
	if (len == 2 && path[0] == '.' && path[1] == '.')
	{
		currentDir = currentDir->predir;
		return true;;
	}
	MyDir* r1 = NULL, * r2 = NULL;
	r1 = Find_path(path);
	r2 = Find_subpath(path);
	if (!r1 && !r2)
	{
		cout << "DIR NOT EXIST			-FALSE" << endl;
		return true;
	}
	if (r1)
		currentDir = r1;
	if (r2)
		currentDir = r2;
	return true;
}

void FileSystem::more(char* file)
{
	MyFile* f;
	f = currentDir->filePTR;
	if (file == "")return;
	while (f->nextFile)
	{
		if (strcmp(f->name, file) == 0)
			break;
		f = f->nextFile;
	}
	if (!f)
	{
		cout << "FILE NOT EXIST			-FALSE" << endl;
		return;
	}
	if(f->content)
		delete []f->content;
	
	f->content = NULL;
	f = readfile(f);
	cout << "#####################  START  ######################" << endl;
	if (f->content != NULL)
		cout << f->content << endl;
	cout << "#####################   END   ######################" << endl;
}

void FileSystem::echo(char* s, char* file)  
{
	MyFile* f = currentDir->filePTR;
	while (f)
	{
		if (strcmp(file, f->name) == 0)
			break;
		f = f->nextFile;
	}
	if (!f) {
		cout << "FILE NOT EXIST			-FALSE" << endl;
		return;
	}
	int loc = f->nodeloc;
	int cnt = 0;
	while (Node[loc].i_ptr != -1)
	{
		cnt++;
		loc = Node[loc].i_ptr;
		if (cnt >= 2)
		{
			Node[loc].i_ptr = -1;
		}
	}
	if (f->content)
		delete[]f->content;
	f->content = NULL;
	Write_File(f, s);
	return;
}

int FileSystem::import_file(char* file, char* path)///*** -->其他磁盘文件复制到当前目录
{
	disk.disk_int->open(path, ios::in);
	char* ch = new char[8192 * 32]{ 0 }; ////文件最大内容
	int cnt = 0;
	while (!disk.disk_int->eof())
	{
		disk.disk_int->read(ch + cnt, 1);
		cnt++;
	}
	disk.disk_int->close();
	newFile(file);
	MyFile* f = currentDir->filePTR;
	while (f)
	{
		if (strcmp(file, f->name) == 0)
		{
			break;
		}
		f = f->nextFile;
	}
	MyDir* pre;
	pre = currentDir;
	currentDir->filecount++;
	file_count++;
	disk.write(0, (char*)&file_count, 10275, 10);
	f->size = strlen(ch);
	int loc = 0;
	while (pre != nullptr)    ///***  上级个目录大小
	{
		pre->size += f->size;
		if (pre->nodeloc == root.nodeloc)
		{
			disk.write(0, &root, 10276);
			pre = pre->predir;
			continue;
		}
		loc = (pre->nodeloc - 10278) / 2;
		Dir[loc] = *pre;
		disk.write(0, Dir + loc, pre->nodeloc);
		pre = pre->predir;
	}
	Write_File(f, ch);	
	return 1;
}

int FileSystem::export_file(char* file, char* path)///*** -->当前目录文件复制到其他磁盘路径
{
	MyFile* f = currentDir->filePTR;
	while (f)
	{
		if (strcmp(file, f->name) == 0)
			break;
		f = f->nextFile;
	}
	if (!f) {
		cout << "FILE NOT EXIST			-FALSE" << endl;
		return 0;
	}
	char* ch;
	ch = new char[strlen(path) + strlen(file) + 2];
	strcpy(ch, path);
	ch[strlen(path)] = '\\';
	ch[strlen(path) + 1] = '\0';
	strcat(ch, file);
	disk.disk_int->open(ch, ios::out);
	if (!disk.disk_int->is_open())
	{
		cout << "未能打开文件." << endl;
		return 0;
	}
	if (f->content)
		disk.disk_int->write(f->content, sizeof(char) * strlen(f->content) + 1);
	disk.disk_int->close();
	return 1;
}

void FileSystem::remove(char* file, char* path1, char* path2)
{
	MyDir* d1, * d2;
	char* tp1 = new char[strlen(path1)];
	char* tp2 = new char[strlen(path2)];
	strcpy(tp1, path1);
	strcpy(tp2, path2);
	d1 = Find_path(tp1); ///*** 绝对路径 和 子目录 查找
	d2 = Find_path(tp2);

	if (!d1) {
		d1 = Find_subpath(path1);
		cout << "<path>1 NO DIR			-FALSE" << endl;
	}
	if (!d2) {
		d2 = Find_subpath(path1);
		cout << "<path>2 NO DIR			-FALSE" << endl;
	}
	if (!d1 || !d2)
	{
		return;
	}
	MyFile* f1, * f2, * pref;
	f1 = d1->filePTR;
	f2 = d2->filePTR;
	pref = NULL;
	while (f1 != NULL)
	{
		if (strcmp(file, f1->name) == 0)
		{
			break;
		}
		pref = f1;
		f1 = f1->nextFile;
	}
	if (!f1)
	{
		cout << "NO FILE			-FALSE" << endl;
		return;
	}
	while (f2 != NULL)
	{
		if (strcmp(f1->name, f2->name) == 0)
		{
			cout << "FILE EXIST			-FALSE" << endl;
			return;
		}
		f2 = f2->nextFile;
	}
	if (!pref)
		d1->filePTR = f1->nextFile;
	else
		pref->nextFile = f1->nextFile;
	d1->filecount--;
	f1->nextFile = d2->filePTR;
	d2->filePTR = f1;
	d2->filecount++;
	cout << "MOVE			-OK" << endl;

	MyDir* pre;
	pre = d2;
	int loc = 0;
	while (pre != nullptr)    ///***  上级个目录大小
	{
		pre->size += f1->size;
		if (pre->nodeloc == root.nodeloc)
		{
			disk.write(0, &root, 10276);
			pre = pre->predir;
			continue;
		}
		loc = (pre->nodeloc - 10278) / 2;
		Dir[loc] = *pre;
		disk.write(0, Dir + loc, pre->nodeloc);
		pre = pre->predir;
	}
	pre = d1;
	while (pre != nullptr)    ///***  上级个目录大小
	{
		pre->size -= f1->size;
		if (pre->nodeloc == root.nodeloc)
		{
			disk.write(0, &root, 10276);
			pre = pre->predir;
			continue;
		}
		loc = (pre->nodeloc - 10278) / 2;
		Dir[loc] = *pre;
		disk.write(0, Dir + loc, pre->nodeloc);
		pre = pre->predir;
	}
	return;
}

void FileSystem::remove_disk(bool isremove)
{
	disk.stop(isremove);
	return;
}







