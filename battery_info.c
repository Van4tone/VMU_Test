#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CAPACITY "/sys/class/power_supply/bq40z50/capacity"      /*当前电池电量百分比 */
#define TEMP     "/sys/class/power_supply/bq40z50/temp"          /*当前电池温度 */
#define VOLTAGE  "/sys/class/power_supply/bq40z50/voltage_now"   /*当前电池电压 */
#define STATUS   "/sys/class/power_supply/bq40z50/status"        /*当前电池状态 */
#define RIGHTS_LEVEL "0644"

/*设置文件权限 */
void set_rights(char *path)
{
	char cmd[128] = {0};
	sprintf(cmd,"chmod %s %s",RIGHTS_LEVEL,path);
	system(cmd);
}

/*对接口进行读操作函数 */
static int* read_int(char *path,int* val)
{
	int buf[1024];
	int fd,ret;
	fd = open(path, O_RDWR);

	if(fd<0)
	{
		set_rights(path);	
		fd = open(path, O_RDWR);
		if(fd <0)
		{
		printf("open %s error\n",path);
		exit(1);
		}
	}
	memset(buf,'\0',1024*sizeof(int));
	memset(val,'\0',sizeof(int));
	ret = read(fd,buf,sizeof(buf));
	if(ret<0)
	{
		perror("read error");
	}
	memcpy(val,buf,sizeof(buf));
//	printf("%s value is %s\n",path,buf);
	close(fd);
	return val;
}

/*对接口进行写操作函数 */
static void write_int(char *path, int val)
{
	char buf[1024];
	int fd;
	fd = open(path, O_RDWR);

	if(fd<0)
	{
		perror("open error");
		exit(1);
	}

	sprintf(buf,"%d",val);

	if(write(fd,buf,strlen(buf))!= strlen(buf))
	{
		perror("write error");
		exit(1);
	}

	close(fd);
}


int main(int argc, const char *argv[])
{
	int capacity[1024];
	int voltage_now[1024];
	int temp[1024];
	int status[1024];

	while(1)
	{
 		read_int(CAPACITY,capacity);
		read_int(VOLTAGE,voltage_now);
 		read_int(TEMP,temp);
 		read_int(STATUS,status);
		printf("capacity:%svoltage_now:%sstatus:%stemp:%s\n",capacity,voltage_now,status,temp);
		sleep(30);	
	}
	return 0;
}
