#ifndef COUNTER_H
#define COUNTER_H

#include <linux/types.h>
#include <linux/mutex.h>
#include <linux/cdev.h>

#define DEVICE_NAME "counter"
#define DEFAULT_PERIOD 1000

// Structure representing the counter device
typedef struct
{
    __u32 value;
    __u32 period;
    struct mutex lock;
    struct cdev cdev;
} counter_t;

#endif // COUNTER_H