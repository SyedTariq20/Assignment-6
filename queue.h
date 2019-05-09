#include <stdio.h>
#include <stdlib.h>
 
struct Node {
	double data;
	struct Node *nextnode;
};
 
struct Queue {
	struct Node *frontNode;
	struct Node *lastNode;
	unsigned int nodesize;
};
 
void init(struct Queue *q) {
	q->frontNode = NULL;
	q->lastNode = NULL;
	q->nodesize = 0;
}
 
double frontNode(struct Queue *q) {
	if(q->nodesize == 0)
        return -1.0;
	return q->frontNode->data;
}
 
int pop(struct Queue *q) {
	if(q->nodesize == 0)
        return -1;
	q->nodesize--;
 
	struct Node *tmp = q->frontNode;
	q->frontNode = q->frontNode->nextnode;
	free(tmp);
}
 
void push(struct Queue *q, double data) {
	q->nodesize++;
 
	if (q->frontNode == NULL) {
		q->frontNode = (struct Node *) malloc(sizeof(struct Node));
		q->frontNode->data = data;
		q->frontNode->nextnode = NULL;
		q->lastNode = q->frontNode;
	} else {
		q->lastNode->nextnode = (struct Node *) malloc(sizeof(struct Node));
		q->lastNode->nextnode->data = data;
		q->lastNode->nextnode->nextnode = NULL;
		q->lastNode = q->lastNode->nextnode;
	}
}
