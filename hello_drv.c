#include <linux/module.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>

/*1.确定主设备号*/
static int major=0;
static char kernel_buf[1024];
static struct class *hello_class;

#define MIN(a,b) (a<b?a:b)
/*3.实现对应的open/read/write等函数，填入file_operations结构体*/
static ssize_t hello_drv_read (struct file *file, char __user *buf, size_t size, loff_t *offset)
{
    printk("%s %s line %d\n",__FILE__,__FUNCTION__,__LINE__);
    copy_to_user(buf,kernel_buf,MIN(1024,size));
    return MIN(1024,size);
}

static ssize_t hello_drv_write (struct file *file, const char __user *buf, size_t size, loff_t *offset)
{
    printk("%s %s line %d\n",__FILE__,__FUNCTION__,__LINE__);
    copy_from_user(kernel_buf,buf,MIN(1024,size));/*从用户空间拷贝到驱动程序中定义的buffer中来*/
    return MIN(1024,size);
}

static int hello_drv_open (struct inode *node, struct file *file)
{
    printk("%s %s line %d\n",__FILE__,__FUNCTION__,__LINE__);
    return 0;
}

static int hello_drv_close (struct inode *, struct file *)
{
    printk("%s %s line %d\n",__FILE__,__FUNCTION__,__LINE__);
    return 0;
}

/*2.定义自己的file_operations结构体*/
static struct file_operations hello_drv
{
    .ower   =THIS_MODULE,
    .open   =hello_drv_open,
    .read   =hello_drv_read,
    .write  =hello_drv_write,
    .release=hello_drv_close,
};
/*4.把file_operations结构体告诉内核：注册驱动程序*/

/*5.谁来注册驱动程序呀？得有一个入口函数：安装驱动程序时，就会去调用这个入口函数*/
static int __init hello_init(void)
{
    int err;
    register_chrdev(0,"hello",&hello_drv);      /*应用程序想要访问该驱动程序需要通过这个设备节点/dev/hello*/
    hello_class=class_create(THIS_MODULE,"hello");
    err=PTR_ERR(hello_class);
    if(IS_ERR(hello_class))
    {
        unregister_chrdev(major,"hello");
        return -1;
    }
    device_create(hello_class,NULL,MKDEV(major,0),NULL,"hello");/*创建设备节点/dev/hello*/
    return 0;
}

/*6.有入口函数就应该有出口函数：卸载驱动程序时，寄回去调用这个出口函数  */
static void __exit hello_exit(void)
{
    device_destroy(hello_class,MKDEV(major,0)); /* 销毁设备节点 */
    class_destory(hello_class);
    unregister_chrdev(major,"hello");
}
/*7.其他完善：提供设备信息，自动创建设备节点*/
module_init(hello_init);    /*把hello_init修饰成入口函数*/
module_exit(hello_exit);    /*把hello_exit修饰成出口函数*/

MODULE_LICENSE("GPL");      /*驱动程序遵守GPL协议*/
