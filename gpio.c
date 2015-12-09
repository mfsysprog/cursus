#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include "gpio.h"

// poll werkt nog niet

int main(void)
{
  struct mygpio mygpio1;
  strncpy(mygpio1.gpionum, "67", 2);
  char gpiopath[ 1024 ];
  char str1[20] = "/sys/class/gpio/gpio";
  char str2[6] = "/value";
  snprintf( gpiopath, sizeof( gpiopath ), "%s%s%s", str1, mygpio1.gpionum, str2 );
  int fd = open(gpiopath,O_RDONLY);
  // not yet exported
  if (errno == 2) 
  {
    printf("Exporting gpio%s...\n",mygpio1.gpionum);
    fd = open("/sys/class/gpio/export",O_WRONLY);
    if (fd < 0)
    {
       fprintf(stderr,"Unexpected error opening /sys/class/gpio/export: %s\n",
       strerror(errno));
       return -1;
    };
    
    if (write(fd,mygpio1.gpionum,sizeof(mygpio1.gpionum)) < 0)
    {
       fprintf(stderr,"Unexpected error writing /sys/class/gpio/export: %s\n",
       strerror(errno));
       return -1;
    } 
     
    if (close(fd) < 0)
    {
       fprintf(stderr,"Unexpected error closing /sys/class/gpio/export: %s\n",
       strerror(errno));
       return -1;
    }
    fd = open(gpiopath,O_RDONLY);
    
    if (fd < 0)
    {
       fprintf(stderr,"Unexpected error '%s' opening %s after export was succesful.\n",
       strerror(errno),gpiopath);
       return -1;
    }
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
  nfds_t nfds = 1; 
  int rc;
  int timeout = -1;
 
  while (1)
  {
    usleep(sleeptime);

    memset((void*)fdset, 0, sizeof(fdset));
    fdset[0].fd = fd;
    fdset[0].events = POLLPRI;     
    rc = poll(fdset, nfds, timeout);
    if(rc < 0)
     {
       fprintf(stderr,"Unexpected error '%s' polling %s.\n",
       strerror(errno),gpiopath);
       return -1;
     }
    if(fdset[0].revents & POLLPRI)
    {
     fprintf(stderr,"rc: %i, revents:%#06x\n",rc, fdset[0].revents);
     //printf("ret: %i - revents[0]: %i", ret, fds[0].revents);
     //  read(fdloadavg,loadavg,sizeof(loadavg));
     //  write(1,loadavg,strlen(loadavg));
     //  lseek(fdloadavg,0,SEEK_SET);
    } 
    else write(1,"waiting...\n",11); 
    if (lseek(fd,0,SEEK_SET)<0) return 1;
  }

}

