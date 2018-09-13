#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

char *get_current_dir_name(void);

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

	struct tm *t;
	time_t tt;
	time(&tt);
	t = localtime(&tt);
	char *dir = malloc(1024);

	memset(dir, 0, sizeof(dir));
	dir = (char*)get_current_dir_name();
	memcpy(&dir[strlen(get_current_dir_name())], "/output", strlen("/output"));
	printf("dir = %s\n", dir);
/* 	printf("%4dÄê%02dÔÂ%02dÈÕ %02d:%02d:%02d\n", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec); */

	char filename[100] = {0};
	char commond_line[100] = {0};

	sprintf(filename, "%4d_%02d_%02d__%02d_%02d_%02d.h264", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

/* 	printf("filename = %s\n", filename); */

	unsigned int i;

	for (i = 0; i < 24; i++){
		time(&tt);
		t = localtime(&tt);
		memset(filename, 0, sizeof(filename));
		sprintf(filename, "%4d_%02d_%02d__%02d_%02d_%02d.h264", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
/* 		printf("filename = %s\n", filename); */

		memcpy(commond_line, "raspivid -w 640 -h 480 -p 0  -t 3600000 -o video.h264", 
			   strlen("raspivid -w 640 -h 480 -p 0  -t 3600000 -o video.h264"));
		memcpy(&commond_line[strlen("raspivid -w 640 -h 480 -p 0  -t 3600000 -o ")], filename, strlen(filename));
		printf("commond_line = %s\n", commond_line);
		system(commond_line);
/* 		sleep(10); */
	}

	return 0;
}
