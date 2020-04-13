#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/statfs.h> 

char *get_current_dir_name(void);
char filename[100] = {0};
char filename_convert[100] = {0};
char g_flag_convert = 0;

int get_disk_space_left(void) 
{ 
	struct statfs diskInfo;  
    statfs("/", &diskInfo);  
    unsigned long long totalBlocks = diskInfo.f_bsize;  
    unsigned long long totalSize = totalBlocks * diskInfo.f_blocks;  
    size_t mbTotalsize = totalSize>>20;  
    unsigned long long freeDisk = diskInfo.f_bfree*totalBlocks;  
    size_t mbFreedisk = freeDisk>>20;  
    printf ("/  total=%dMB, free=%dMB\n", mbTotalsize, mbFreedisk); 

	return mbFreedisk;
/*
    statfs("/boot", &diskInfo);  
    totalBlocks = diskInfo.f_bsize;  
    totalSize = totalBlocks * diskInfo.f_blocks;  
    mbTotalsize = totalSize>>20;  
    freeDisk = diskInfo.f_bfree*totalBlocks;  
    mbFreedisk = freeDisk>>20;  
    printf ("/boot  total=%dMB, free=%dMB\n", mbTotalsize, mbFreedisk);
    statfs("/dev/shm", &diskInfo);
    totalBlocks = diskInfo.f_bsize;
    totalSize = totalBlocks * diskInfo.f_blocks;
    mbTotalsize = totalSize>>20;
    freeDisk = diskInfo.f_bfree*totalBlocks;
    mbFreedisk = freeDisk>>20;
    printf ("/dev/shm  total=%dMB, free=%dMB\n", mbTotalsize, mbFreedisk);
	return 0;
*/
}

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

unsigned int find_and_remove_mp4(void)
{
	int fd;
	int ret;
	unsigned int items;
	unsigned int length;
	unsigned int i;
	char name[25] = {0};
	char command_rm[28] = {0};

	char buf[1024*1024] = {0};
	
	system("find *.mp4 > find_mp4.txt");

	fd = open("find_mp4.txt", O_RDWR);

	ret = read(fd, buf, 1024*1024);


	length = strlen(buf);
	items = length / 25;

	memcpy(command_rm, "rm ", 3);
	memcpy(&command_rm[3], &buf[0], 24);
	system(command_rm);
	
	close(fd);
	system("rm find_mp4.txt");

	return items;
}

void *video_convert_task(void)
{
	int i;
	int items;
	char commond_line[100] = {0};
	char convert_filename[100] = {0};
	
	system("rm *.h264");
	usleep(100000);
	
	while (get_disk_space_left() < 4000)
	{
		items = find_and_remove_mp4();
		printf("items = %d\n", items);
		usleep(100000);
	}
	
	while(1)
	{
		if (g_flag_convert == 1)
		{
			g_flag_convert = 0;
			
			memset(commond_line, 0, sizeof(commond_line));
			memcpy(commond_line, "MP4Box -fps 30 -add ", strlen("MP4Box -fps 30 -add "));
			memcpy(&commond_line[strlen("MP4Box -fps 30 -add ")], filename_convert, strlen(filename_convert));
			memcpy(convert_filename, filename_convert, strlen(filename_convert));
			memcpy(&convert_filename[strlen(convert_filename) - 4], "mp4", strlen("mp4"));
			convert_filename[strlen(convert_filename) - 1] = 0;
			memcpy(&commond_line[strlen(commond_line)], " ", 1);
			memcpy(&commond_line[strlen(commond_line)], convert_filename, strlen(convert_filename));
			printf("convert commond_line = %s\n", commond_line);
			system(commond_line);
			
			while (get_disk_space_left() < 4000)
			{
				items = find_and_remove_mp4();
				printf("items = %d\n", items);
				usleep(100000);
			}
			
			memset(commond_line, 0, sizeof(commond_line));
			memcpy(commond_line, "rm ", strlen("rm "));
			memcpy(&commond_line[strlen(commond_line)], filename_convert, strlen(filename_convert));
			system(commond_line);
			// system("rm *.h264");
		}
		usleep(100000);
	}
}

/* 86400000 一小时一个 */
/* 14400000 10分钟一个 */
int main(int argc, char** argv)
{	
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

	char commond_line[100] = {0};

	sprintf(filename, "%4d_%02d_%02d__%02d_%02d_%02d.h264", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);


	unsigned int i;

	pthread_t id1;
	int ret = 0;
	
	ret = pthread_create(&id1, NULL, (void *)video_convert_task,NULL);
	if (ret)
	{
		printf("video_convert_task create fail\n");
	}

/* 	1440 保留10天log */
	for (i = 0; i < 6*24*10; i++){
		time(&tt);
		t = localtime(&tt);
		sprintf(filename, "%4d_%02d_%02d__%02d_%02d_%02d.h264", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

		memcpy(commond_line, "raspivid -p 1080,850,160,120 -rot 180 -w 1280 -h 720 -t 600000 -o video.h264", 
			   strlen("raspivid -p 1080,850,160,120 -rot 180 -w 1280 -h 720 -t 600000 -o video.h264"));
		memcpy(&commond_line[strlen("raspivid -p 1080,850,160,120 -rot 180 -w 1280 -h 720 -t 600000 -o ")], filename, strlen(filename));
		printf("commond_line = %s\n", commond_line);
		system(commond_line);
		memcpy(filename_convert, filename, sizeof(filename));
		g_flag_convert = 1;
		sleep(1);
	}

	return 0;
}
