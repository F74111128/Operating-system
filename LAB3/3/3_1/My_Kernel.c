#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <asm/current.h>

#define procfs_name "Mythread_info"
#define BUFSIZE  1024
char buf[BUFSIZE];

static ssize_t Mywrite(struct file *fileptr, const char __user *ubuf, size_t buffer_len, loff_t *offset){
    /* Do nothing */
	return 0;
}


static ssize_t Myread(struct file *fileptr, char __user *ubuf, size_t buffer_len, loff_t *offset){
    /*Your code here*/
    struct task_struct *task;
    struct task_struct *cur = current;
    int len = 0;

    if (*offset > 0 || buffer_len < BUFSIZE) { // Already read (only need to read once)
        return 0;
    }

    for_each_thread(cur, task) {
        if(cur==task){
            continue;
        }
        len += snprintf(buf + len, BUFSIZE - len,
                        "PID: %d, TID: %d, Priority: %d, State: %ld\n",
                        task->tgid, 
                        task->pid,  
                        task->prio, 
                        task->stats 
        ); //avoid overflow (+len)(-len) remain BUFSIZE-Len
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