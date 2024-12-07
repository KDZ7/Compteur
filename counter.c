#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include "counter.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("MKGL: Marwa-Kim-Gaby-Lou");
MODULE_DESCRIPTION("Variable period counter module");
MODULE_VERSION("0.0.0");

// Paramètres globaux
static int num_devices = 3;         // Nombre de périphériques
static int delay = 100;             // Délai en ms entre les incréments
static int major;                   // Numéro majeur
static struct class *counter_class; // Classe pour les fichiers /dev
static counter_t *pcounter;         // Tableau des compteurs

module_param(num_devices, int, S_IRUGO);
MODULE_PARM_DESC(num_devices, "Number of counter devices (default=3)");
module_param(delay, int, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(delay, "Delay (ms) between counter increments (default=100)");

/* Initialisation d'un périphérique compteur */
static void counter_init(counter_t *cnt)
{
    mutex_init(&cnt->lock);
    cnt->value = 0;
    cnt->period = DEFAULT_PERIOD;
}

/* Lecture du compteur */
static ssize_t counter_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    counter_t *cnt = file->private_data; // Récupération de l'objet compteur
    char kbuf[32];
    int len;

    mutex_lock(&cnt->lock);
    len = snprintf(kbuf, sizeof(kbuf), "%u\n", cnt->value);
    cnt->value += cnt->period; // Incrémenter selon la période
    mutex_unlock(&cnt->lock);

    if (copy_to_user(buf, kbuf, len))
        return -EFAULT;

    *ppos = 0;
    msleep(delay); // Introduire un délai pour simuler une lecture fluide
    return len;
}

/* Écriture pour modifier la période */
static ssize_t counter_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
    counter_t *cnt = file->private_data;
    char kbuf[16];
    unsigned int new_period;

    if (count == 0 || count >= sizeof(kbuf))
        return -EINVAL;

    if (copy_from_user(kbuf, buf, count))
        return -EFAULT;

    kbuf[count] = '\0';
    // La période doit être strictement positive
    if (kstrtouint(kbuf, 10, &new_period) || new_period == 0)
        return -EINVAL;

    mutex_lock(&cnt->lock);
    cnt->period = new_period; // Changer la période
    mutex_unlock(&cnt->lock);

    pr_info("Counter%d: Period updated to %u\n", (int)(cnt - pcounter), new_period);
    return count;
}

/* Ouverture du périphérique */
static int counter_open(struct inode *inode, struct file *file)
{
    counter_t *cnt = container_of(inode->i_cdev, counter_t, cdev);
    file->private_data = cnt; // Associer l'instance compteur au fichier
    return 0;
}

/* Fermeture du périphérique */
static int counter_release(struct inode *inode, struct file *file)
{
    return 0;
}

/* Opérations sur le périphérique */
static const struct file_operations counter_fops = {
    .owner = THIS_MODULE,
    .read = counter_read,
    .write = counter_write,
    .open = counter_open,
    .release = counter_release,
    .llseek = no_llseek,
};

/* Initialisation du module */
static int __init counter_module_init(void)
{
    int ret, i;
    dev_t dev;

    if (num_devices <= 0)
        return -EINVAL;

    // Allocation dynamique d'un numéro majeur
    ret = alloc_chrdev_region(&dev, 0, num_devices, DEVICE_NAME);
    if (ret < 0)
    {
        pr_err("Failed to allocate major number\n");
        return ret;
    }
    major = MAJOR(dev);

    // Allocation mémoire pour les compteurs
    pcounter = kzalloc(num_devices * sizeof(counter_t), GFP_KERNEL);
    if (!pcounter)
    {
        unregister_chrdev_region(dev, num_devices);
        return -ENOMEM;
    }

    // Création de la classe
    counter_class = class_create(THIS_MODULE, DEVICE_NAME);
    if (IS_ERR(counter_class))
    {
        kfree(pcounter);
        unregister_chrdev_region(dev, num_devices);
        return PTR_ERR(counter_class);
    }

    // Initialisation et enregistrement des périphériques
    for (i = 0; i < num_devices; i++)
    {
        dev_t dev_num = MKDEV(major, i);
        counter_init(&pcounter[i]);

        cdev_init(&pcounter[i].cdev, &counter_fops);
        pcounter[i].cdev.owner = THIS_MODULE;

        ret = cdev_add(&pcounter[i].cdev, dev_num, 1);
        if (ret < 0)
        {
            pr_err("Failed to add cdev for device %d\n", i);
            continue;
        }

        if (IS_ERR(device_create(counter_class, NULL, dev_num, NULL, DEVICE_NAME "%d", i)))
        {
            pr_err("Failed to create device %d\n", i);
            cdev_del(&pcounter[i].cdev);
            continue;
        }
    }

    pr_info("Counter module loaded with major number %d\n", major);
    return 0;
}

/* Nettoyage du module */
static void __exit counter_module_exit(void)
{
    int i;

    for (i = 0; i < num_devices; i++)
    {
        device_destroy(counter_class, MKDEV(major, i));
        cdev_del(&pcounter[i].cdev);
    }

    class_destroy(counter_class);
    kfree(pcounter);
    unregister_chrdev_region(MKDEV(major, 0), num_devices);

    pr_info("Counter module unloaded\n");
}

module_init(counter_module_init);
module_exit(counter_module_exit);
