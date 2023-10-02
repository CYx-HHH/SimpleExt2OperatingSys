实现简单的ext2操作系统

用一个文件虚拟1M磁盘空间模拟Linux单用户文件系统,只有一个超级用户root,用户名为root,密码是123456.
一共分为Disk,Node,filesystem,Interface头文件,分别完成磁盘(二进制文件)读写操作,文件系统各节点定义,文件系统的操作和对输入字符判断处理的接口.
文件系统各节点定义如下:
Inode节点,8字节,占8字节.
Group描述块,16字节,占2node.
超级块,16字节,占2node.
MyFile文件结构体,占2node.名称限长11位.
MyDir文件夹结构体,占2node.名称限长11位.

内存共1M,确定32字节为一个node,1024字节为一个block.一共有1024个块,即32768个node.内存的分配情况为:
超级块							------ 1 node
组描述块						------ 1 node
Block位图						------ 32 node
inode位图						------ 1024 node
每个Block的inode位图			------ 1024 node
用户名和密码   				------ 1 node
Node表							------ 8*1024 node     
文件计数和文件夹计数			------ 1 node          
root根目录						------ 2 node          
Dir路径表						------ 6106 node       
File文件表						------ 8192 node
File内容						------ 8192 node

起始只有root一个文件夹.

![image](https://github.com/CYx-HHH/SimpleExt2OperatingSys/assets/53809467/92f01ca4-30c7-4b6b-8591-1d7ae5b55cf1)

