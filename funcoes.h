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
void previsao(Plane* plane, int time);	
void updateFuel(Queue *takeoffQueue, Queue *landingQueue, Queue *emergencyQueue);
void updateWaitingTime(Queue *takeoffQueue, Queue *landingQueue, Queue *emergencyQueue, Control *control, int time);

#endif /* FUNCOES_H */