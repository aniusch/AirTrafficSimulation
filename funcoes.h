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
void previsao(Plane* plane, int time, int n);	
void updateFuel(Queue *takeoffQueue, Queue *landingQueue, Queue *emergencyQueue, Control *control);
void updateWaitingTime(Queue *takeoffQueue, Queue *landingQueue, Queue *emergencyQueue, Control *control, int time, int n);
void averageTime(Queue* queue, int total);
void requests(Control *control, Queue *takeoffQueue, Queue *landingQueue, Queue *emergencyQueue, int alpha, int time);
void lane1and2(Control *control, Queue *takeoffQueue, Queue *landingQueue, Queue *emergencyQueue, Lane lanes[3],int alpha, int time);
void lane3(Control *control, Queue *takeoffQueue, Queue *landingQueue, Queue *emergencyQueue, Lane lanes[3],int alpha, int time);

#endif /* FUNCOES_H */