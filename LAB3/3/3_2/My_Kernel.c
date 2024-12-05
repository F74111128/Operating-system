#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <asm/current.h>

#define procfs_name "Mythread_info"
#define BUFSIZE  1024
char buf[BUFSIZE]; //kernel buffer

static ssize_t Mywrite(struct file *fileptr, const char __user *ubuf, size_t buffer_len, loff_t *offset){
    /*Your code here*/
   int len = 0;
    //check size 
    if (buffer_len > BUFSIZE) {
        return -EINVAL;
    }
    if (copy_from_user(buf, ubuf, buffer_len)) {
        return -EFAULT;
    }
    buf[buffer_len] = '\0'; // 確保緩衝區以 NULL 結尾
    return len;
    /****************/
}


static ssize_t Myread(struct file *fileptr, char __user *ubuf, size_t buffer_len, loff_t *offset){
    /*Your code here*/
    struct task_struct *task;
    int len = 0;
    if (*offset > 0 || buffer_len < BUFSIZE) { // Already read
        return 0;
    }

    for_each_thread(current, task) {
        len += snprintf(buf + len, BUFSIZE - len,
                        "%s\n,PID: %d, TID: %d,time: %d\n",
                        buf,
                        task->tgid, 
                        task->pid,  
                        task->utime/100/1000 
        ); //avoid overflow (+len -len)
         if (len >= BUFSIZE) {
            break; // avoid overflow
        }
    }
    if (copy_to_user(ubuf, buf, len)) {
        return -EFAULT;
    }

    *offset = len; // Update offset
    return len;

    /****************/
}

static struct proc_ops Myops = {
    .proc_read = Myread,
    .proc_write = Mywrite,
};

static int My_Kernel_Init(void){
    proc_create(procfs_name, 0644, NULL, &Myops);   
    pr_info("My kernel says Hi");
    return 0;
}

static void My_Kernel_Exit(void){
    pr_info("My kernel says GOODBYE");
}

module_init(My_Kernel_Init);
module_exit(My_Kernel_Exit);

MODULE_LICENSE("GPL");
