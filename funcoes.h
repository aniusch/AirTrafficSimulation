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
void prediction(Plane* plane, int time);	
void updateFuel(Queue *takeoffQueue, Queue *landingQueue, Queue *emergencyQueue, Control *control);
void updateWaitingTime(Queue *takeoffQueue, Queue *landingQueue, Queue *emergencyQueue, Control *control, int time);
void averageTime(float *sum, int total);
void requests(Control *control, Queue *takeoffQueue, Queue *landingQueue, Queue *emergencyQueue, int alpha, int time);
void lane1and2(Control *control, Queue *takeoffQueue, Queue *landingQueue, Queue *emergencyQueue, Lane lanes[3],int alpha, int time, float *waitTime);
void lane3(Control *control, Queue *takeoffQueue, Queue *landingQueue, Queue *emergencyQueue, Lane lanes[3],int alpha, int time, float *waitTime);

#endif /* FUNCOES_H */