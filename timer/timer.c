/*ubuntu 自带的内核timer_list不一致，所以无法运行*/
#include <linux/debugfs.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>
#include <linux/of.h>
#include <linux/gpio.h>
#include <linux/of_gpio.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/of_regulator.h>
#include <linux/extcon.h>
#include <linux/usb/tcpm.h>
#include <linux/atomic.h>
#include <linux/spinlock.h>

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/timer.h>
#include <linux/jiffies.h>


#define TIMER_CNT 1
#define TIMER_NAME "timer"

struct timer_dev
{
    dev_t devid;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    int major;
    int minor;
    struct device_node *nd;
    struct timer_list timer;
};

struct timer_dev timerdev;

void timer_func(/*unsigned long dummy*/)
{
	// struct timer_dev *dev = (struct timer_dev*)dummy;
    static int status = 1;
    status = !status;
    pr_err("jiang: the status value is %d\r\n",status);
}

static int timer_open(struct inode *inode, struct file *filp)
{
    return 0;
}

static ssize_t timer_read(struct file *filp,char __user *buf,size_t cnt,loff_t *offt)
{
    return 0;
}

static ssize_t timer_write(struct file *filp,const char __user *buf,size_t cnt,loff_t *offt)
{
    return 0;
}

static int timer_release(struct inode *inode,struct file *filp)
{
    return 0;
}

static struct file_operations timerdev_fops = {
    .owner = THIS_MODULE,
    .open = timer_open,
    .read = timer_read,
    .write = timer_write,
    .release = timer_release,
};

static int __init timer_init(void)
{
    int ret = 0;

    /* 1、创建设备号 */
    if (timerdev.major) { /* 定义了设备号 */
        timerdev.devid = MKDEV(timerdev.major, 0);
        register_chrdev_region(timerdev.devid, TIMER_CNT,TIMER_NAME);
    } 
    else { /* 没有定义设备号 */
        alloc_chrdev_region(&timerdev.devid, 0, TIMER_CNT,TIMER_NAME); /* 申请设备号 */
        timerdev.major = MAJOR(timerdev.devid);/* 获取分配号的主设备号 */
        timerdev.minor = MINOR(timerdev.devid);/* 获取分配号的次设备号 */
    }
    printk("timerdev major=%d,minor=%d\r\n",timerdev.major,timerdev.minor);

    /* 2、初始化 cdev */
    timerdev.cdev.owner = THIS_MODULE;
    cdev_init(&timerdev.cdev, &timerdev_fops);
    
    /* 3、添加一个 cdev */
    cdev_add(&timerdev.cdev, timerdev.devid, TIMER_CNT);
    
    /* 4、创建类 */
    timerdev.class = class_create(THIS_MODULE, TIMER_NAME);
    if (IS_ERR(timerdev.class)) {
        return PTR_ERR(timerdev.class);
    }

     /* 5、创建设备 */
    timerdev.device = device_create(timerdev.class, NULL,timerdev.devid, NULL, TIMER_NAME);
    if (IS_ERR(timerdev.device)) {
        return PTR_ERR(timerdev.device);
    }

    // 初始化定时器
    init_timer(&timerdev.timer);
    timerdev.timer.function = timer_func;
    // timerdev.timer.expires =  jiffies + (HZ);
    timerdev.timer.expires = jiffies+msecs_to_jiffies(1000);
    // timerdev.timer.data = (unsigned long)&timerdev;//timer_func;的参数

    // 添加到系统
    add_timer(&timerdev.timer);

    return 0;
}

static void __exit timer_exit(void)
{
    // 删除定时器
    del_timer_sync(&timerdev.timer);
    /* 注销字符设备驱动 */
    cdev_del(&timerdev.cdev);/* 删除 cdev */
    unregister_chrdev_region(timerdev.devid, TIMER_CNT);
    
    device_destroy(timerdev.class, timerdev.devid);
    class_destroy(timerdev.class);
}
   
module_init(timer_init);
module_exit(timer_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("xxx");

