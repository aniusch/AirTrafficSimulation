#include "funcoes.h"
#include "tad.h"
#include <stdio.h>
#include <stdlib.h>

// Insere um aviao no final da fila
void enqueue(Queue* queue, Plane* newPlane) {
    if (queue->last != NULL) {
        queue->last->next = newPlane;
    }
    else {
        queue->first = newPlane;
    }
    queue->last = newPlane;
    newPlane->next = NULL;
}

// Remove e retorna o aviao do inicio da fila
Plane* dequeue(Queue* queue) {
    Plane* plane = queue->first;
    if (plane != NULL) {
        queue->first = plane->next;
        if (queue->first == NULL) {
            queue->last = NULL;
        }
    }
    return plane;
}

void freeQueue(Queue* queue){
    Plane* plane = queue->first;
    while (plane != NULL) {
        Plane* nextPlane = plane->next;
        free(plane);
        plane = nextPlane;
    }
    queue->first = NULL;
    queue->last = NULL;
}