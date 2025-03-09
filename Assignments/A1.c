/**
 * seconds.c
 *
 * Mitchell von Dehn and Teaken Brazeau February 4, 2025
 * SFWRENG 3SH3, Winter 2025
 * 
 * Kernel Version: 6.8.0-51-generic
 * Distribution ID: Ubuntu
 * Description: Ubuntu 24.04.1 LTS
 * Release: 24.04
 * Codename: noble
 */
 
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/jiffies.h>

#define BUFFER_SIZE 128
#define PROC_NAME "seconds"

static unsigned long start_jiffies;

/*Function Prototype*/
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);

static const struct proc_ops my_proc_ops = {
    .proc_read = proc_read,
};

// Function that is called when the module is loaded.
int proc_init(void) {
    start_jiffies = jiffies;
    
    proc_create(PROC_NAME, 0, NULL, &my_proc_ops);
    
    printk(KERN_INFO "proc/%s created\n", PROC_NAME);
    return 0;  
}

// Function that is called when the module is removed.
void proc_exit(void){
    remove_proc_entry(PROC_NAME,NULL);
    
    printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

// Function that is called every time /proc/seconds is read.
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos) {
    int rv;
    char buffer[BUFFER_SIZE];
    static int completed = 0;
    unsigned long elapsed_jiffies = jiffies - start_jiffies;
    unsigned long elapsed_seconds = elapsed_jiffies / HZ;
    
    if (completed) {
      completed = 0;
      return 0;
    }
    
    completed = 1;
    
    // output message
    rv = sprintf(buffer, "Elapsed seconds: %lu\n", elapsed_seconds);
    
    copy_to_user(usr_buf, buffer, rv);

    return rv;
}

//Registering entry and exit points.

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Seconds Module");
MODULE_AUTHOR("Mitch von Dehn and Teaken Brazeau");


