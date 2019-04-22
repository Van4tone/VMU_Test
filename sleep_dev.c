#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define KEY_DEV  "/dev/input/event0"
#define POWER_STATE "/sys/power/state"
#define DARK  "mem"
#define LIGHT "freeze standby mem"

void send_cmd(const char *dev_name, const char *status)
{
	char cmd[128] = {0};
//	char status[32] = {0};

	sprintf(cmd, "echo %s > %s",status, dev_name);
	system(cmd);
}

pthread_rwlock_t rwlock;


int main(int argc, const char *argv[])
{
	int keys_fd,dark_fd;
	int flags = 1;
	struct input_event keys_event;	
	char buf[16]={0};
	const char * dark_events = argv[1];

	keys_fd = open(KEY_DEV, O_RDONLY);
	if(keys_fd <0)
	{
		perror("KEY_DEV open");
		return -1;
	}

	dark_fd = open(POWER_STATE,O_RDONLY);
	if(dark_fd < 0)
	{
		perror("DARK_SCREEN open");
		return -1;
	}

	while(1)
	{
		if(read(keys_fd, &keys_event, sizeof(keys_event))==sizeof(keys_event))
		{
			if(keys_event.type == EV_KEY)
			{

				sleep(1);

				if(flags &&keys_event.code == KEY_VOLUMEUP && keys_event.value == 1)
				{
					flags++;
		//			usleep(10000);
					if(flags%2==0)
					{
						printf("system sleep\n");
						send_cmd(POWER_STATE,DARK);
					}
					else
					{
						printf("wake up\n");
					}
				}
			}
		}
	}
	return 0;
}









