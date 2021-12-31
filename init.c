#include "init.h"
#include<string.h>
#define BLOCK_START    1053696             //索引节点表起始位置+索引节点表大小，起始位置是5个块大小，索引节点表大小是128B*8192=1048576
//将超级块信息写回模拟磁盘
static void write_SuperBlock()
{
    fp = fopen("./Ext2Simulation", "r+");
    fseek(fp, BLOCK_SIZE, SEEK_SET);            //除去前1024K是引导区外，从1024KB位置开始写超级块
    fwrite(super_block, BLOCK_SIZE, 1, fp);  //缓冲区→文件流
    fflush(fp); //立刻将缓冲区的内容输出，保证磁盘内存数据的一致性
}
//从模拟磁盘中读入超级块
static void read_SuperBlock()
{
    fseek(fp, BLOCK_SIZE, SEEK_SET);
    fread(super_block, BLOCK_SIZE, 1, fp);//读取内容到超级块缓冲区中，文件流→缓冲区
}

static void write_gdt(void)
{
    fp = fopen("./Ext2Simulation", "r+");
    fseek(fp, BLOCK_SIZE+ BLOCK_SIZE, SEEK_SET);    //超级块和引导区合起来占用两个块
    fwrite(gdt, 32, 1, fp);                         //groupdesc大小是32B
    fflush(fp);
}

static void read_gdt(void)
{
    fseek(fp, BLOCK_SIZE + BLOCK_SIZE, SEEK_SET);
    fread(gdt, 32, 1, fp);
}

static void write_inode(unsigned short i)
{
    fp = fopen("./Ext2Simulation", "r+");
    fseek(fp, 5*BLOCK_SIZE + (i - 1) * 64, SEEK_SET);
    fwrite(inode, 128, 1, fp);
    fflush(fp);
}

static void read_inode(unsigned short i)
{
    fseek(fp, 5 * BLOCK_SIZE + (i - 1) * 64, SEEK_SET);
    fread(inode, 128, 1, fp);
}

static void write_blockbitmap(void)
{
    fp = fopen("./Ext2Simulation", "r+");
    fseek(fp, 3* BLOCK_SIZE, SEEK_SET);
    fwrite(blockbitmap_buffer, BLOCK_SIZE, 1, fp);
    fflush(fp);
}

static void read_blockbitmap(void)
{
    fseek(fp, 3 * BLOCK_SIZE, SEEK_SET);
    fread(blockbitmap_buffer, BLOCK_SIZE, 1, fp);
}

static void write_inodebitmap(void)
{
    fp = fopen("./Ext2Simulation", "r+");
    fseek(fp, 4* BLOCK_SIZE, SEEK_SET);
    fwrite(inodebitmap_buffer, BLOCK_SIZE, 1, fp);
    fflush(fp);
}

static void read_inodebitmap(void)
{
    fseek(fp, 4 * BLOCK_SIZE, SEEK_SET);
    fread(inodebitmap_buffer, BLOCK_SIZE, 1, fp);
}

static void write_dir(unsigned short i)
{
    fp = fopen("./Ext2Simulation", "r+");
    fseek(fp, BLOCK_START + i * BLOCK_SIZE, SEEK_SET);
    fwrite(dir, BLOCK_SIZE, 1, fp);
    fflush(fp);
}

static void read_dir(unsigned short i)
{
    fseek(fp, BLOCK_START + i * BLOCK_SIZE, SEEK_SET);
    fread(dir, BLOCK_SIZE, 1, fp);

}

static void write_block(unsigned short i)
{
    fp = fopen("./Ext2Simulation", "r+");
    fseek(fp, BLOCK_START + i * BLOCK_SIZE, SEEK_SET);
    fwrite(buffer, BLOCK_SIZE, 1, fp);
    fflush(fp);
}

static void read_block(unsigned short i)
{
    fseek(fp, BLOCK_START + i * BLOCK_SIZE, SEEK_SET);
    fread(buffer, BLOCK_SIZE, 1, fp);
}
//获取一个数据块，并返回获得的数据块的块号i
static unsigned int get_free_block()
{   //首先判断是否还有空闲块，如果有，就根据上次分配的块号算出它在块位图中的位置，除以8即可，这样便于连续分配，然后分配一个空闲块并改变位图即可。

    unsigned short cur = num_last_alloc_block;      //cur为上次分配的数据块的位置，单位是块位图字节号+块位图字节内位移，即块号
    unsigned char con = 128; // 1000 0000
    int flag = 0;           //记录字节内位移
    if (gdt[0].free_block_count == 0)
    {
        printf("all blocks are allocated!!\n");
        return 0;
    }
    read_blockbitmap();     //从模拟磁盘中读入块位图的信息
    cur /= 8;               //cur变为cur所在的字节
    while (blockbitmap_buffer[cur] == 255)//遍历，直到在块位图中找到空闲的块
    {
        if (cur == 1023)cur = 0; //最后一个字节也已经满，从头开始寻找
        else cur++;
    }
    while (blockbitmap_buffer[cur] & con) //在一个字节中找具体的某一个bit
    {
        con = con / 2;      //右移一位
        flag++;
    }
    blockbitmap_buffer[cur] = blockbitmap_buffer[cur] + con;    //空闲的0被1填上
    num_last_alloc_inode = cur * 8 + flag;                      //这次分配的位置写入变量中，便于连续分配块

    write_blockbitmap();
    gdt[0].free_block_count--;
    super_block[0].free_block_count--;
    write_gdt();
    write_SuperBlock();
    printf("we have alloc a new block!!!\n");
    return num_last_alloc_block;

}
//获取一个inode，并返回获得的数据块的块号i
static unsigned int get_free_inode()
{
    unsigned short cur = num_last_alloc_inode;
    unsigned char con = 128;
    int flag = 0;
    if (gdt[0].free_inode_count == 0)
    {
        printf("There is no Inode to be alloced!\n");
        return 0;
    }
    read_inodebitmap();

    cur = (cur - 1) / 8;   //第一个标号是1，但是存储是从0开始的,这里与分配块有细微差异，块从0开始分配
    //printf("%s",)
    while (inodebitmap_buffer[cur] == 255) //先看该字节的8个位是否已经填满
    {
        if (cur == 1023)cur = 0;
        else cur++;
    }
    while (inodebitmap_buffer[cur] & con)  //寻找0空闲位置
    {
        con = con / 2;
        flag++;
    }
    inodebitmap_buffer[cur] = inodebitmap_buffer[cur] + con;
    num_last_alloc_inode = cur * 8 + flag + 1;
    write_inodebitmap();
    gdt[0].free_inode_count--;
    super_block[0].free_inode_count--;
    write_gdt();
    write_SuperBlock();
    printf("we have alloc a new inode!!!\n");
    return num_last_alloc_inode;
}

static unsigned int find_FileOrDirent(char filename[23], int filetype, unsigned int* num_block, unsigned int* num_dirent)
{
    unsigned int j, k;
    read_inode(num_current_dir); //进入当前目录
    j = 0;
    while (j < inode[0].i_block_count&&j<12)        //为了简化不使用一二三级索引表
    {
        //读入inode的信息，用j循环扫描占用的数据块
        read_dir(inode[0].i_blocks[j]);
        k = 0;
        //循环扫描目录项下的项，知道找到类型和文件名都匹配且对应inode存在的
        while (k < 32)
        {
            if (!dir[k].i_number || dir[k].file_type != filetype || strcmp(dir[k].name, filename))
            {
                k++;
            }
            else
            {
                *num_block = j;
                *num_dirent = k;
                return dir[k].i_number;
            }
        }
        j++;
    }
    return 0;
	
}

static void alloc_file_data(unsigned int i, unsigned short length, char type)
{
    read_inode(i);      //读入i节点信息
    //如果是普通文件只需要初始化简单数据
    if (type == 0) {    
        inode[0].i_size = 0;
        inode[0].i_block_count = 0;
        inode[0].i_mode = 0755;
    }
    //目录文件则要修改目录项缓冲区,主要是添加两个目录项：.和..
    else if (type == 1) {
        //首先创建i节点信息
        inode[0].i_size = 32;
        inode[0].i_block_count = 1;
        inode[0].i_blocks[0] = get_free_block();
        //0是当前目录项.，1是父目录..
        dir[0].i_number = i;
        dir[1].i_number = num_current_dir;
        dir[0].name_len = length;
        dir[1].name_len = len_current_dir;
        //二者都是目录项
        dir[0].file_type = dir[1].file_type = 0;

        for (type = 2; type < 32; type++)
            dir[type].i_number = 0;
        strcpy(dir[0].name, ".");
        strcpy(dir[1].name, "..");
        //把目录初始化的内容写回模拟磁盘
        write_dir(inode[0].i_blocks[0]);
        //修改访问模式
        inode[0].i_mode = 01006;    
    }
    write_inode(i);
}

static void delete_block(unsigned int i)
{
    unsigned short num_block;
    num_block = i / 8;      //在块位图中的字节号
    read_blockbitmap();
    switch (i % 8) // 根据字节内位移更新block位图,将被删除的块设置为空闲块，在位图中标为0
    {
    case 0:blockbitmap_buffer[num_block] = blockbitmap_buffer[num_block] & 127; break; //0111 1111b
    case 1:blockbitmap_buffer[num_block] = blockbitmap_buffer[num_block] & 191; break; //1011 1111b
    case 2:blockbitmap_buffer[num_block] = blockbitmap_buffer[num_block] & 223; break; //1101 1111b
    case 3:blockbitmap_buffer[num_block] = blockbitmap_buffer[num_block] & 239; break; //1110 1111b
    case 4:blockbitmap_buffer[num_block] = blockbitmap_buffer[num_block] & 247; break; //1111 0111b
    case 5:blockbitmap_buffer[num_block] = blockbitmap_buffer[num_block] & 251; break; //1111 1011b
    case 6:blockbitmap_buffer[num_block] = blockbitmap_buffer[num_block] & 253; break; //1111 1101b
    case 7:blockbitmap_buffer[num_block] = blockbitmap_buffer[num_block] & 254; break; //1111 1110b
    }
    write_blockbitmap();
    gdt[0].free_block_count++;
    write_gdt();
}

static void delete_inode(unsigned int i)
{
    unsigned short num_inode;
    num_inode = i / 8;      //在块位图中的字节号
    read_inodebitmap();
    switch (i % 8) // 根据字节内位移更新block位图,将被删除的块设置为空闲块，在位图中标为0
    {
    case 0:inodebitmap_buffer[num_inode] = inodebitmap_buffer[num_inode] & 127; break; //0111 1111b
    case 1:inodebitmap_buffer[num_inode] = inodebitmap_buffer[num_inode] & 191; break; //1011 1111b
    case 2:inodebitmap_buffer[num_inode] = inodebitmap_buffer[num_inode] & 223; break; //1101 1111b
    case 3:inodebitmap_buffer[num_inode] = inodebitmap_buffer[num_inode] & 239; break; //1110 1111b
    case 4:inodebitmap_buffer[num_inode] = inodebitmap_buffer[num_inode] & 247; break; //1111 0111b
    case 5:inodebitmap_buffer[num_inode] = inodebitmap_buffer[num_inode] & 251; break; //1111 1011b
    case 6:inodebitmap_buffer[num_inode] = inodebitmap_buffer[num_inode] & 253; break; //1111 1101b
    case 7:inodebitmap_buffer[num_inode] = inodebitmap_buffer[num_inode] & 254; break; //1111 1110b
    }
    write_inodebitmap();
    gdt[0].free_inode_count++;
    write_gdt();
}

void initExt2()
{
    printf("now it is creating a file system of EXT2....\n");
    //初始化分配inode和块的两个变量
    num_last_alloc_block = 0;
    num_last_alloc_inode = 1;
    if (fp != NULL) fclose(fp);
    fp = fopen("./Ext2Simulation", "w+");
    memset(file_opened, 0, sizeof(unsigned int) * 10);
    memset(buffer, 0, sizeof(char) * 1024);
    fseek(fp, BLOCK_SIZE, SEEK_SET);//文件从引导区外面开始读
    fwrite(buffer, BLOCK_SIZE, 1, fp);  //清空磁盘数据块的数据

    read_SuperBlock();
    strcpy(super_block[0].fs_name, "EXT 2 0.0");
    super_block[0].fs_id = 1;
    super_block[0].datablock_size = BLOCK_SIZE;
    super_block[0].first_block = 1;//块大小是1024，所以1作为第一个逻辑块b
    super_block[0].inode_size = sizeof(struct Inode);
    super_block[0].groupblock_size = BLOCK_SIZE * 8;
    super_block[0].block_count = super_block[0].groupblock_size;    //因为只模拟一个块组，所以相等
    super_block[0].inode_count = super_block[0].groupblock_size;
    super_block[0].free_block_count = super_block[0].free_inode_count = BLOCK_COUNT;
    write_SuperBlock();

    read_inode(1);  //根目录节点
    read_dir(0);    //根目录
    //改为根目录
    strcpy(current_dir_name, "root/");
    read_gdt();
    gdt[0].position_block_bitmap = 3 * BLOCK_SIZE;
    gdt[0].position_inode_bitmap = 4 * BLOCK_SIZE;
    gdt[0].first_inode_table = 5 * BLOCK_SIZE;
    gdt[0].free_block_count = BLOCK_COUNT;
    gdt[0].free_inode_count = BLOCK_COUNT;
    gdt[0].contents_count = 0;
    write_gdt();

    read_blockbitmap();
    read_inodebitmap();
    inode[0].i_mode = 518;//1 000 000 110
    inode[0].i_block_count = 0;
    inode[0].i_size = 64;
    inode[0].i_uid = 1;
    inode[0].i_group_id = 0;
    inode[0].i_link_count = 0;
    inode[0].i_access_time = 0;
    inode[0].i_create_time = 0;
    inode[0].i_modify_time = 0;
    inode[0].i_delete_time = 0;
    inode[0].i_blocks[0] = get_free_block();
    inode[0].i_block_count++;
    num_current_dir = get_free_inode();
    inode[0].i_number = num_current_dir;
    write_inode(num_current_dir);

    //根目录的目录项
    strcpy(dir[0].name, ".");
    strcpy(dir[1].name, "..");
    dir[0].i_number = dir[1].i_number = num_current_dir;//把. 和..都设置为根目录
    dir[0].name_len = dir[1].name_len = 0;
    dir[0].file_type = dir[1].file_type = 1;    //目录类型
    write_dir(inode[0].i_blocks[0]);
    fclose(fp);
    printf("succeed to initiate the filesystem!!!!\n");
}

void init() {
    num_last_alloc_block = 0;
    num_last_alloc_inode = 1;
    memset(file_opened, 0, sizeof(unsigned int) * 10);
    strcpy(current_dir_name, "root/");
    num_current_dir = 1;
    fp = fopen("./Ext2Simulation", "r+");
    if (fp == NULL) {
        printf("now there is no file system,we would creating a new fs for you....\n");
        initExt2();
        return;
    }
}

void ShowSuperBlock() {
    read_SuperBlock();
    printf("element\t\t\t\t\tvalue\n");
    printf("filename:\t\t\t%s\n", super_block[0].fs_name);
    printf("filesystem id\t\t\t\t%d\n", super_block[0].fs_id);
    printf("block size:\t\t\t\t%d\n", super_block[0].datablock_size);
    printf("inode size:\t\t\t\t%d\n", super_block[0].inode_size);
    printf("group size:\t\t\t\t%d\n", super_block[0].groupblock_size);
    printf("all blocks count:\t\t\t%d\n", super_block[0].block_count);
    printf("all inodes count:\t\t\t%d\n", super_block[0].inode_count);
    printf("free block count:\t\t\t%d\n", super_block[0].free_block_count);
    printf("free inode count:\t\t\t%d\n", super_block[0].free_inode_count);
}
void makefile(char fileName[23], int type)
{
    unsigned int newInodeNum, i, j, k, flag;
    read_inode(num_current_dir);    //读入当前文件夹下的inode节点

    if (!(i=find_FileOrDirent(fileName, type,&j, &k)))    //如果读入的节点为空
    {
        if (inode[0].i_size == 12*BLOCK_SIZE)    //目录已满，占满了所有数据块
        {
            printf("Directory has no room to be alloced!\n");
            return;
        }
        flag = 1;
        if (inode[0].i_size != inode[0].i_block_count * BLOCK_SIZE)//块没有占满
        {
            i = 0;
            while (flag && i < inode[0].i_block_count)
            {
                read_dir(inode[0].i_blocks[i]); //读入目录文件内容
                j = 0;
                while (j < 32)
                {
                    if (dir[j].i_number == 0)//找到了未分配的目录项
                    {
                        flag = 0;
                        break;
                    }
                    j++;
                }
                i++;
            }
            newInodeNum = dir[j].i_number = get_free_inode();//分配一个新的inode项
            dir[j].name_len = strlen(fileName);             //在目录中加入新的目录项
            dir[j].file_type = type;
            strcpy(dir[j].name, fileName);
            write_dir(inode[0].i_blocks[i - 1]);
        }
        else //当前数据块已占满，现在分配一个新的数据块
        {
            inode[0].i_blocks[inode[0].i_block_count] = get_free_block();
            inode[0].i_block_count++;   //占用数据块数目+1
            read_dir(inode[0].i_blocks[inode[0].i_block_count - 1]);    //读入新数据块
            newInodeNum = dir[0].i_number = get_free_inode();           //分配新的节点
            dir[0].name_len = strlen(fileName);
            dir[0].file_type = type;
            strcpy(dir[0].name, fileName);
            //初始化新快其他项目为0
            for (flag = 1; flag < 32; flag++)
            {
                dir[flag].i_number = 0;
            }
            write_dir(inode[0].i_blocks[inode[0].i_block_count - 1]);
        }
        inode[0].i_size += 16;
        write_inode(num_current_dir);
        //将新增文件的inode节点初始化
        alloc_file_data(newInodeNum, strlen(fileName), type);

    }
    else
    {
        printf("File has already existed!\n");
    }
}