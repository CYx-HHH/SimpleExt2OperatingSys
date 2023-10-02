#pragma warning(disable:4996)
#include"Interface.h"
#include"filesystem.h"
#include<iostream>
//#include<Windows.h>
#include<windows.h>
#include<string>

using namespace std;
extern bool isLogin;
FileSystem fs;
void help()
{
    cout << "***************************************************************************************************************" << endl;
    cout << " * 1.mkdir/md + <string>                           在当前目录下新建文件夹                                   *" << endl;
    cout << " * 2.rmdir/rd + <path>[绝对路径|当前目录下文件夹]  删除名为<string>的文件夹                                 *" << endl;
    cout << " * 3.rename + <string>1 + <string>2                重命名文件,<string2>是新名字                             *" << endl;
    cout << " * 4.cp/copy + <string> + <path>                   复制(当前目录下)文件到<path>                             *" << endl;
    cout << " *     例:cp a /root/aaa                                                                                    *" << endl;
    cout << " * 5.move + <string>1 + <path>1 + <path>2          移动文件<string>1,从所在目录<path>1移动到目录<path>2     *" << endl;
    cout << " *     例:move a /root /root/usera/aaa                                                                      *" << endl;
    cout << " * 6.cd + <path>                                   进入文件夹,名为<path>                                    *" << endl;
    cout << " * 7.edit + <string>                               当前文件夹下创建文件                                     *" << endl;
    cout << " * 8.echo + <string>1  >  <string>2                将<string>1输入到当前文件夹下的文件<string>2             *" << endl;
    cout << " * 9.del + <string>                                当前文件夹下删除文件                                     *" << endl;
    cout << " * 10.ls                                           显示子目录                                               *" << endl;
    cout << " * 11.more + <string>[显示文件]                    查看当前目录下文件                                       *" << endl;
    cout << " * 12.export + <string> + <path>                   其他磁盘<path>下文件复制到当前目录,<string>为文件名      *" << endl;
    cout << " *     例:export a C:\\user\\destktop                                                                       *" << endl;
    cout << " * 13.import + <string> + <path>                   当前目录下文件<string>复制到其他磁盘路径<path>           *" << endl;
    cout << " *     例:import a C:\\user\\desktop\\a.txt                                                                 *" << endl;
    cout << " * 14.pwd                                          显示当前工作目录                                         *" << endl;
    cout << " * 15.time                                         显示系统当前时间                                         *" << endl;
    cout << " * 16.ver                                          显示当前机器上的操作系统版本信息                         *" << endl;
    cout << " * 17.exit                                         退出程序                                                 *" << endl;
    cout << " * 18.help                                         查看帮助                                                 *" << endl;
    cout << "***************************************************************************************************************" << endl
        << endl;
}

void print()
{
    string s = fs.showCurrentDir();
    if (s == "/root")
        cout << fs.username << "@" << fs.root.name << ":/# ";
    else
        cout << fs.username << "@" << fs.root.name << ":" << s << "# ";
}



void exitt()
{
    fs.remove_disk(false);
    exit(0);
}

void time()
{
    SYSTEMTIME tmp;
    GetSystemTime(&tmp);
    cout << "当前系统时间为:" << endl;
    cout << tmp.wYear << "/" << tmp.wMonth << "/" << tmp.wDay << "    " << tmp.wHour << ":" << tmp.wMinute << ":" << tmp.wSecond << endl;
}

void ver()
{
    OSVERSIONINFOEX osver;
    osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    //获取版本信息
    if (!GetVersionEx((LPOSVERSIONINFOW)&osver))
    {
        cout << "Error:" << GetLastError() << endl;
    }
    //打印版本信息
    cout << "System info:" << endl;
    cout << "Version:" << osver.dwMajorVersion << "." << osver.dwMinorVersion
        << " Build " << osver.dwBuildNumber
        << " Service Pack " << osver.wServicePackMajor << "."
        << osver.wServicePackMinor << endl;
    return;
}


bool pro_command(string s, const char* ch)
{
    int loc = -1;
    for (int i = 0; i < s.length(); ++i)
    {
        if (isspace(s[i]))
            loc = i;
        else
            break;
    }
    if (loc != -1)                  ///*** 去除命令前的空格.
    {
        for (int i = 0; i < s.length(); ++i)
            s[i] = s[i + loc + 1];
    }
    int t = s.find(ch);
    if (t != string::npos && t == 0) ///*** 是否在0位置找到命令.
        return true;
    else return false;
}

void fs_command(string s)
{
    if (s == "help")
        help();
    else if (s == "ls")
        fs.ls();
    else if (s == "pwd")
        cout << fs.showCurrentDir() << endl;
    else if (s == "time")
        time();
    else if (s == "ver")
        ver();
    else if (s == "exit")
        exitt();
    else
    {
        if (pro_command(s,"mkdir"))
        {
            char* ss = new char[s.length()];
            strcpy(ss, s.c_str());
            char* d = strtok(ss, " ");
            d = strtok(NULL, " ");
            if (d)
                fs.newDir(d);
        }
        else if (pro_command(s, "md"))
        {
            char* ss = new char[s.length()];
            strcpy(ss, s.c_str());
            char* d = strtok(ss, " ");
            d = strtok(NULL, " ");
            if (d)
                fs.newDir(d);
        }
        else if (pro_command(s, "rmdir"))
        {
            char* ss = new char[s.length()];
            strcpy(ss, s.c_str());
            char* d = strtok(ss, " ");
            d = strtok(NULL, " ");
            if (d)
                fs.delDir(d);
        }
        else if (pro_command(s, "rd"))
        {
            char* ss = new char[s.length()];
            strcpy(ss, s.c_str());
            char* d = strtok(ss, " ");
            d = strtok(NULL, " ");
            if (d)
                fs.delDir(d);
        }
        else if (pro_command(s, "rename"))
        {
            char* ss = new char[s.length()];
            strcpy(ss, s.c_str());
            char* file = strtok(ss, " ");
            file = strtok(NULL, " ");
            char* name = strtok(NULL, " ");
            if (name)
                fs.renameFile(file, name);
        }
        else if (pro_command(s, "copy"))
        {
            char* ss = new char[s.length()];
            strcpy(ss, s.c_str());
            char* file = strtok(ss, " ");
            file = strtok(NULL, " ");
            char* path = strtok(NULL, " ");

            if (file)
                fs.copyFile(file);
            else return;

            MyDir* tmp = fs.Find_path(path);
            if (tmp == NULL)
            {
                tmp = fs.Find_subpath(path);
            }
            if (tmp == NULL)
            {
                cout << "DIR NOT EXIST 			-FALSE" << endl;
                return;
            }
            if (tmp)
                fs.pasteFile(tmp);
        }
        else if (pro_command(s, "cp"))
        {
            char* ss = new char[s.length()];
            strcpy(ss, s.c_str());
            char* file = strtok(ss, " ");
            file = strtok(NULL, " ");
            char* path = strtok(NULL, " ");

            if (file)
                fs.copyFile(file);

            MyDir* tmp = fs.Find_path(path);
            if (tmp == NULL)
            {
                tmp = fs.Find_subpath(path);
            }
            if (tmp == NULL)
            {
                cout << "DIR NOT EXIST 			-FALSE" << endl;
                return;
            }
            if (tmp)
                fs.pasteFile(tmp);
        }
        else if (pro_command(s, "move"))
        {
            char* ss = new char[s.length()];
            strcpy(ss, s.c_str());
            char* file = strtok(ss, " ");
            file = strtok(NULL, " ");
            char* path1 = strtok(NULL, " ");
            char* path2 = strtok(NULL, " ");
            if (file && path1 && path2)
                fs.remove(file, path1, path2);
        }
        else if (pro_command(s, "cd"))
        {
            char* ss = new char[s.length()];
            strcpy(ss, s.c_str());
            char* path = strtok(ss, " ");
            path = strtok(NULL, " ");
            if (path)
            {
                bool f = fs.cd(path);
                if (!f) cout << "COMMAND ERROR 			-FALSE" << endl;
            }
        }
        else if (pro_command(s, "edit"))
        {
            char* ss = new char[s.length()];
            strcpy(ss, s.c_str());
            char* file = strtok(ss, " ");
            file = strtok(NULL, " ");
            int res = 0;
            if (file)
                res = fs.newFile(file);
            if (res == 1)
                cout << "CREATE			-OK" << endl;
        }
        else if (pro_command(s, "del"))
        {
            char* ss = new char[s.length()];
            strcpy(ss, s.c_str());
            char* file = strtok(ss, " ");
            file = strtok(NULL, " ");
            if (file)
                fs.delFile(file);
        }
        else if (pro_command(s, "echo"))
        {
            char* ss = new char[s.length()];
            strcpy(ss, s.c_str());
            char* str = strtok(ss, " ");
            str = strtok(NULL, " ");
            char* file = strtok(NULL, " ");
            file = strtok(NULL, " ");
            if (file)
                fs.echo(str, file);
        }
        else if (pro_command(s, "more"))
        {
            char* ss = new char[s.length()];
            strcpy(ss, s.c_str());
            char* file = strtok(ss, " ");
            file = strtok(NULL, " ");
            fs.more(file);
        }
        else if (pro_command(s, "import"))///*** -->其他磁盘文件复制到当前目录
        {
        char* ss = new char[s.length()];
        strcpy(ss, s.c_str());
        char* file = strtok(ss, " ");
        file = strtok(NULL, " ");
        char* path = strtok(NULL, " ");
        fs.import_file(file, path);
        }
        else if (pro_command(s, "export"))
        {
        char* ss = new char[s.length()];
        strcpy(ss, s.c_str());
        char* file = strtok(ss, " ");
        file = strtok(NULL, " ");
        char* path = strtok(NULL, " ");
        fs.export_file(file, path);
        }
        else
        {
        return;
        }
    }


}

void run()
{
    isLogin = false;
    int flag = 1;
    string s;
    cout << "这是一个模拟文件系统.\n" << endl;
    cout << "请登陆.\n";
    fs.login();
    while (!isLogin)
    {
        cout << "请登陆:\n";
        fs.login();
    }
    getchar();
    char cmd[255];
    cout << "输入help查看指令" << endl;
   
    while (flag)
    {
        print();
        cin.getline(cmd, 255);
        s = cmd;
        //// system("CLS");
        fs_command(s);

    }
}


