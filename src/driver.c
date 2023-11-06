#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/jiffies.h>
#include <linux/cred.h>
#include <linux/err.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kthread.h>
#include <linux/wait.h>
#include <linux/uaccess.h>
#define BUFFER_SIZE 256


//initializing params
static int kernel_version[3] = {0, 0, 0};
static int timer = 0;
module_param_array(kernel_version, int, NULL, 0644);
module_param(timer, int, 0644);


//flags
static int read_flag = 0;
static int write_flag = 0;

//buffer
static char kernel_buffer[BUFFER_SIZE];

//wait struct
static struct task_struct* wait_thread;
DECLARE_WAIT_QUEUE_HEAD(wait_queue_driver);
static int wait_queue_flag = 0;

//function prototypes
static int __init osAss2Driver_init(void);
static void __exit osAss2Driver_exit(void);
static int osAss2Driver_open(struct inode*, struct file*);
static int osAss2Driver_release(struct inode*, struct file*);
static ssize_t osAss2Driver_read(struct file*, char __user*, size_t, loff_t*);
static ssize_t osAss2Driver_write(struct file*, const char __user*, size_t, loff_t*);


static struct file_operations file_ops = {
    .open = osAss2Driver_open,
    .release = osAss2Driver_release,
    .read = osAss2Driver_read,
    .write = osAss2Driver_write,
};

static struct cdev driver_cdev;
static struct class *dev_class;
dev_t dev = 0; // holds info about minor and major number assigned
static int result = -1;

static int wait_function(void* unused) {
    while(1) {
        pr_info("Waiting for an event... %d\n", wait_queue_flag);
        result = wait_event_timeout(wait_queue_driver, wait_queue_flag != 0, msecs_to_jiffies(timer*1000));
        if (result <= 1)
            return 0;
        if (wait_queue_flag == 1) {
            pr_info("Time elapsed : %d secs\n", timer - (result / HZ));
            printk(KERN_INFO "User:%s\tSucessfully completed the actions within time\n", kernel_buffer);
            return 0;
        }
    }
    // do_exit(0);
    return 0;

}



static int __init osAss2Driver_init(void) {
    int target_version = KERNEL_VERSION(kernel_version[0], kernel_version[1], kernel_version[2]);

    if (target_version != LINUX_VERSION_CODE) {
        printk(KERN_INFO "Incorrect kernel version\n");
        return -1;
    }

    if (alloc_chrdev_region(&dev, 0, 1, "OsAss2Driver") < 0) {
        printk(KERN_INFO "Failed to allocate inode number\n");
        return -1;
    }

    printk(KERN_INFO "MajorNo=%d \t MinorNo=%d \t timer=%d\n", MAJOR(dev), MINOR(dev), timer);

    // create device structure
    cdev_init(&driver_cdev, &file_ops);

    //adding char driver to ssytem
    if (cdev_add(&driver_cdev, dev, 1) < 0) {
        pr_err("Cannot add driver to system\n");
        unregister_chrdev_region(dev, 1);
        return -1;
    }

    if (IS_ERR(dev_class = class_create("OsAss2DriverClass"))) {
        pr_err("Cannot create the struct class\n");
        unregister_chrdev_region(dev, 1);
        return -1;
    }

    if (IS_ERR(device_create(dev_class, NULL, dev, NULL, "OsAss2DriverDevice"))) {
        pr_err("Cannot create the device\n");
        class_destroy(dev_class);
        unregister_chrdev_region(dev, 1);
        return -1;
    }

    wait_thread = kthread_create(wait_function, NULL, "WaitThread");
    if (wait_thread) {
        pr_info("Thread created successfully\n");
        wake_up_process(wait_thread);
    }
    else {
        pr_info("Thread creation failed\n");
    }

    snprintf(kernel_buffer, BUFFER_SIZE, "%s", "<Nothing to read>");

    pr_info("Device driver successfully inserted\n");
    return 0;
}

static int osAss2Driver_open(struct inode* inode, struct file* file) {
    pr_info("Driver was opened\n");
    return 0;
}

static int osAss2Driver_release(struct inode* inode, struct file* file) {
    pr_info("Driver was released\n");
    return 0;
}

static ssize_t osAss2Driver_read(struct file* file, char __user* buf, size_t len, loff_t* offset) {
    if (!read_flag && !write_flag)
        read_flag = 1;

    ssize_t copy_len = (len > BUFFER_SIZE) ? BUFFER_SIZE : len;

    if (copy_len == 0)
        return 0;
    if (copy_to_user(buf, kernel_buffer, copy_len))
        return -EFAULT;
    
    pr_info("Driver read completed successfully\n");
    return copy_len;
}

static ssize_t osAss2Driver_write(struct file* file, const char __user* buf, size_t len, loff_t* offset) {
    if (read_flag && !write_flag)
        write_flag = 1;

    if (len > BUFFER_SIZE)
        return -EINVAL; //size over limit
    if (copy_from_user(kernel_buffer, buf, len))
        return -EFAULT;

    pr_info("Driver write completed successfully\n");
    return len;
}

static void __exit osAss2Driver_exit(void) {
    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&driver_cdev);
    unregister_chrdev_region(dev, 1);

    if (result == -1) {
        wait_queue_flag = 1;
        wake_up(&wait_queue_driver);
    }
    else {
        printk(KERN_INFO "Failed to complete the actions in time\n");

    }

}

module_init(osAss2Driver_init);
module_exit(osAss2Driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("diljith_b210023cs");
MODULE_DESCRIPTION("This driver was created as part of OS theory Ass2");