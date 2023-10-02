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
    cout << " * 1.mkdir/md + <string>                           �ڵ�ǰĿ¼���½��ļ���                                   *" << endl;
    cout << " * 2.rmdir/rd + <path>[����·��|��ǰĿ¼���ļ���]  ɾ����Ϊ<string>���ļ���                                 *" << endl;
    cout << " * 3.rename + <string>1 + <string>2                �������ļ�,<string2>��������                             *" << endl;
    cout << " * 4.cp/copy + <string> + <path>                   ����(��ǰĿ¼��)�ļ���<path>                             *" << endl;
    cout << " *     ��:cp a /root/aaa                                                                                    *" << endl;
    cout << " * 5.move + <string>1 + <path>1 + <path>2          �ƶ��ļ�<string>1,������Ŀ¼<path>1�ƶ���Ŀ¼<path>2     *" << endl;
    cout << " *     ��:move a /root /root/usera/aaa                                                                      *" << endl;
    cout << " * 6.cd + <path>                                   �����ļ���,��Ϊ<path>                                    *" << endl;
    cout << " * 7.edit + <string>                               ��ǰ�ļ����´����ļ�                                     *" << endl;
    cout << " * 8.echo + <string>1  >  <string>2                ��<string>1���뵽��ǰ�ļ����µ��ļ�<string>2             *" << endl;
    cout << " * 9.del + <string>                                ��ǰ�ļ�����ɾ���ļ�                                     *" << endl;
    cout << " * 10.ls                                           ��ʾ��Ŀ¼                                               *" << endl;
    cout << " * 11.more + <string>[��ʾ�ļ�]                    �鿴��ǰĿ¼���ļ�                                       *" << endl;
    cout << " * 12.export + <string> + <path>                   ��������<path>���ļ����Ƶ���ǰĿ¼,<string>Ϊ�ļ���      *" << endl;
    cout << " *     ��:export a C:\\user\\destktop                                                                       *" << endl;
    cout << " * 13.import + <string> + <path>                   ��ǰĿ¼���ļ�<string>���Ƶ���������·��<path>           *" << endl;
    cout << " *     ��:import a C:\\user\\desktop\\a.txt                                                                 *" << endl;
    cout << " * 14.pwd                                          ��ʾ��ǰ����Ŀ¼                                         *" << endl;
    cout << " * 15.time                                         ��ʾϵͳ��ǰʱ��                                         *" << endl;
    cout << " * 16.ver                                          ��ʾ��ǰ�����ϵĲ���ϵͳ�汾��Ϣ                         *" << endl;
    cout << " * 17.exit                                         �˳�����                                                 *" << endl;
    cout << " * 18.help                                         �鿴����                                                 *" << endl;
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
    cout << "��ǰϵͳʱ��Ϊ:" << endl;
    cout << tmp.wYear << "/" << tmp.wMonth << "/" << tmp.wDay << "    " << tmp.wHour << ":" << tmp.wMinute << ":" << tmp.wSecond << endl;
}

void ver()
{
    OSVERSIONINFOEX osver;
    osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    //��ȡ�汾��Ϣ
    if (!GetVersionEx((LPOSVERSIONINFOW)&osver))
    {
        cout << "Error:" << GetLastError() << endl;
    }
    //��ӡ�汾��Ϣ
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
    if (loc != -1)                  ///*** ȥ������ǰ�Ŀո�.
    {
        for (int i = 0; i < s.length(); ++i)
            s[i] = s[i + loc + 1];
    }
    int t = s.find(ch);
    if (t != string::npos && t == 0) ///*** �Ƿ���0λ���ҵ�����.
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
        else if (pro_command(s, "import"))///*** -->���������ļ����Ƶ���ǰĿ¼
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
    cout << "����һ��ģ���ļ�ϵͳ.\n" << endl;
    cout << "���½.\n";
    fs.login();
    while (!isLogin)
    {
        cout << "���½:\n";
        fs.login();
    }
    getchar();
    char cmd[255];
    cout << "����help�鿴ָ��" << endl;
   
    while (flag)
    {
        print();
        cin.getline(cmd, 255);
        s = cmd;
        //// system("CLS");
        fs_command(s);

    }
}


