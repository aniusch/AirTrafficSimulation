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

void previsao(Plane* plane, int time){
    if(plane->waitTime > 1){
        int previsao = (time + plane->waitTime);
        if (plane->type == DECOLAGEM){
            printf("Decolagem: Destino: %s, Previsao: %02d:%02d, Situacao: Atrasado, Nova Previsao: %02d:%02d\n", airports[plane->origDest], (time - 1) / 4, ((time - 1) % 4) * 15, (previsao - 1) / 4, ((previsao - 1) % 4) * 15);
        }
        else{
            printf("Aterrissagem: Origem: %s, Previsao: %02d:%02d, Situacao: Atrasado, Nova Previsao: %02d:%02d\n", airports[plane->origDest], (time - 1) / 4, ((time - 1) % 4) * 15, (previsao - 1) / 4, ((previsao - 1) % 4) * 15);
        }
    }
}


//Atualizar o combustivel
void updateFuel(Queue *takeoffQueue, Queue *landingQueue, Queue *emergencyQueue){
    Plane* plane = takeoffQueue->first;
    while (plane != NULL) {
        plane->fuel--;
        plane = plane->next;
    }
    plane = landingQueue->first;
    while (plane != NULL) {
        plane->fuel--;
        plane = plane->next;
    }

    plane = emergencyQueue->first;
    while (plane != NULL) {
        plane->fuel--;
        plane = plane->next;
    }
}

// Atualizar o tempo de espera das aeronaves
void updateWaitingTime(Queue *takeoffQueue, Queue *landingQueue, Queue *emergencyQueue, Control *control, int time){
    Plane* plane = landingQueue->first;
    while (plane != NULL) {
        plane->waitTime++;
        previsao(plane, time);
        plane = plane->next;
    }
    plane = takeoffQueue->first;
    while (plane != NULL) {
        plane->waitTime++;
        previsao(plane, time);
        plane = plane->next;

    }
    plane = emergencyQueue->first;
    while (plane != NULL) {
        plane->waitTime++;
        previsao(plane, time);
        plane = plane->next;
    }
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

    Lane lanes[3];

    Control control;
    control.landings = 0;
    control.landingRequests = 0;
    control.takeoffs = 0;
    control.takeoffsRequests = 0;
    control.late = 0;
    control.accidents = 0;

    for (int time = 1; time <= n; time++){
        printf("Tempo: %02d:%02d\n", (time - 1) / 4, ((time - 1) % 4) * 15);
        // Gerar solicitações de decolagem
        int takeoffRequests = genRandom(3, 0);
        for (int i = 0; i < takeoffRequests; i++) {
            Plane* plane = (Plane*)malloc(sizeof(Plane));
            plane->id = (control.takeoffsRequests % 2 == 0) ? control.takeoffsRequests + 1 : control.takeoffsRequests;
            plane->fuel = genRandom(8, 1);
            plane->origDest = getRandomAirport();
            plane->isLanded = TRUE;
            plane->type = DECOLAGEM; 
            plane->waitTime = 0;
            control.takeoffsRequests++;
            enqueue(&takeoffQueue, plane);
        }
        // Gerar solicitações de aterrissagem
        int landingRequests = genRandom(4, 0);
        for (int i = 0; i < landingRequests; i++){
            Plane* plane = (Plane*)malloc(sizeof(Plane));
            plane->id = (control.landingRequests % 2 == 0) ? control.landingRequests : control.landingRequests + 1;
            plane->fuel = genRandom(8, 1);
            plane->origDest = getRandomAirport();
            plane->isLanded = FALSE;
            plane->type = ATERRISSAGEM; 
            plane->waitTime = 0;
            if(plane->fuel > alpha)
                enqueue(&landingQueue, plane);
            else
                enqueue(&emergencyQueue, plane);
            control.landingRequests++;
        }

        for (int i = 0; i < 2; i++){
            if (emergencyQueue.first != NULL){
                if(emergencyQueue.first->next != NULL){
                    Plane* plane = dequeue(&emergencyQueue);
                    if (plane->fuel < 1 && plane->isLanded == FALSE){
                        control.accidents++; // Incrementa o contador de acidentes
                        printf("Acidente! Aviao com identificador %d caiu por falta de combustivel.\n", plane->id);
                        if (emergencyQueue.first != NULL){
                            plane = dequeue(&emergencyQueue);
                            lanes[i].busy = 1;
                            plane->isLanded = TRUE;
                            if (plane->waitTime > 1){
                                control.late++;
                            }
                            control.landings++;
                            printf("Aterrissagem (Pista %d): Origem: %s, Horario: %02d:%02d, Situacao: Confirmado\n", i + 1, airports[plane->origDest], (time - 1) / 4, ((time - 1) % 4) * 15);        
                        } 
                    }
                    else{
                        lanes[i].busy = 1;
                        plane->isLanded = TRUE;
                        if (plane->waitTime > 1){
                            control.late++;
                        }
                        control.landings++;
                        printf("Aterrissagem (Pista %d): Origem: %s, Horario: %02d:%02d, Situacao: Confirmado\n", i + 1, airports[plane->origDest], (time - 1) / 4, ((time - 1) % 4) * 15);
                    }
                }    
            }
            else if(landingQueue.first != NULL){
                Plane* plane = dequeue(&landingQueue);
                if (plane->fuel < 1) {
                    control.accidents++; // Incrementa o contador de acidentes
                    printf("Acidente! Aviao com identificador %d caiu por falta de combustivel.\n", plane->id);
                    if (landingQueue.first != NULL){
                        plane = dequeue(&landingQueue);
                        lanes[i].busy = 1;
                        plane->isLanded = TRUE;
                        control.landings++;
                        if (plane->waitTime > 1){
                            control.late++;
                        }
                        printf("Aterrissagem (Pista %d): Origem: %s, Horario: %02d:%02d, Situacao: Confirmado\n", i + 1, airports[plane->origDest], (time - 1) / 4, ((time - 1) % 4) * 15);        
                    } 
                }
            }
            else if (takeoffQueue.first != NULL){
                Plane* takeoffPlane = dequeue(&takeoffQueue);
                lanes[i].busy = 1;
                takeoffPlane->isLanded = FALSE;
                control.takeoffs++;
                if (takeoffPlane->waitTime > 1){
                    control.late++;
                }
                printf("Decolagem (Pista %d): Destino: %s, Horario: %02d:%02d, Situacao: Confirmado\n", i + 1, airports[takeoffPlane->origDest], (time - 1) / 4, ((time - 1) % 4) * 15);                   
            }
        }

        if(emergencyQueue.first != NULL){
            Plane* plane = dequeue(&emergencyQueue);
            if (plane->fuel < 1) {
                control.accidents++; // Incrementa o contador de acidentes
                printf("Acidente! Aviao com identificador %d caiu por falta de combustivel.\n", plane->id);
                if(emergencyQueue.first != NULL){
                    plane = dequeue(&emergencyQueue);
                    lanes[2].busy = TRUE;
                    plane->isLanded = TRUE;
                    if (plane->waitTime > 1){
                        control.late++;
                    }
                    control.landings++;
                    printf("Aterrissagem (Pista %d): Origem: %s, Horario: %02d:%02d, Situacao: Confirmado\n", 3, airports[plane->origDest], (time - 1) / 4, ((time - 1) % 4) * 15);
                }
            }
            else{
                lanes[2].busy = TRUE;
                plane->isLanded = TRUE;
                if (plane->waitTime > 1){
                    control.late++;
                }
                control.landings++;
                printf("Aterrissagem (Pista %d): Origem: %s, Horario: %02d:%02d, Situacao: Confirmado\n", 3, airports[plane->origDest], (time - 1) / 4, ((time - 1) % 4) * 15);
            }
        }
        else if(takeoffQueue.first != NULL){
            Plane* takeoffPlane = dequeue(&takeoffQueue);
            lanes[2].busy = TRUE;
            takeoffPlane->isLanded = FALSE;
            control.takeoffs++;
            if (takeoffPlane->waitTime > 1){
                control.late++;
            }
            printf("Decolagem (Pista %d): Destino: %s, Horario: %02d:%02d, Situacao: Confirmado\n", 2, airports[takeoffPlane->origDest], (time - 1) / 4, ((time - 1) % 4) * 15);                   
        }
        else if (landingQueue.first != NULL){
            Plane* plane = dequeue(&landingQueue);
            if (plane->fuel < 1 && plane->isLanded == FALSE){
                control.accidents++; // Incrementa o contador de acidentes
                printf("Acidente! Aviao com identificador %d caiu por falta de combustivel.\n", plane->id);
                if (landingQueue.first != NULL){
                    plane = dequeue(&landingQueue);
                    lanes[2].busy = 1;
                    plane->isLanded = TRUE;
                    if (plane->waitTime > 1){
                        control.late++;
                    }
                    control.landings++;
                     printf("Aterrissagem (Pista %d): Origem: %s, Horario: %02d:%02d, Situacao: Confirmado\n", 2, airports[plane->origDest], (time - 1) / 4, ((time - 1) % 4) * 15);
                } 
            }
            else{
                lanes[2].busy = 1;
                plane->isLanded = TRUE;
                if (plane->waitTime > 1){
                    control.late++;
                }
                control.landings++;
                printf("Aterrissagem (Pista %d): Origem: %s, Horario: %02d:%02d, Situacao: Confirmado\n", 2, airports[plane->origDest], (time - 1) / 4, ((time - 1) % 4) * 15);
            }
        }

        // Atualizar o estado das pistas
        for (int i = 0; i < 3; i++) {
            if (lanes[i].busy) {
                lanes[i].busy = FALSE;
            }
        }

        updateFuel(&takeoffQueue, &landingQueue, &emergencyQueue);
        updateWaitingTime(&takeoffQueue, &landingQueue, &emergencyQueue, &control, time);

        printf("\n");
    }

    // Exibir informações de controle
    printf("Total de decolagens: %d\n", control.takeoffs);
    printf("Total de aterrissagens: %d\n", control.landings);
    printf("Total de atrasos: %d\n", control.late);
    printf("Total de acidentes: %d\n", control.accidents);

    freeQueue(&takeoffQueue);
    freeQueue(&landingQueue);
    freeQueue(&emergencyQueue);
    

}