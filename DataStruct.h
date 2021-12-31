#ifndef DATASTRUCT_H
#define DATASTRUCT_H
#define BLOCK_SIZE 1024		//块大小设置为1024KB
#define BLOCK_COUNT 8192	//设置至多可以使用8192个块，即8M
#define INODE_COUNT 8192	//INODE个数设置为与BLOCK相同

//超级块的数据结构
struct SuperBlock {
	char fs_name[10];				//文件系统的名称	10B
	unsigned short fs_id;			//文件系统标识号	2B
	unsigned short first_block;		//第一个数据块的位置为0/1，取决于数据块的大小是不是1K	2B

	unsigned int datablock_size;	//一个数据块的大小，为2的幂次方		4B
	unsigned int inode_size;		//inode节点的大小					4B
	unsigned int groupblock_size;	//块组大小，即每个块组有多少个块	4B
	unsigned int block_count;		//文件系统总块的数量				4B

	unsigned int inode_count;		//文件系统inode的数量				4B

	unsigned int free_block_count;	//文件系统空闲块的数量				4B
	unsigned int free_inode_count;	//文件系统中空闲inode的数量			4B
	char padding[982];					//填充项，将超级块填满为1个块的大小	982B
};

//组描述符的数据结构，32B
struct GroupDesc
{
	unsigned int position_block_bitmap;		//每个组中块位图所在的块号			4B
	unsigned int position_inode_bitmap;		//每个组中索引节点位图所在块号		4B
	unsigned int first_inode_table;			//每个组中索引节点表的第一个块号	4B
	unsigned short free_block_count;		//块组中空闲块个数					2B
	unsigned short free_inode_count;		//块组中空闲inode的个数				2B
	unsigned int contents_count;			//目录的个数						4B
	char padding[12];						//填充到32B							12B

};
//inode节点的数据结构	128B
struct Inode{
	unsigned int i_number;					//索引节点号				4B
	unsigned int i_block_count;				//文件占用的数据块的个数	4B
	unsigned short i_mode;					//文件的访问模式			
	unsigned short i_uid;					//文件拥有者的id
	unsigned short i_group_id;				//文件所属组的id
	unsigned long i_size;					//文件的大小
	unsigned short i_link_count;			//文件的连接数，这里指硬链接
	unsigned int i_access_time;				//文件的访问时间
	unsigned int i_create_time;				//文件的创建时间
	unsigned int i_modify_time;				//修改时间
	unsigned int i_delete_time;				//删除时间
	unsigned int i_blocks[15];				//15个数据项中，前12个指向文件块位置，后3个指向索引表位置
	char padding[28];

};
//文件的一些必要的数据结构,32B
struct File {
	unsigned int i_number;				//索引节点号			4B 
	unsigned short content_len;				//目录项长度			2B
	unsigned short name_len;				//文件名长度			2B
	char file_type;							//文件类型(0 普通文件 1 目录.. ),为方便只模拟两种文件	1B
	char name[23];							//文件名				23B
};

#endif // 