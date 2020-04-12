#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>

#define KEEP_VIDEO_NUM						(6*24)
// (6*24*2)

char *get_current_dir_name(void);

int readFileList(char *basePath)
{
	unsigned int i = 0;
    DIR *dir;
    struct dirent *ptr;
    char base[1000];
	char tmp[5] = {0};

    if ((dir=opendir(basePath)) == NULL)
    {
        perror("Open dir error...");
        exit(1);
    }

    while ((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
            continue;
        else if(ptr->d_type == 8){    		///file
            printf("d_name:%s/%s\n",basePath,ptr->d_name);
			printf("tmp = %s\n", ptr->d_name);
		}
        else if(ptr->d_type == 10)    ///link file
            printf("d_name:%s/%s\n",basePath,ptr->d_name);
        else if(ptr->d_type == 4)    ///dir
        {
            memset(base,'\0',sizeof(base));
            strcpy(base,basePath);
            strcat(base,"/");
            strcat(base,ptr->d_name);
            readFileList(base);
        }
    }
    closedir(dir);
    return 1;
}

void test_read_file_list(void)
{
	DIR *dir;
	char basePath[1000];

	///get the current absoulte path
	memset(basePath,'\0',sizeof(basePath));
	getcwd(basePath, 999);
	printf("the current dir is : %s\n",basePath);

	///get the file list
/* 	memset(basePath,'\0',sizeof(basePath)); */
/* 	strcpy(basePath,"./XL"); */
	readFileList(basePath);
	return;
}

unsigned int find_and_remove(void)
{
	int fd;
	int ret;
	unsigned int items;
	unsigned int length;
	unsigned int i;
	char name[26] = {0};
	char command_rm[29] = {0};

	char buf[1024*1024] = {0};
	
	system("find *.h264 > find.txt");

	fd = open("find.txt", O_RDWR);

	ret = read(fd, buf, 1024*1024);


	length = strlen(buf);
	items = length / 26;

	if (items > KEEP_VIDEO_NUM){
		memcpy(command_rm, "rm ", 3);
		memcpy(&command_rm[3], &buf[0], 25);
/* 		printf("command_rm = %s\n", command_rm); */
		system(command_rm);
	}
	
	system("rm find.txt");

	return items;
}

/* 86400000 一小时一个 */
/* 14400000 10分钟一个 */
int main(int argc, char** argv)
{
/*
	time_t tNow =time(NULL);  
	time_t tEnd = tNow + 1800;  

	struct tm ptm = { 0 };  
	struct tm ptmEnd = { 0 };  
	localtime_r(&tNow, &ptm);  
	localtime_r(&tEnd, &ptmEnd);  

	char szTmp[50] = {0};  
	strftime(szTmp,50,"%H:%M:%S",&ptm);  
	char szEnd[50] = {0};  
	strftime(szEnd,50,"%H:%M:%S",&ptmEnd);  
	printf("%s \n",szTmp);  
	printf("%s \n",szEnd);  


	system("PAUSE");  
*/

/*	system("raspivid -w 320 -h 240 -p 0  -t 1000 -o video.h264"); */
/*	sleep(2); */

/*
	test_read_file_list();
	return 0;
*/

	unsigned int items;

	struct tm *t;
	time_t tt;
	time(&tt);
	t = localtime(&tt);
	char *dir = malloc(1024);

	memset(dir, 0, sizeof(dir));
	dir = (char*)get_current_dir_name();
	memcpy(&dir[strlen(get_current_dir_name())], "/output", strlen("/output"));
	printf("dir = %s\n", dir);
/* 	printf("%4d年%02d月%02d日 %02d:%02d:%02d\n", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec); */

	char filename[100] = {0};
	char commond_line[100] = {0};

	sprintf(filename, "%4d_%02d_%02d__%02d_%02d_%02d.h264", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

/* 	printf("filename = %s\n", filename); */

	unsigned int i;

	for (i = 0; i < KEEP_VIDEO_NUM; i++){
		find_and_remove();
	}

/* 	1440 保留10天log */
	for (i = 0; i < 6*24*10; i++){
		time(&tt);
		t = localtime(&tt);
		memset(filename, 0, sizeof(filename));
		sprintf(filename, "%4d_%02d_%02d__%02d_%02d_%02d.h264", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
/* 		printf("filename = %s\n", filename); */

/* 		raspivid -op 100 -f -rot 180 -w 1280 -h 1024 -p 0  -t 14400000 -o video.h264 */

		memcpy(commond_line, "raspivid -p 1080,850,160,120 -rot 180 -w 1280 -h 720 -t 600000 -o video.h264", 
			   strlen("raspivid -p 1080,850,160,120 -rot 180 -w 1280 -h 720 -t 600000 -o video.h264"));
		memcpy(&commond_line[strlen("raspivid -p 1080,850,160,120 -rot 180 -w 1280 -h 720 -t 600000 -o ")], filename, strlen(filename));
		printf("commond_line = %s\n", commond_line);
		system(commond_line);

		do{
			items = find_and_remove();
			printf("items = %d\n", items);
			items = find_and_remove();
			printf("items = %d\n", items);
		}while (items > KEEP_VIDEO_NUM);
/* 		sleep(10); */
	}

	return 0;
}
