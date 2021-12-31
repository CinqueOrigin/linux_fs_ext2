#ifndef INIT_H
#define INIT_H
#include"DataStruct.h"
#include <stdio.h>

/**本次实验是在一个文件中模拟文件系统，即把文件系统的所有数据存储在一个文件中读写
  *为了简化实验的内容，本次只模拟一个块组，所以组描述符表中只有一项，下面的缓存区数组也只需要1个就好
  * 文件系统的构成是引导区+1个块组，其中块组中包含超级块、gdt、位图、数据块等
  */
static unsigned int num_last_alloc_inode; // 最近分配的节点号
static unsigned int num_last_alloc_block; // 最近分配的数据块号 
static unsigned int num_current_dir;		// 当前目录的节点号 
static unsigned int len_current_dir;		//当前路径的长度
static unsigned int file_opened[10];		//打开的文件列表

static struct SuperBlock super_block[1];		//超级块
static struct GroupDesc gdt[1];					//组描述符
static struct Inode inode[1];						//inode
static unsigned char blockbitmap_buffer[BLOCK_SIZE] = { 0 };//块位图
static unsigned char inodebitmap_buffer[BLOCK_SIZE] = { 0 };	//inode位图
static struct File dir[32];   // 文件/目录缓冲区	
static char buffer[BLOCK_SIZE];					//数据块缓冲区
static FILE* fp;								// 模拟磁盘指针
static char current_dir_name[64];

static void write_SuperBlock();	//将超级块信息写回模拟磁盘
static void read_SuperBlock();	//从模拟磁盘中读入超级块
static void write_gdt(void);    //将gdt写回模拟磁盘
static void read_gdt(void);    //加载gdt到缓存
static void write_inode(unsigned short i); //写回inode表到模拟磁盘
static void read_inode(unsigned short i); //从模拟磁盘加载inode表
static void write_blockbitmap(void);  //写回inode表到模拟磁盘
static void read_blockbitmap(void);  //从模拟磁盘加载块位图
static void write_inodebitmap(void);  //写回inodebitmap表到模拟磁盘
static void read_inodebitmap(void);  //从模拟磁盘加载inodebitmap
static void write_dir(unsigned short i);//写回dir到模拟磁盘
static void read_dir(unsigned short i);//从模拟磁盘加载目录
static void write_block(unsigned short i);//写回数据块到模拟磁盘
static void read_block(unsigned short i);//从模拟磁盘加载数据块

static unsigned int get_free_block();			//分配得到一个空闲的数据块
static unsigned int get_free_inode();			//分配得到一个空闲的inode
static unsigned int find_FileOrDirent(char filename[23],int filetype,unsigned int *num_block,unsigned int *num_dirent);//在当前目录下查找文件是否存在，若存在返回i节点号，不存在则返回0
static void alloc_file_data(unsigned int i,unsigned short length,char type);			//分配一个文件所需要的数据和空间
static void delete_block(unsigned int i);		//删除分配的数据块
static void delete_inode(unsigned int i);		//删除inode节点
void initExt2();		//初始化模拟的ext2文件系统
void init();
void ShowSuperBlock();
void makefile(char fileName[23], int type);
#endif
