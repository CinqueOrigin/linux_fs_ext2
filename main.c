#include"init.h"

int main() {
	char filename[23];
	int type=0;
	while (1)
	{
		init();
		printf("-------------------------------------------------------\n");
		printf("please enter a number to execute a function------------\n");
		printf("1:read the information of superblock-------------------\n");
		printf("2:init the file system of EXT2-------------------------\n");
		printf("!!if you init the file system,it would clear all data!-\n");
		printf("3:make a file with name--------------------------------\n");
		printf("0:exit-------------------------------------------------\n");
		int i;
		printf("please enter a number to execute:");
		scanf("%d", &i);
		if (i == 1) {
			ShowSuperBlock();
			printf("\n \n \n");
		}
		else if (i == 2)
		{
			initExt2();
			printf("\n \n \n");
		}
		else if (i == 3) {
			printf("please enter a filename and type");
			printf("(0 common file 1 dirent file):");
			scanf("%s%d", filename,&type);
			makefile(filename, type);
		}
		else
		{
			printf("error input!!!!\n");
			return 0;
		}
	}
	return 0;
}