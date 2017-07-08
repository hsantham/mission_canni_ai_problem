#include <stdlib.h>
#include <queue.h>

// FIFO QUEUE
// Insert new element at the end
void insert (q_node_t** queue, 
             void*      valueToInsert) {
    
    if (queue == NULL) {
        assert(0);
    } 

    // Create new node
    q_node_t *newNode = malloc(sizeof(q_node_t));
    if(newNode == NULL) {
        assert(0);
    }
    newNode->value = valueToInsert;
    newNode->next = NULL;

    q_node_t *temp = *queue;
    if(temp == NULL)
    {
        *queue = newNode;
    }
    else
    {
        while(temp->next)
            temp=temp->next;
        temp->next = newNode;
    }
}

// FIFO queue
// Pop first element in the queue
void* pop(q_node_t** queue) {
    if (queue == NULL) {
        assert(0);
    } 
    
    q_node_t* nodeToBeDeleted = *queue;
    if(nodeToBeDeleted == NULL) {
        return NULL;
    }

    *queue = (*queue)->next;
    return nodeToBeDeleted->value;
}
