#include "funcoes.h"
#include "tad.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char airports[11][10] = { "RON", "SIN", "ALF", "LRV", "BRS", "BH", "SP", "RJ", "SCLS", "BA", "ASS" };

// Gera um número aleatório entre min e max
int genRandom(int max, int min) {
    return ((rand() % (max - min + 1)) + min);
}

// Retorna um aeroporto aleatório
Airport getRandomAirport() {
    return genRandom(ASS, RON);
}

// Calculo do tempo medio de espera
void averageTime(float *sum, int total) {
    float aux = *sum;
    int averageInMinutes = (int)(aux / total * 15);
    int hours = averageInMinutes / 60;
    int minutes = averageInMinutes % 60;
    printf("%02dh:%02dmin\n", hours, minutes);
}

void previsao(Plane* plane, int time){
    if(plane->waitTime > 1){
        if (plane->type == DECOLAGEM){
            printf("Decolagem: ID: %d, Destino: %s, Previsao: %02d:%02d, Situacao: Atrasado, Nova Previsao: %02d:%02d\n", plane->id, airports[plane->origDest], (time) / 4, ((time) % 4) * 15, (time + 1) / 4, ((time + 1) % 4) * 15);
        }
        else{
            printf("Aterrissagem: ID: %d, Origem: %s, Previsao: %02d:%02d, Situacao: Atrasado, Nova Previsao: %02d:%02d\n", plane->id, airports[plane->origDest], (time) / 4, ((time) % 4) * 15, (time + 1) / 4, ((time + 1) % 4) * 15);
        }
    }
}

//Atualizar o combustivel
void updateFuel(Queue *takeoffQueue, Queue *landingQueue, Queue *emergencyQueue, Control *control){
    Plane* plane = takeoffQueue->first;
    while (plane != NULL) {
        plane->fuel--;
        if (plane->fuel < 0 && plane->isLanded == FALSE) {
                control->accidents++; // Incrementa o contador de acidentes
                printf("Acidente! Aviao com identificador %d caiu por falta de combustivel.\n", plane->id);
        }        
        plane = plane->next;
    }
    plane = landingQueue->first;
    while (plane != NULL){
        plane->fuel--;
        if (plane->fuel < 0 && plane->isLanded == FALSE) {
                control->accidents++; // Incrementa o contador de acidentes
                printf("Acidente! Aviao com identificador %d sofreu um acidente por falta de combustivel.\n", plane->id);
        }
        plane = plane->next;
    }

    plane = emergencyQueue->first;
    while (plane != NULL) {
        plane->fuel--;
        if (plane->fuel < 0 && plane->isLanded == FALSE) {
                control->accidents++; // Incrementa o contador de acidentes
                printf("Acidente! Aviao (EMERGENCIAL) com identificador %d caiu por falta de combustivel.\n", plane->id);
        }
        plane = plane->next;
    }
}

// Atualizar o tempo de espera das aeronaves
void updateWaitingTime(Queue *takeoffQueue, Queue *landingQueue, Queue *emergencyQueue, Control *control, int time){
    Plane* plane = landingQueue->first;
    while (plane != NULL) {
        plane->waitTime++;
        if(plane->waitTime > 1){
            if(plane->isLate == FALSE){
                plane->isLate = TRUE;
                control->late++;
            }
            previsao(plane, time);
        }
        plane = plane->next;
    }
    plane = takeoffQueue->first;
    while (plane != NULL) {
        plane->waitTime++;
        if(plane->waitTime > 1){
            if(plane->isLate == FALSE){
                plane->isLate = TRUE;
                control->late++;
            }
            previsao(plane, time);
        }
        plane = plane->next;

    }
    plane = emergencyQueue->first;
    while (plane != NULL) {
        plane->waitTime++;
        if(plane->waitTime > 1){
            if(plane->isLate == FALSE){
                plane->isLate = TRUE;
                control->late++;
            }
            previsao(plane, time);
        }
        plane = plane->next;
    }
}

void requests(Control *control, Queue *takeoffQueue, Queue *landingQueue, Queue *emergencyQueue, int alpha, int time){
        // Gerar solicitações de decolagem
        int takeoffRequests = genRandom(3, 0);
        for (int i = 0; i < takeoffRequests; i++) {
            Plane* plane = (Plane*)malloc(sizeof(Plane));
            plane->id = (control->takeoffsRequests % 2 == 0) ? control->takeoffsRequests + 1 : control->takeoffsRequests;
            plane->fuel = genRandom(8, 1);
            plane->origDest = getRandomAirport();
            plane->isLanded = TRUE;
            plane->type = DECOLAGEM;  // Decolagem
            plane->waitTime = 0;
            plane->isLate = FALSE;
            printf("Decolagem: Destino: %s, Horario: %02d:%02d, Situacao: Previsto\n", airports[plane->origDest], (time + 1) / 4, ((time + 1) % 4) * 15);
            control->takeoffsRequests++;
            enqueue(takeoffQueue, plane);
        }
        // Gerar solicitações de aterrissagem
        int landingRequests = genRandom(4, 0);
        for (int i = 0; i < landingRequests; i++){
            Plane* plane = (Plane*)malloc(sizeof(Plane));
            plane->id = (control->landingRequests % 2 == 0) ? control->landingRequests : control->landingRequests + 1;
            plane->fuel = genRandom(8, 1);
            plane->origDest = getRandomAirport();
            plane->isLanded = FALSE;
            plane->type = ATERRISSAGEM;  // Aterrissagem
            plane->waitTime = 0;
            plane->isLate = FALSE;
            if(plane->fuel > alpha){
                enqueue(landingQueue, plane);
                printf("Aterrissagem: Origem: %s, Horario: %02d:%02d, Situacao: Previsto\n", airports[plane->origDest], (time + 1) / 4, ((time + 1) % 4) * 15);
            }
            else{
                enqueue(emergencyQueue, plane);
                printf("Aterrissagem(EMERGENCIAL): Origem: %s, Horario: %02d:%02d, Situacao: Previsto\n", airports[plane->origDest], (time + 1) / 4, ((time + 1) % 4) * 15);
            }
            control->landingRequests++;
        }
}

//simula a decolagem / pouso de emergenciais na pista 3
void lane3(Control *control, Queue *takeoffQueue, Queue *landingQueue, Queue *emergencyQueue, Lane lanes[3],int alpha, int time, float *waitTime){
    Plane* plane = NULL;
    if(emergencyQueue->first != NULL && lanes[2].busy == FALSE){
        plane = dequeue(emergencyQueue);
        if (plane->fuel >= 0){
            control->landings++;
            if(plane->fuel == 0){
                control->landingsWithZeroFuel++;
            }
            lanes[2].busy = TRUE;
            plane->isLanded = TRUE;
            *waitTime += plane->waitTime;
            printf("Aterrissagem (Pista %d): ID: %d, Origem: %s, Horario: %02d:%02d, Situacao: Confirmado\n", 3, plane->id, airports[plane->origDest], time / 4, (time  % 4) * 15);
        }
        else if (plane->fuel < 0 && emergencyQueue->first != NULL){
            while(emergencyQueue->first != NULL && lanes[2].busy == FALSE){
                plane = dequeue(emergencyQueue);
                if (plane->fuel >= 0){
                    control->landings++;
                    if(plane->fuel == 0){
                        control->landingsWithZeroFuel++;
                    }
                    lanes[2].busy = TRUE;
                    plane->isLanded = TRUE;
                    *waitTime += plane->waitTime;
                    printf("Aterrissagem(EMERGENCIAL)(Pista %d): ID: %d, Origem: %s, Horario: %02d:%02d, Situacao: Confirmado\n", 3, plane->id, airports[plane->origDest], time / 4, (time  % 4) * 15);
                    break;
                }
            }
        }
    }
    if (takeoffQueue->first != NULL && lanes[2].busy == FALSE){
        plane = dequeue(takeoffQueue);
        lanes[2].busy = TRUE;
        plane->isLanded = FALSE;
        control->takeoffs++;
        *waitTime += plane->waitTime;
        printf("Decolagem (Pista %d): ID: %d, Destino: %s, Horario: %02d:%02d, Situacao: Confirmado\n", 3, plane->id, airports[plane->origDest], time / 4, (time  % 4) * 15);
    }
    lanes[2].busy = FALSE;
}

//simula pouso nas pistas 1 e 2
void lane1and2(Control *control, Queue *takeoffQueue, Queue *landingQueue, Queue *emergencyQueue, Lane lanes[3],int alpha, int time, float *waitTime){
    Plane *plane = NULL;
    for(int i = 0; i < 2; i++){
        if (emergencyQueue->first != NULL && lanes[i].busy == FALSE){
            plane = dequeue(emergencyQueue);
            if (plane->fuel >= 0){
                control->landings++;
                if(plane->fuel == 0){
                    control->landingsWithZeroFuel++;
                }
                lanes[2].busy = TRUE;
                plane->isLanded = TRUE;
                *waitTime += plane->waitTime;
                printf("Aterrissagem(EMERGENCIAL) (Pista %d): ID: %d, Origem: %s, Horario: %02d:%02d, Situacao: Confirmado\n", 3, plane->id, airports[plane->origDest], time / 4, (time  % 4) * 15);
            }
            else if (plane->fuel < 0 && emergencyQueue->first != NULL){
                while(emergencyQueue->first != NULL){
                    plane = dequeue(emergencyQueue);
                    if (plane->fuel >= 0){
                        control->landings++;
                        if(plane->fuel == 0){
                            control->landingsWithZeroFuel++;
                        }
                        lanes[2].busy = TRUE;
                        plane->isLanded = TRUE;
                        *waitTime += plane->waitTime;
                        printf("Aterrissagem(EMERGENCIAL) (Pista %d): ID: %d, Origem: %s, Horario: %02d:%02d, Situacao: Confirmado\n", 3, plane->id, airports[plane->origDest], time / 4, (time  % 4) * 15);
                        break;
                    }
                }
            }
        }
        if(landingQueue->first != NULL && lanes[i].busy == FALSE){
            plane = dequeue(landingQueue);
            if (plane->fuel >= 0){
                control->landings++;
                if(plane->fuel == 0){
                    control->landingsWithZeroFuel++;
                }
                lanes[2].busy = TRUE;
                plane->isLanded = TRUE;
                *waitTime += plane->waitTime;
                printf("Aterrissagem (Pista %d): ID: %d, Origem: %s, Horario: %02d:%02d, Situacao: Confirmado\n", i + 1, plane->id, airports[plane->origDest], time / 4, (time  % 4) * 15);
            }
            else if (plane->fuel < 0 && landingQueue->first != NULL){
                while(landingQueue->first != NULL){
                    plane = dequeue(landingQueue);
                    if (plane->fuel >= 0){
                        control->landings++;
                        if(plane->fuel == 0){
                            control->landingsWithZeroFuel++;
                        }
                        lanes[2].busy = TRUE;
                        plane->isLanded = TRUE;
                        *waitTime += plane->waitTime;
                        printf("Aterrissagem (Pista %d): ID: %d, Origem: %s, Horario: %02d:%02d, Situacao: Confirmado\n", i + 1, plane->id, airports[plane->origDest], time / 4, (time  % 4) * 15);
                        break;
                    }
                }
            }
        }  
    }
    lanes[0].busy = FALSE;
    lanes[1].busy = FALSE;
}

// Simula o controle de tráfego aéreo
void simulateAirTrafficControl(int n, int alpha){
    Queue takeoffQueue;
    takeoffQueue.first = NULL;
    takeoffQueue.last = NULL;

    Queue landingQueue;
    landingQueue.first = NULL;
    landingQueue.last = NULL;

    Queue emergencyQueue;
    emergencyQueue.first = NULL;
    emergencyQueue.last = NULL;

    Queue taxiQueue; // fila para avioes que ja pousaram
    taxiQueue.first = NULL;
    taxiQueue.last = NULL;

    Queue tookOffQueue; // fila para avioes que ja decolaram
    tookOffQueue.first = NULL;
    tookOffQueue.last = NULL;

    Lane lanes[3];

    Control control;
    control.landings = 0;
    control.landingRequests = 0;
    control.takeoffs = 0;
    control.takeoffsRequests = 0;
    control.late = 0;
    control.accidents = 0;
    control.landingsWithZeroFuel = 0;
 
    int time = 0;
    float sumWaitingLanding = 0.0; // soma dos tempos de espera para pousar
    float sumWaitingTakeoff = 0.0; // soma dos tempos de espera para decolar

    while(time != n){
        printf("Horario atual: %02d:%02d\n", (time) / 4, ((time) % 4) * 15);

        // Gerar solicitações de decolagem e aterrissagem
        requests(&control, &takeoffQueue, &landingQueue, &emergencyQueue, ALPHA, time);

        //solicitar decolagem / pousar emergencial;
        lane3(&control, &takeoffQueue, &landingQueue, &emergencyQueue, lanes, ALPHA, time, &sumWaitingLanding);

        //solicitar aterrissagem
        lane1and2(&control, &takeoffQueue, &landingQueue, &emergencyQueue, lanes, ALPHA, time, &sumWaitingTakeoff);

        //incrementa tempo em uma unidade
        time++;

        //combustivel eh decrementado em uma unidade
        updateFuel(&takeoffQueue, &landingQueue, &emergencyQueue, &control);
        updateWaitingTime(&takeoffQueue, &landingQueue, &emergencyQueue, &control, time);

        printf("\n");
    }

    printf("Encerra recebimento de aeronaves\n");
    printf("\n");

    while(emergencyQueue.first != NULL || landingQueue.first != NULL || takeoffQueue.first != NULL){
        printf("Horario atual: %02d:%02d\n", (time) / 4, ((time) % 4) * 15);
       
        //solicitar decolagem / pousar emergencial;
        lane3(&control, &takeoffQueue, &landingQueue, &emergencyQueue, lanes, ALPHA, time, &sumWaitingTakeoff);

        //solicitar aterrissagem
        lane1and2(&control, &takeoffQueue, &landingQueue, &emergencyQueue, lanes, ALPHA, time, &sumWaitingLanding);

        //incrementa tempo em uma unidade
        time++;
        if (time > MAX){
            time = 0;
        }
        //combustivel eh decrementado em uma unidade
        updateFuel(&takeoffQueue, &landingQueue, &emergencyQueue, &control);
        updateWaitingTime(&takeoffQueue, &landingQueue, &emergencyQueue, &control, time);

        printf("\n");
    }

    // Exibir informações de controle
    printf("Total de decolagens: %d\n", control.takeoffs);
    printf("Total de aterrissagens: %d\n", control.landings);
    printf("Total de aterrissagens sem reserva: %d\n", control.landingsWithZeroFuel);
    printf("Total de atrasos: %d\n", control.late);
    printf("Total de acidentes: %d\n", control.accidents);
    averageTime(&sumWaitingLanding, control.landings);
    averageTime(&sumWaitingTakeoff, control.takeoffs);

    freeQueue(&takeoffQueue);
    freeQueue(&landingQueue);
    freeQueue(&emergencyQueue);
}