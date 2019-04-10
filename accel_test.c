#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ACCEL_X_RAW "/sys/bus/iio/devices/iio:device0/in_accel_x_raw"  /*陀螺仪x轴数据接口 */
#define ACCEL_Y_RAW "/sys/bus/iio/devices/iio:device0/in_accel_y_raw"  /*陀螺仪y轴数据接口 */
#define ACCEL_Z_RAW "/sys/bus/iio/devices/iio:device0/in_accel_z_raw"  /*陀螺仪z轴数据接口 */

/*对接口进行读操作函数 */
static int* read_int(char *path,int* val)
{
	int buf[1024];
	int fd,ret;
	fd = open(path, O_RDWR);
	if(fd<0)
	{
		perror("open error");
		exit(1);
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
	int x_raw[1024];
	int y_raw[1024];
	int z_raw[1024];

	while(1)
	{
 		read_int(ACCEL_X_RAW,x_raw);
 		read_int(ACCEL_Y_RAW,y_raw);
 		read_int(ACCEL_Z_RAW,z_raw);
	
		printf("x=%sy=%sz=%s\n",x_raw,y_raw,z_raw);
	}
	return 0;
}
