#include <stdio.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/time.h>
#include <linux/slab.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MYFS module");
MODULE_AUTHOR("Batbileg Nomuundalai");

static int number = 0;
module_param(number, int, 0);
static struct inode **myfs_inodes = NULL;


#define SLABNAME "myfs_inode_cache"
struct kmem_cache *cache = NULL;

static const unsigned long MYFS_MAGIC_NUMBER = 0x13131313;

struct myfs_inode {
    int i_mode;
    unsigned long i_ino;
};

static int size = sizeof(struct myfs_inode);

static struct inode *myfs_make_inode(struct super_block *sb, int mode) {
    struct inode *ret = new_inode(sb);
    struct myfs_inode *myfs_inode = NULL;
    
    if (ret) {
        inode_init_owner(ret, NULL, mode);
        ret->i_size = PAGE_SIZE;
        ret->i_atime = ret->i_mtime = ret->i_ctime = current_time(ret);
        myfs_inode = kmem_cache_alloc(cache, GFP_KERNEL);
        *myfs_inode = (struct myfs_inode) {
        .i_mode = ret->i_mode,
        .i_ino = ret->i_ino,
        };
    ret->i_private = myfs_inode;
    }

    return ret;
}

static void myfs_put_super(struct super_block *sb) {
    printk(KERN_DEBUG "MYFS super block destroyed!\n");
}

static int myfs_drop_inode(struct inode *inode) {
    kmem_cache_free(cache, inode->i_private);
    return generic_drop_inode(inode);
}
static struct super_operations const myfs_super_ops = {
    .put_super = myfs_put_super,
    .statfs = simple_statfs,
    .drop_inode = myfs_drop_inode,
};

static int myfs_fill_sb(struct super_block *sb, void *data, int silent) {
    struct inode *root = NULL;
    int i = 0;

    sb->s_blocksize = PAGE_SIZE;
    sb->s_blocksize_bits = PAGE_SHIFT;
    sb->s_magic = MYFS_MAGIC_NUMBER;
    sb->s_op = &myfs_super_ops;

    root = myfs_make_inode(sb, S_IFDIR | 0755);
    if (!root) {
        printk(KERN_ERR "MYFS make inode failed!\n");
        return -ENOMEM;
    }

    root->i_op = &simple_dir_inode_operations;
    root->i_fop = &simple_dir_operations;

    sb->s_root = d_make_root(root);
    if (!sb->s_root) {
        printk(KERN_ERR "MYFS create root failed!\n");
        iput(root);
        return -ENOMEM;
    }

    for (i = 0; i < number; i++) {
        myfs_inodes[i] = myfs_make_inode(sb, S_IFDIR | 0755);
        if (!myfs_inodes[i]) {
            printk(KERN_ERR "MYFS kmem_cache_alloc error\n");
            for (i = 0; i < number; i++) {
                myfs_drop_inode(myfs_inodes[i]);
            }
           
            kmem_cache_destroy(cache);
            kfree(myfs_inodes);
            return -ENOMEM;
        }
    }
    return 0;
}

static struct dentry *myfs_mount(struct file_system_type *type, int flags, char const *dev, void *data) {
    struct dentry *const entry = mount_nodev(type, flags, data, myfs_fill_sb);
    if (IS_ERR(entry)) {
        printk(KERN_ERR "MYFS mount failed!\n");
    } else {
        printk(KERN_DEBUG "MYFS mounted!\n");
    }

    return entry;
}
    
static struct file_system_type myfs_type = {
    .owner = THIS_MODULE,
    .name = "myfs",
    .mount = myfs_mount,
    .kill_sb = kill_litter_super,
};

static int __init myfs_init(void) {
    int ret = register_filesystem(&myfs_type);
    if (ret != 0) {
        printk(KERN_ERR "MYFS_MODULE can't register FS!\n");
        return ret;
    }

    myfs_inodes = kmalloc(sizeof(struct inode *) * number, GFP_KERNEL);
    if (!myfs_inodes) {
        printk(KERN_ERR "MYFS allocating error\n");
        kfree(myfs_inodes);
        return -ENOMEM;
    }

    cache = kmem_cache_create(SLABNAME, size, 0, SLAB_POISON, NULL);
        
    if (!cache) {
        printk(KERN_ERR "MYFS kmem_cache_create error\n");
        kfree(myfs_inodes);
        kmem_cache_destroy(cache);
        return -ENOMEM;
    }

    printk(KERN_DEBUG "MYFS_MODULE successfully loaded\n");

    return 0;
}

static void __exit myfs_exit(void) {
    int i = 0;
    int ret = unregister_filesystem(&myfs_type);
    if (ret != 0) {
        printk(KERN_ERR "MYFS_MODULE can't unregister FS!\n");
    }
    for (i = 0; i < number; i++) {
        myfs_drop_inode(myfs_inodes[i]);
    }
    
    kmem_cache_destroy(cache);
    kfree(myfs_inodes);
    printk(KERN_DEBUG "MYFS_MODULE successfully unloaded\n");
}

module_init(myfs_init);
module_exit(myfs_exit);
