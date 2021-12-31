#ifndef INIT_H
#define INIT_H
#include"DataStruct.h"
#include <stdio.h>

/**����ʵ������һ���ļ���ģ���ļ�ϵͳ�������ļ�ϵͳ���������ݴ洢��һ���ļ��ж�д
  *Ϊ�˼�ʵ������ݣ�����ֻģ��һ�����飬����������������ֻ��һ�����Ļ���������Ҳֻ��Ҫ1���ͺ�
  * �ļ�ϵͳ�Ĺ�����������+1�����飬���п����а��������顢gdt��λͼ�����ݿ��
  */
static unsigned int num_last_alloc_inode; // �������Ľڵ��
static unsigned int num_last_alloc_block; // �����������ݿ�� 
static unsigned int num_current_dir;		// ��ǰĿ¼�Ľڵ�� 
static unsigned int len_current_dir;		//��ǰ·���ĳ���
static unsigned int file_opened[10];		//�򿪵��ļ��б�

static struct SuperBlock super_block[1];		//������
static struct GroupDesc gdt[1];					//��������
static struct Inode inode[1];						//inode
static unsigned char blockbitmap_buffer[BLOCK_SIZE] = { 0 };//��λͼ
static unsigned char inodebitmap_buffer[BLOCK_SIZE] = { 0 };	//inodeλͼ
static struct File dir[32];   // �ļ�/Ŀ¼������	
static char buffer[BLOCK_SIZE];					//���ݿ黺����
static FILE* fp;								// ģ�����ָ��
static char current_dir_name[64];

static void write_SuperBlock();	//����������Ϣд��ģ�����
static void read_SuperBlock();	//��ģ������ж��볬����
static void write_gdt(void);    //��gdtд��ģ�����
static void read_gdt(void);    //����gdt������
static void write_inode(unsigned short i); //д��inode��ģ�����
static void read_inode(unsigned short i); //��ģ����̼���inode��
static void write_blockbitmap(void);  //д��inode��ģ�����
static void read_blockbitmap(void);  //��ģ����̼��ؿ�λͼ
static void write_inodebitmap(void);  //д��inodebitmap��ģ�����
static void read_inodebitmap(void);  //��ģ����̼���inodebitmap
static void write_dir(unsigned short i);//д��dir��ģ�����
static void read_dir(unsigned short i);//��ģ����̼���Ŀ¼
static void write_block(unsigned short i);//д�����ݿ鵽ģ�����
static void read_block(unsigned short i);//��ģ����̼������ݿ�

static unsigned int get_free_block();			//����õ�һ�����е����ݿ�
static unsigned int get_free_inode();			//����õ�һ�����е�inode
static unsigned int find_FileOrDirent(char filename[23],int filetype,unsigned int *num_block,unsigned int *num_dirent);//�ڵ�ǰĿ¼�²����ļ��Ƿ���ڣ������ڷ���i�ڵ�ţ��������򷵻�0
static void alloc_file_data(unsigned int i,unsigned short length,char type);			//����һ���ļ�����Ҫ�����ݺͿռ�
static void delete_block(unsigned int i);		//ɾ����������ݿ�
static void delete_inode(unsigned int i);		//ɾ��inode�ڵ�
void initExt2();		//��ʼ��ģ���ext2�ļ�ϵͳ
void init();
void ShowSuperBlock();
void makefile(char fileName[23], int type);
#endif
