// TAD.h
#ifndef TAD_H
#define TAD_H

#define TRUE 1
#define FALSE 0
#define MAX 96
#define ALPHA 4

typedef enum { RON, SIN, ALF, LRV, BRS, BH, SP, RJ, SCLS, BA, ASS } Airport;

typedef struct SCTA {
    int landings;
    int landingRequests;
    int takeoffs;
    int takeoffsRequests;
    int late;
    int accidents;
} Control;

typedef struct AIRPLANE {
    int id;
    int fuel;
    Airport origDest;
    int isLanded;
    int type;  // 0 para aterrissagem, 1 para decolagem
    int waitTime; // tempo para aterrissagem decolagem
    struct AIRPLANE* next;
} Plane;

typedef struct LANE {
    int busy;
} Lane;

typedef struct QUEUE {
    Plane* first;
    Plane* last;
} Queue;

#endif /* TAD_H */