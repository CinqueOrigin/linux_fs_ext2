#include "init.h"
#include<string.h>
#define BLOCK_START    1053696             //�����ڵ����ʼλ��+�����ڵ���С����ʼλ����5�����С�������ڵ���С��128B*8192=1048576
//����������Ϣд��ģ�����
static void write_SuperBlock()
{
    fp = fopen("./Ext2Simulation", "r+");
    fseek(fp, BLOCK_SIZE, SEEK_SET);            //��ȥǰ1024K���������⣬��1024KBλ�ÿ�ʼд������
    fwrite(super_block, BLOCK_SIZE, 1, fp);  //���������ļ���
    fflush(fp); //���̽��������������������֤�����ڴ����ݵ�һ����
}
//��ģ������ж��볬����
static void read_SuperBlock()
{
    fseek(fp, BLOCK_SIZE, SEEK_SET);
    fread(super_block, BLOCK_SIZE, 1, fp);//��ȡ���ݵ������黺�����У��ļ�����������
}

static void write_gdt(void)
{
    fp = fopen("./Ext2Simulation", "r+");
    fseek(fp, BLOCK_SIZE+ BLOCK_SIZE, SEEK_SET);    //�������������������ռ��������
    fwrite(gdt, 32, 1, fp);                         //groupdesc��С��32B
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
//��ȡһ�����ݿ飬�����ػ�õ����ݿ�Ŀ��i
static unsigned int get_free_block()
{   //�����ж��Ƿ��п��п飬����У��͸����ϴη���Ŀ��������ڿ�λͼ�е�λ�ã�����8���ɣ����������������䣬Ȼ�����һ�����п鲢�ı�λͼ���ɡ�

    unsigned short cur = num_last_alloc_block;      //curΪ�ϴη�������ݿ��λ�ã���λ�ǿ�λͼ�ֽں�+��λͼ�ֽ���λ�ƣ������
    unsigned char con = 128; // 1000 0000
    int flag = 0;           //��¼�ֽ���λ��
    if (gdt[0].free_block_count == 0)
    {
        printf("all blocks are allocated!!\n");
        return 0;
    }
    read_blockbitmap();     //��ģ������ж����λͼ����Ϣ
    cur /= 8;               //cur��Ϊcur���ڵ��ֽ�
    while (blockbitmap_buffer[cur] == 255)//������ֱ���ڿ�λͼ���ҵ����еĿ�
    {
        if (cur == 1023)cur = 0; //���һ���ֽ�Ҳ�Ѿ�������ͷ��ʼѰ��
        else cur++;
    }
    while (blockbitmap_buffer[cur] & con) //��һ���ֽ����Ҿ����ĳһ��bit
    {
        con = con / 2;      //����һλ
        flag++;
    }
    blockbitmap_buffer[cur] = blockbitmap_buffer[cur] + con;    //���е�0��1����
    num_last_alloc_inode = cur * 8 + flag;                      //��η����λ��д������У��������������

    write_blockbitmap();
    gdt[0].free_block_count--;
    super_block[0].free_block_count--;
    write_gdt();
    write_SuperBlock();
    printf("we have alloc a new block!!!\n");
    return num_last_alloc_block;

}
//��ȡһ��inode�������ػ�õ����ݿ�Ŀ��i
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

    cur = (cur - 1) / 8;   //��һ�������1�����Ǵ洢�Ǵ�0��ʼ��,������������ϸ΢���죬���0��ʼ����
    //printf("%s",)
    while (inodebitmap_buffer[cur] == 255) //�ȿ����ֽڵ�8��λ�Ƿ��Ѿ�����
    {
        if (cur == 1023)cur = 0;
        else cur++;
    }
    while (inodebitmap_buffer[cur] & con)  //Ѱ��0����λ��
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
    read_inode(num_current_dir); //���뵱ǰĿ¼
    j = 0;
    while (j < inode[0].i_block_count&&j<12)        //Ϊ�˼򻯲�ʹ��һ������������
    {
        //����inode����Ϣ����jѭ��ɨ��ռ�õ����ݿ�
        read_dir(inode[0].i_blocks[j]);
        k = 0;
        //ѭ��ɨ��Ŀ¼���µ��֪���ҵ����ͺ��ļ�����ƥ���Ҷ�Ӧinode���ڵ�
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
    read_inode(i);      //����i�ڵ���Ϣ
    //�������ͨ�ļ�ֻ��Ҫ��ʼ��������
    if (type == 0) {    
        inode[0].i_size = 0;
        inode[0].i_block_count = 0;
        inode[0].i_mode = 0755;
    }
    //Ŀ¼�ļ���Ҫ�޸�Ŀ¼�����,��Ҫ���������Ŀ¼�.��..
    else if (type == 1) {
        //���ȴ���i�ڵ���Ϣ
        inode[0].i_size = 32;
        inode[0].i_block_count = 1;
        inode[0].i_blocks[0] = get_free_block();
        //0�ǵ�ǰĿ¼��.��1�Ǹ�Ŀ¼..
        dir[0].i_number = i;
        dir[1].i_number = num_current_dir;
        dir[0].name_len = length;
        dir[1].name_len = len_current_dir;
        //���߶���Ŀ¼��
        dir[0].file_type = dir[1].file_type = 0;

        for (type = 2; type < 32; type++)
            dir[type].i_number = 0;
        strcpy(dir[0].name, ".");
        strcpy(dir[1].name, "..");
        //��Ŀ¼��ʼ��������д��ģ�����
        write_dir(inode[0].i_blocks[0]);
        //�޸ķ���ģʽ
        inode[0].i_mode = 01006;    
    }
    write_inode(i);
}

static void delete_block(unsigned int i)
{
    unsigned short num_block;
    num_block = i / 8;      //�ڿ�λͼ�е��ֽں�
    read_blockbitmap();
    switch (i % 8) // �����ֽ���λ�Ƹ���blockλͼ,����ɾ���Ŀ�����Ϊ���п飬��λͼ�б�Ϊ0
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
    num_inode = i / 8;      //�ڿ�λͼ�е��ֽں�
    read_inodebitmap();
    switch (i % 8) // �����ֽ���λ�Ƹ���blockλͼ,����ɾ���Ŀ�����Ϊ���п飬��λͼ�б�Ϊ0
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
    //��ʼ������inode�Ϳ����������
    num_last_alloc_block = 0;
    num_last_alloc_inode = 1;
    if (fp != NULL) fclose(fp);
    fp = fopen("./Ext2Simulation", "w+");
    memset(file_opened, 0, sizeof(unsigned int) * 10);
    memset(buffer, 0, sizeof(char) * 1024);
    fseek(fp, BLOCK_SIZE, SEEK_SET);//�ļ������������濪ʼ��
    fwrite(buffer, BLOCK_SIZE, 1, fp);  //��մ������ݿ������

    read_SuperBlock();
    strcpy(super_block[0].fs_name, "EXT 2 0.0");
    super_block[0].fs_id = 1;
    super_block[0].datablock_size = BLOCK_SIZE;
    super_block[0].first_block = 1;//���С��1024������1��Ϊ��һ���߼���b
    super_block[0].inode_size = sizeof(struct Inode);
    super_block[0].groupblock_size = BLOCK_SIZE * 8;
    super_block[0].block_count = super_block[0].groupblock_size;    //��Ϊֻģ��һ�����飬�������
    super_block[0].inode_count = super_block[0].groupblock_size;
    super_block[0].free_block_count = super_block[0].free_inode_count = BLOCK_COUNT;
    write_SuperBlock();

    read_inode(1);  //��Ŀ¼�ڵ�
    read_dir(0);    //��Ŀ¼
    //��Ϊ��Ŀ¼
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

    //��Ŀ¼��Ŀ¼��
    strcpy(dir[0].name, ".");
    strcpy(dir[1].name, "..");
    dir[0].i_number = dir[1].i_number = num_current_dir;//��. ��..������Ϊ��Ŀ¼
    dir[0].name_len = dir[1].name_len = 0;
    dir[0].file_type = dir[1].file_type = 1;    //Ŀ¼����
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
    read_inode(num_current_dir);    //���뵱ǰ�ļ����µ�inode�ڵ�

    if (!(i=find_FileOrDirent(fileName, type,&j, &k)))    //�������Ľڵ�Ϊ��
    {
        if (inode[0].i_size == 12*BLOCK_SIZE)    //Ŀ¼������ռ�����������ݿ�
        {
            printf("Directory has no room to be alloced!\n");
            return;
        }
        flag = 1;
        if (inode[0].i_size != inode[0].i_block_count * BLOCK_SIZE)//��û��ռ��
        {
            i = 0;
            while (flag && i < inode[0].i_block_count)
            {
                read_dir(inode[0].i_blocks[i]); //����Ŀ¼�ļ�����
                j = 0;
                while (j < 32)
                {
                    if (dir[j].i_number == 0)//�ҵ���δ�����Ŀ¼��
                    {
                        flag = 0;
                        break;
                    }
                    j++;
                }
                i++;
            }
            newInodeNum = dir[j].i_number = get_free_inode();//����һ���µ�inode��
            dir[j].name_len = strlen(fileName);             //��Ŀ¼�м����µ�Ŀ¼��
            dir[j].file_type = type;
            strcpy(dir[j].name, fileName);
            write_dir(inode[0].i_blocks[i - 1]);
        }
        else //��ǰ���ݿ���ռ�������ڷ���һ���µ����ݿ�
        {
            inode[0].i_blocks[inode[0].i_block_count] = get_free_block();
            inode[0].i_block_count++;   //ռ�����ݿ���Ŀ+1
            read_dir(inode[0].i_blocks[inode[0].i_block_count - 1]);    //���������ݿ�
            newInodeNum = dir[0].i_number = get_free_inode();           //�����µĽڵ�
            dir[0].name_len = strlen(fileName);
            dir[0].file_type = type;
            strcpy(dir[0].name, fileName);
            //��ʼ���¿�������ĿΪ0
            for (flag = 1; flag < 32; flag++)
            {
                dir[flag].i_number = 0;
            }
            write_dir(inode[0].i_blocks[inode[0].i_block_count - 1]);
        }
        inode[0].i_size += 16;
        write_inode(num_current_dir);
        //�������ļ���inode�ڵ��ʼ��
        alloc_file_data(newInodeNum, strlen(fileName), type);

    }
    else
    {
        printf("File has already existed!\n");
    }
}