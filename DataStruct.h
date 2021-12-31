#ifndef DATASTRUCT_H
#define DATASTRUCT_H
#define BLOCK_SIZE 1024		//���С����Ϊ1024KB
#define BLOCK_COUNT 8192	//�����������ʹ��8192���飬��8M
#define INODE_COUNT 8192	//INODE��������Ϊ��BLOCK��ͬ

//����������ݽṹ
struct SuperBlock {
	char fs_name[10];				//�ļ�ϵͳ������	10B
	unsigned short fs_id;			//�ļ�ϵͳ��ʶ��	2B
	unsigned short first_block;		//��һ�����ݿ��λ��Ϊ0/1��ȡ�������ݿ�Ĵ�С�ǲ���1K	2B

	unsigned int datablock_size;	//һ�����ݿ�Ĵ�С��Ϊ2���ݴη�		4B
	unsigned int inode_size;		//inode�ڵ�Ĵ�С					4B
	unsigned int groupblock_size;	//�����С����ÿ�������ж��ٸ���	4B
	unsigned int block_count;		//�ļ�ϵͳ�ܿ������				4B

	unsigned int inode_count;		//�ļ�ϵͳinode������				4B

	unsigned int free_block_count;	//�ļ�ϵͳ���п������				4B
	unsigned int free_inode_count;	//�ļ�ϵͳ�п���inode������			4B
	char padding[982];					//����������������Ϊ1����Ĵ�С	982B
};

//�������������ݽṹ��32B
struct GroupDesc
{
	unsigned int position_block_bitmap;		//ÿ�����п�λͼ���ڵĿ��			4B
	unsigned int position_inode_bitmap;		//ÿ�����������ڵ�λͼ���ڿ��		4B
	unsigned int first_inode_table;			//ÿ�����������ڵ��ĵ�һ�����	4B
	unsigned short free_block_count;		//�����п��п����					2B
	unsigned short free_inode_count;		//�����п���inode�ĸ���				2B
	unsigned int contents_count;			//Ŀ¼�ĸ���						4B
	char padding[12];						//��䵽32B							12B

};
//inode�ڵ�����ݽṹ	128B
struct Inode{
	unsigned int i_number;					//�����ڵ��				4B
	unsigned int i_block_count;				//�ļ�ռ�õ����ݿ�ĸ���	4B
	unsigned short i_mode;					//�ļ��ķ���ģʽ			
	unsigned short i_uid;					//�ļ�ӵ���ߵ�id
	unsigned short i_group_id;				//�ļ��������id
	unsigned long i_size;					//�ļ��Ĵ�С
	unsigned short i_link_count;			//�ļ���������������ָӲ����
	unsigned int i_access_time;				//�ļ��ķ���ʱ��
	unsigned int i_create_time;				//�ļ��Ĵ���ʱ��
	unsigned int i_modify_time;				//�޸�ʱ��
	unsigned int i_delete_time;				//ɾ��ʱ��
	unsigned int i_blocks[15];				//15���������У�ǰ12��ָ���ļ���λ�ã���3��ָ��������λ��
	char padding[28];

};
//�ļ���һЩ��Ҫ�����ݽṹ,32B
struct File {
	unsigned int i_number;				//�����ڵ��			4B 
	unsigned short content_len;				//Ŀ¼���			2B
	unsigned short name_len;				//�ļ�������			2B
	char file_type;							//�ļ�����(0 ��ͨ�ļ� 1 Ŀ¼.. ),Ϊ����ֻģ�������ļ�	1B
	char name[23];							//�ļ���				23B
};

#endif // 