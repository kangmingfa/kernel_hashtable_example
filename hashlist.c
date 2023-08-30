#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/hashtable.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Kernel Module with Hash Table Example");

struct my_data {
    int key;
    char value[20];
    struct hlist_node node;
};

DEFINE_HASHTABLE(my_hash_table, 6); // Define a hash table with size 2^6

static void insert_data(int key, const char *value) {
    struct my_data *new_data = kmalloc(sizeof(struct my_data), GFP_KERNEL);
    new_data->key = key;
    snprintf(new_data->value, sizeof(new_data->value), "%s", value);
    hash_add(my_hash_table, &new_data->node, key);
}

static struct my_data *search_data(int key) {
    struct my_data *data;
    hash_for_each_possible(my_hash_table, data, node, key) {
        if (data->key == key) {
            return data;
        }
    }
    return NULL;
}

static void cleanup_hash_table(void) {
    struct my_data *data;
    struct hlist_node *tmp;
    int bkt;
    hash_for_each_safe(my_hash_table, bkt,  tmp, data, node) {
        hash_del(&data->node);
        kfree(data);
    }
}

static int __init hashtable_module_init(void) {
    printk(KERN_INFO "Hash Table Module Initialized\n");

    insert_data(5, "five");
    insert_data(10, "ten");
    insert_data(2, "two");

    struct my_data *data = search_data(10);
    if (data) {
        printk(KERN_INFO "Found data: key=%d, value=%s\n", data->key, data->value);
    } else {
        printk(KERN_INFO "Data not found\n");
    }

    return 0;
}

static void __exit hashtable_module_exit(void) {
    printk(KERN_INFO "Hash Table Module Exited\n");

    // Cleanup code to free allocated memory and destroy the hash table
    cleanup_hash_table();
}

module_init(hashtable_module_init);
module_exit(hashtable_module_exit);
