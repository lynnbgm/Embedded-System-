 
/* Necessary includes for device drivers */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/jiffies.h> /* jiffies */
#include <asm/system.h> /* cli(), *_flags */
#include <asm/uaccess.h> /* copy_from/to_user */
#include <linux/vmalloc.h>

#include <linux/time.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/fs_struct.h>
#include <linux/jiffies.h>

MODULE_LICENSE("Dual BSD/GPL");

static int mytimer_fasync(int fd, struct file *filp, int mode);
static int mytimer_open(struct inode *inode, struct file *filp);
static ssize_t mytimer_read(struct file *filp, char *buf, size_t count, loff_t *f_pos);
static int mytimer_release(struct inode *inode, struct file *filp);
static ssize_t mytimer_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
static int mytimer_init(void);
static void mytimer_exit(void);
static void timer_handler(unsigned long data);
int chartonum(char*, unsigned int);
void numtochar(unsigned int, char*);
void concat(char*, char*, char*, char);

/*
 * The file operations for the pipe device
 * (some are overlayed with bare scull)
 */
struct file_operations mytimer_fops = {
	write: mytimer_write,
	open: mytimer_open,
	read: mytimer_read,
	release: mytimer_release,
	fasync: mytimer_fasync
};

/* Declaration of the init and exit functions */
module_init(mytimer_init);
module_exit(mytimer_exit);

static int mytimer_major = 61;
struct fasync_struct *async_queue; /* asynchronous readers */
static struct timer_list * fasync_timer;
static char *request_buffer;
static char *timer_buffer;
static char *message_buffer;
static int timerset = 0;
static char timername[128];
static int callerpid;
static char *callername;
static unsigned long loadtime;
struct file *curr_filp;

static struct proc_dir_entry *proc_entry;
static int proc_read( char *page, char **start, off_t off, int count, int *eof, void *data);

static int mytimer_init(void) {
	int result;

	/* Registering device */
	result = register_chrdev(mytimer_major, "mytimer", &mytimer_fops);
	if (result < 0)
	{
		printk(KERN_ALERT
			"mytimer: cannot obtain major number %d\n", mytimer_major);
		return result;
	}

	/* Allocating buffers */
	fasync_timer = (struct timer_list *) kmalloc(sizeof(struct timer_list), GFP_KERNEL);
	request_buffer = kmalloc(256, GFP_KERNEL); 
	message_buffer = kmalloc(256, GFP_KERNEL); 
	timer_buffer = kmalloc(256, GFP_KERNEL); 
	if (!request_buffer)
	{ 
		printk(KERN_ALERT "Insufficient kernel memory\n"); 
		result = -ENOMEM;
		goto fail; 
	} 
	if (!message_buffer)
	{ 
		printk(KERN_ALERT "Insufficient kernel memory\n"); 
		result = -ENOMEM;
		goto fail; 
	} 
	if (!timer_buffer)
	{ 
		printk(KERN_ALERT "Insufficient kernel memory\n"); 
		result = -ENOMEM;
		goto fail; 
	} 		
	memset(request_buffer, 0, 256);
	memset(timer_buffer, 0, 256);
	memset(message_buffer, 0, 256);


    proc_entry = create_proc_entry( "mytimer", 0644, NULL );

    if (proc_entry == NULL) {

      printk(KERN_INFO "mytimer: Couldn't create proc entry\n");
      return -ENOMEM;

    } else {
		
	  proc_entry->read_proc = proc_read;
      proc_entry->owner = THIS_MODULE;
      
    }
	
	/* get time module is loaded */	
	loadtime = jiffies;	

	printk("Module mytimer loaded.\n");
	return 0;

fail: 
	mytimer_exit(); 
	return result;
}

static void mytimer_exit(void) {
	
	/* Delete any pending timers */
	if (timerset == 1)
		del_timer(fasync_timer);

	/* Freeing the major number */
	unregister_chrdev(mytimer_major, "mytimer");
	
	/* Freeing buffer memory */
	if (request_buffer)
		kfree(request_buffer);
	if (message_buffer)
		kfree(message_buffer);
	if (timer_buffer)
		kfree(timer_buffer);	
	if (fasync_timer)
		kfree(fasync_timer);

	remove_proc_entry("mytimer", &proc_root);
//	remove_proc_entry("mytimer", NULL);

	printk(KERN_ALERT "Removing mytimer module\n");

}

static ssize_t mytimer_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{ 
	unsigned int remaining;
	char* numptr;

	numptr =  kmalloc(sizeof(char) * 11, GFP_KERNEL);	
		
	/* Calculate remaining time for single timers */	
	if (timerset == 1) {
		long diff = ((long)fasync_timer->expires - (long)jiffies);
		if (diff > 0) {
			remaining = (int) (diff/HZ);
			numtochar(remaining, numptr);
		}
	} else {
		return 0;
	}

	/* Transfering data to user space */
	if (copy_to_user(buf,numptr, 11))
	{
		return -EFAULT;
	}
	kfree(numptr);				
	return 11; 
	
}

static int mytimer_open(struct inode *inode, struct file *filp) {
	return 0;
}

static int mytimer_release(struct inode *inode, struct file *filp) {
	mytimer_fasync(-1, filp, 0);
	return 0;
}

static ssize_t mytimer_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos) {

	unsigned long duration;
	unsigned int numsize;
	int temp, msgbegin, modify;
	char *mbptr = message_buffer;
	char *tbptr = timer_buffer;

	/* Get timer request from user */
	if (copy_from_user(request_buffer + *f_pos, buf, count))
	{
		return -EFAULT;
	}

	/* Remove existing timer */		
	if (count == 0)	{
		if (timerset == 1) {
			del_timer(fasync_timer);
			timerset = 0;
			strcpy(timername,""); 
		}
		return 0;	
	}
	
	/* Parse request to determine if new or modify request*/
	msgbegin = numsize = modify = 0;	
	for (temp = *f_pos; temp < count + *f_pos; temp++) {
		if (!msgbegin && request_buffer[temp] == ' ') {
			msgbegin = 1;
		} else {
			if (!msgbegin) {
				tbptr += sprintf(tbptr, "%c", request_buffer[temp]);
				numsize++;
			} else {
				mbptr += sprintf(mbptr, "%c", request_buffer[temp]);
			}
		}
	}
	tbptr += sprintf(tbptr, "%c", '\0');
	mbptr += sprintf(mbptr, "%c", '\0');			
		
	if (strcmp(timername,message_buffer) == 0) {
		modify = 1;
	} else {
		if (timerset == 1) {
			return 0;
		}
	}		

	/* Set new timer */
	duration = (unsigned long) (chartonum(timer_buffer,numsize));		
	if (modify == 0) {
		setup_timer(fasync_timer, timer_handler, 0);
		mod_timer(fasync_timer, jiffies + (duration * HZ));
		timerset = 1;
		strcpy(timername,message_buffer);
		callerpid = current->pid;
		callername = current->comm;	
		curr_filp =	filp;
		return 1;
		
	/* Modify existing timer */		
	} else if (modify == 1) {
		mytimer_fasync(-1, curr_filp, 0);
		setup_timer(fasync_timer, timer_handler, 0);		
		mod_timer(fasync_timer,jiffies  + (duration * HZ));
		callerpid = current->pid;
		strcpy(callername,current->comm);
		curr_filp = filp;
		return 2;
	}	
	
	return count;
}

static int mytimer_fasync(int fd, struct file *filp, int mode) {
	return fasync_helper(fd, filp, mode, &async_queue);
}

static void timer_handler(unsigned long data) {
	if (async_queue)
		kill_fasync(&async_queue, SIGIO, POLL_IN);

	del_timer(fasync_timer);
	timerset = 0;
	strcpy(timername,""); 
}

static int proc_read( char *page, char **start, off_t off,
                   int count, int *eof, void *data )
{
	int len, timesince;
	long elapsed;
	unsigned int remaining;

	if (off > 0) {
	*eof = 1;
	return 0;
	}

	elapsed = (long)jiffies - (long)loadtime;
	timesince = (int) (elapsed/HZ);
				
	len = sprintf(page, "Module: mytimer \nInserted: %d secs ago\n",timesince);

	/* Calculate remaining time for all set timers */	
	if (timerset == 1) {
		long diff = ((long)fasync_timer->expires - (long)jiffies);
		if (diff > 0) {
			remaining = (int) (diff/HZ);
			len = sprintf(page,
				"Module: mytimer \nInserted: %d secs ago\nProcess id: %d\nProcess name: %s\nTimer message: %s\nRemaining time: %d secs\n", 
				timesince, callerpid, callername, timername, remaining);		
		}
	}

	return len;

}

/* convert char* into int */
int chartonum(char* strnum, unsigned int size) {
	int num = 0;
	int i = 0;
	for (i = 0; i < size; i++) {
		num += (*strnum - 48);
		num *= 10;
		strnum++;
	}
	num /= 10;
	return num;
}

/* convert int into char* */
void numtochar(unsigned int num, char* nptr) {
	char c[11] = "0123456789\0";
	char reverse[11];
	unsigned int mod = 0, i = 0, len = 0;
	while (num >= 10) {
		mod = num % 10;
		num = num / 10;
		reverse[i] = c[mod];
		i++;
	}
	reverse[i] = c[num];
	len = i + 1;
	reverse[len] = '\0';

	i = 0;
	while (i < len) {
		*nptr = reverse[len - i - 1];
		i++;
		nptr++;
	}
	*nptr = '\0';
}

/* concatenate first and second using delim, and store in new */
void concat(char* new, char* first, char* second, char delim) {
	while(*first) {
		*new = *first;
		new++;
		first++;
	}	
	*new = delim;
	new++;
	while(*second) {
		*new = *second;
		new++;
		second++;
	}
	*new = '\0';
}
