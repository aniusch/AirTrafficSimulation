// funcoes.h
#include "tad.h"

#ifndef FUNCOES_H
#define FUNCOES_H

int genRandom(int max, int min);
Airport getRandomAirport();
void enqueue(Queue* queue, Plane* newPlane);
Plane* dequeue(Queue* queue);
void simulateAirTrafficControl(int n, int alpha);
void freeQueue(Queue* queue);

#endif /* FUNCOES_H */