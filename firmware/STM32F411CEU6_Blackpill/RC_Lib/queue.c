/*
 * queue.c
 *
 *  Created on: May 4, 2024
 *      Author: Felix
 */

#include <stdio.h>
#include "queue.h"

// Function to create a queue
void initializeQueue(Queue* q) {
    q->front = -1;
    q->rear = -1;
}

// Check if the queue is full
int isFull(Queue* q) {
    if(q->rear == SIZE - 1)
        return 1;
    else
        return 0;
}

// Check if the queue is empty
int isEmpty(Queue* q) {
    if(q->front == -1)
        return 1;
    else
        return 0;
}

// Add elements into queue
void enqueue(Queue* q, int value){
	if(isFull(q))
	{
		//printf("Queue is full\n");
	}
	else
	{
		if(q->front == -1)
		{
			q->front = 0;
		}
		q->rear++;
		q->items[q->rear] = value;
	}
}

// Remove elements from queue
int dequeue(Queue* q){
   int retVal = 0;
	if(isEmpty(q)) {
    	{
    		//printf("Queue is empty\n");
    	}
    } else {
       // printf("Removed %d\n", q->items[q->front]);
    	retVal = q->items[q->front];
        q->front++;
        if(q->front > q->rear)
        {
            //printf("Resetting queue\n");
            q->front = q->rear = -1;
        }
    }
	return retVal;
}
