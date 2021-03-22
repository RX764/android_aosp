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


#define GPIOLED_CNT 1
#define GPIOLED_NAME "gpioled"
#define LEDOFF  0 //关
#define LEDON   1 //开

struct gpioled_dev
{
    dev_t devid;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    int major;
    int minor;
    struct device_node *nd;
    int led_gpio;
    atomic_t lock;
};

struct gpioled_dev gpioled;


static int led_open(struct inode *inode, struct file *filp)
{
    if (!atomic_dec_and_test(&gpioled.lock))
    {
        atomic_inc(&gpioled.lock);
        return -EBUSY;
    }
    filp->private_data = &gpioled;
    return 0;
}

static ssize_t led_read(struct file *filp,char __user *buf,size_t cnt,loff_t *offt)
{
    return 0;
}

static ssize_t led_write(struct file *filp,const char __user *buf,size_t cnt,loff_t *offt)
{
    int retvalue;
    unsigned char databuf[1];
    unsigned char ledstat;
    struct gpioled_dev *dev = filp->private_data;
    retvalue = copy_from_user(databuf,buf,cnt);
    if (retvalue <0)
    {
        pr_err("kernel write failed!\r\n");
        return -EFAULT;
    }
    ledstat = databuf[0];
    if (ledstat == LEDON)   
    {
        pr_err("LED is ON!\r\n");
    }else if(ledstat == LEDOFF){
        pr_err("LED is OFF!\r\n");
    }
    return 0;
}

static int led_release(struct inode *inode,struct file *filp)
{
    struct gpioled_dev *dev = filp->private_data;
    atomic_inc(&dev->lock);
    pr_err("device is release, LED is OFF!\r\n");
    return 0;
}

static struct file_operations gpioled_fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .read = led_read,
    .write = led_write,
    .release = led_release,
};

static int __init led_init(void)
{
    int ret = 0;
    atomic_set(&gpioled.lock,1);

    /* 1、创建设备号 */
    if (gpioled.major) { /* 定义了设备号 */
        gpioled.devid = MKDEV(gpioled.major, 0);
        register_chrdev_region(gpioled.devid, GPIOLED_CNT,GPIOLED_NAME);
    } 
    else { /* 没有定义设备号 */
        alloc_chrdev_region(&gpioled.devid, 0, GPIOLED_CNT,GPIOLED_NAME); /* 申请设备号 */
        gpioled.major = MAJOR(gpioled.devid);/* 获取分配号的主设备号 */
        gpioled.minor = MINOR(gpioled.devid);/* 获取分配号的次设备号 */
    }
    printk("gpioled major=%d,minor=%d\r\n",gpioled.major,gpioled.minor);

    /* 2、初始化 cdev */
    gpioled.cdev.owner = THIS_MODULE;
    cdev_init(&gpioled.cdev, &gpioled_fops);
    
    /* 3、添加一个 cdev */
    cdev_add(&gpioled.cdev, gpioled.devid, GPIOLED_CNT);
    
    /* 4、创建类 */
    gpioled.class = class_create(THIS_MODULE, GPIOLED_NAME);
    if (IS_ERR(gpioled.class)) {
        return PTR_ERR(gpioled.class);
    }

     /* 5、创建设备 */
    gpioled.device = device_create(gpioled.class, NULL,gpioled.devid, NULL, GPIOLED_NAME);
    if (IS_ERR(gpioled.device)) {
        return PTR_ERR(gpioled.device);
    }
    return 0;
}

static void __exit led_exit(void)
{
    /* 注销字符设备驱动 */
    cdev_del(&gpioled.cdev);/* 删除 cdev */
    unregister_chrdev_region(gpioled.devid, GPIOLED_CNT);
    
    device_destroy(gpioled.class, gpioled.devid);
    class_destroy(gpioled.class);
}
   
module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("xxx");

