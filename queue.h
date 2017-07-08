#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <assert.h>

struct node_t {
    void*           value;
    struct node_t*  next;
};

typedef struct node_t q_node_t;

void insert (q_node_t** queue, 
             void*      valueToInsert);

void* pop(q_node_t** queue);

#endif 
