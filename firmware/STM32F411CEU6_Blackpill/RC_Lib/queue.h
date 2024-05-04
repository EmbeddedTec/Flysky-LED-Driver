/*
 * queue.h
 *
 *  Created on: May 4, 2024
 *      Author: Felix
 */

#ifndef QUEUE_H_
#define QUEUE_H_



#define SIZE 100

// Define a Queue
typedef struct Queue {
    int items[SIZE];
    int front;
    int rear;
} Queue;

// Function prototypes
void initializeQueue(Queue* q);
int isFull(Queue* q);
int isEmpty(Queue* q);
void enqueue(Queue* q, int value);
int dequeue(Queue* q);



#endif /* QUEUE_H_ */
