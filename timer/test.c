// #include<sys/types.h>
// #include<sys/stat.h>
// #include<fcntl.h>
// #include<stdio.h>
// #include<unistd.h>

// int main(int argc, char *argv[])
// {
//     int ret = 0;
//     int fd = 0;
//     char *filename = argv[1];
//     char readbuf[100], writebuf[100];

//     fd = open(filename,O_RDWR);
//     if (fd < 0)
//     {
//         printf("can not open file %s \r\n",filename);
//         return -1;
//     }
//     ret = read(fd,readbuf,20);
//     if (ret < 0)
//     {
//         printf("read file %s is failed！ \r\n",filename);
        
//     }
//     else{

//     }
//     if (ret < 0 )     
//     {
//         printf("write file %s is failed！ \r\n",filename);
//     }
//     ret = close(fd);
//     if (ret < 0 )     
//     {
//         printf("close file %s is failed！ \r\n",filename);
//     }
//     return 0;
// }


#include "stdio.h"
#include "unistd.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "stdlib.h"
#include "string.h"

#define LEDOFF 0
#define LEDON 1

int main(int argc, char *argv[])
{
    int fd, retvalue;
    char *filename;
    unsigned char cnt = 0;
    unsigned char databuf[1];

    if(argc != 3){
        printf("Error Usage!\r\n");
    return -1;
    }

    filename = argv[1];

    /* 打开 beep 驱动 */
    fd = open(filename, O_RDWR);
    if(fd < 0){
        printf("file %s open failed!\r\n", argv[1]);
        return -1;
    }

    databuf[0] = atoi(argv[2]); /* 要执行的操作：打开或关闭 */

    /* 向/dev/gpioled 文件写入数据 */
    retvalue = write(fd, databuf, sizeof(databuf));
    if(retvalue < 0){
        printf("LED Control Failed!\r\n");
        close(fd);
        return -1;
    }

    /* 模拟占用 25S LED */
    while(1) {
        sleep(5);
        cnt++;
        printf("App running times:%d\r\n", cnt);
        if(cnt >= 5) break;
    }

    printf("App running finished!");
    retvalue = close(fd); /* 关闭文件 */
    if(retvalue < 0){
        printf("file %s close failed!\r\n", argv[1]);
        return -1;
    }
    return 0;
}