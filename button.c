#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>

int main(void)
{
  int fd = open("/sys/class/gpio/gpio67/value",O_RDONLY);
  if (errno == 2) 
  {
    printf("This program assumes gpio67 to be setup for input.\n");
    return 0;
  } 	
  else if (fd < 0)
  { 
    fprintf(stderr,"Unexpected error opening /sys/class/gpio/gpio67/value: %s\n",
           strerror(errno));
    return -1;
  }
  
  int fdloadavg = open("/proc/loadavg",O_RDONLY);
  if (fdloadavg < 0) 
  {
    fprintf(stderr,"Unexpected error opening /proc/loadavg");
    return -1;
  }  
//  char value[1];
  char loadavg[80];
  int sleeptime=150000;
  struct pollfd fdset[1];
  int nfds = 1; 
  int rc;
  int timeout = 3000;

  while (1)
  {
    usleep(sleeptime);
    memset((void*)fdset, 0, sizeof(fdset));
    fdset[0].fd = fd;
    fdset[0].events = POLLPRI;

    rc = poll(fdset, nfds, timeout);
    if(rc < 0)
     {
       fprintf(stderr,"Unexpected error reading /sys/class/gpio/gpio67/value: %s\n",
       strerror(errno));
       return -1;
     }
    if(fdset[0].revents & POLLPRI)
    {
       read(fdloadavg,loadavg,sizeof(loadavg));
       write(1,loadavg,strlen(loadavg));
       lseek(fdloadavg,0,SEEK_SET);
    } 
    else write(1,"waiting...\n",11); 
    if (lseek(fd,0,SEEK_SET)<0) return 1;
  }

}

