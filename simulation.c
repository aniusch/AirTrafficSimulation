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
void averageTime(Queue* queue, int total) {
    float tempoMedio = 0.0;
    Plane* plane = queue->first;
    while (plane != NULL) {
        tempoMedio += plane->waitTime;
        plane = plane->next;
    }
    int tempoMedioMin = tempoMedio / total;
    printf("Tempo medio de espera aproximadamente: %d minutos\n", tempoMedioMin + 1);
}

void previsao(Plane* plane, int time, int n){
    if(plane->waitTime > 1){
        int previsao = (time + plane->waitTime);
        if(previsao > n){
            printf("Aterrissagem: Origem: %s, Previsao: %02d:%02d, Situacao: Atrasado, Nova Previsao: Apos %02d:%02d\n", airports[plane->origDest], (time - 1) / 4, ((time - 1) % 4) * 15, (n - 1) / 4, (((n - 1) % 4) * 15) - 1);
        }
        else if (plane->type == DECOLAGEM){
            printf("Decolagem: Destino: %s, Previsao: %02d:%02d, Situacao: Atrasado, Nova Previsao: %02d:%02d\n", airports[plane->origDest], (time - 1) / 4, ((time - 1) % 4) * 15, (previsao - 1) / 4, ((previsao - 1) % 4) * 15);
        }
        else{
            printf("Aterrissagem: Origem: %s, Previsao: %02d:%02d, Situacao: Atrasado, Nova Previsao: %02d:%02d\n", airports[plane->origDest], (time - 1) / 4, ((time - 1) % 4) * 15, (previsao - 1) / 4, ((previsao - 1) % 4) * 15);
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
                printf("Acidente! Aviao EMERGENCIAL com identificador %d caiu por falta de combustivel.\n", plane->id);
        }
        plane = plane->next;
    }
}

// Atualizar o tempo de espera das aeronaves
void updateWaitingTime(Queue *takeoffQueue, Queue *landingQueue, Queue *emergencyQueue, Control *control, int time, int n){
    Plane* plane = landingQueue->first;
    while (plane != NULL) {
        plane->waitTime++;
        plane = plane->next;
    }
    plane = takeoffQueue->first;
    while (plane != NULL) {
        plane->waitTime++;
        plane = plane->next;

    }
    plane = emergencyQueue->first;
    while (plane != NULL) {
        plane->waitTime++;
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
            printf("Pedido: Decolagem: Destino: %s, Horario: %02d:%02d, Situacao: Previsto\n", airports[plane->origDest], (time + 1) / 4, ((time + 1) % 4) * 15);
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
            if(plane->fuel > alpha){
                enqueue(landingQueue, plane);
                printf("Pedido: Aterrissagem: Origem: %s, Horario: %02d:%02d, Situacao: Previsto\n", airports[plane->origDest], (time + 1) / 4, ((time + 1) % 4) * 15);
            }
            else{
                enqueue(emergencyQueue, plane);
                printf("Pedido: Aterrissagem(EMERGENCIAL): Origem: %s, Horario: %02d:%02d, Situacao: Previsto\n", airports[plane->origDest], (time + 1) / 4, ((time + 1) % 4) * 15);
            }
            control->landingRequests++;
        }
}

//simula a decolagem / pouso de emergenciais na pista 3
void lane3(Control *control, Queue *takeoffQueue, Queue *landingQueue, Queue *emergencyQueue, Lane lanes[3],int alpha, int time){
    Plane* plane = NULL;
    if(emergencyQueue->first != NULL && lanes[2].busy == FALSE){
        plane = dequeue(emergencyQueue);
        if (plane->fuel >= 0){
            control->landings++;
            if(plane->waitTime > 1){
                control->late++;
            }
            if(plane->fuel == 0){
                control->landingsWithZeroFuel++;
            }
            lanes[2].busy = TRUE;
            plane->isLanded = TRUE;
            printf("Aterrissagem (Pista %d): ID: %d, Origem: %s, Horario: %02d:%02d, Situacao: Confirmado\n", 3, plane->id, airports[plane->origDest], time / 4, (time  % 4) * 15);
        }
        else if (plane->fuel < 0 && emergencyQueue->first != NULL){
            while(emergencyQueue->first != NULL){
                plane = dequeue(emergencyQueue);
                if (plane->fuel >= 0){
                    control->landings++;
                    if(plane->waitTime > 1){
                        control->late++;
                    }
                    if(plane->fuel == 0){
                        control->landingsWithZeroFuel++;
                    }
                    lanes[2].busy = TRUE;
                    plane->isLanded = TRUE;
                    printf("Aterrissagem (Pista %d): ID: %d, Origem: %s, Horario: %02d:%02d, Situacao: Confirmado\n", 3, plane->id, airports[plane->origDest], time / 4, (time  % 4) * 15);
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
        if (plane->waitTime > 1){
            control->late++;
        }
        printf("Decolagem (Pista %d): ID: %d, Destino: %s, Horario: %02d:%02d, Situacao: Confirmado\n", 3, plane->id, airports[plane->origDest], time / 4, (time  % 4) * 15);
    }
    lanes[2].busy = FALSE;
}

//simula pouso nas pistas 1 e 2
void lane1and2(Control *control, Queue *takeoffQueue, Queue *landingQueue, Queue *emergencyQueue, Lane lanes[3],int alpha, int time){
    Plane *plane = NULL;
    for(int i = 0; i < 2; i++){
        if (emergencyQueue->first != NULL && lanes[i].busy == FALSE){
            plane = dequeue(emergencyQueue);
            if (plane->fuel >= 0){
                control->landings++;
                if(plane->waitTime > 1){
                    control->late++;
                }
                if(plane->fuel == 0){
                    control->landingsWithZeroFuel++;
                }
                lanes[2].busy = TRUE;
                plane->isLanded = TRUE;
                printf("Aterrissagem EMERGENCIAL (Pista %d): ID: %d, Origem: %s, Horario: %02d:%02d, Situacao: Confirmado\n", 3, plane->id, airports[plane->origDest], time / 4, (time  % 4) * 15);
            }
            else if (plane->fuel < 0 && emergencyQueue->first != NULL){
                while(emergencyQueue->first != NULL){
                    plane = dequeue(emergencyQueue);
                    if (plane->fuel >= 0){
                        control->landings++;
                        if(plane->waitTime > 1){
                            control->late++;
                        }
                        if(plane->fuel == 0){
                            control->landingsWithZeroFuel++;
                        }
                        lanes[2].busy = TRUE;
                        plane->isLanded = TRUE;
                        printf("Aterrissagem EMERGENCIAL (Pista %d): ID: %d, Origem: %s, Horario: %02d:%02d, Situacao: Confirmado\n", 3, plane->id, airports[plane->origDest], time / 4, (time  % 4) * 15);
                        break;
                    }
                }
            }
        }
        if(landingQueue->first != NULL && lanes[i].busy == FALSE){
            plane = dequeue(landingQueue);
            if (plane->fuel >= 0){
                control->landings++;
                if(plane->waitTime > 1){
                    control->late++;
                }
                if(plane->fuel == 0){
                    control->landingsWithZeroFuel++;
                }
                lanes[2].busy = TRUE;
                plane->isLanded = TRUE;
                printf("Aterrissagem (Pista %d): ID: %d, Origem: %s, Horario: %02d:%02d, Situacao: Confirmado\n", i + 1, plane->id, airports[plane->origDest], time / 4, (time  % 4) * 15);
            }
            else if (plane->fuel < 0 && landingQueue->first != NULL){
                while(landingQueue->first != NULL){
                    plane = dequeue(landingQueue);
                    if (plane->fuel >= 0){
                        control->landings++;
                        if(plane->waitTime > 1){
                            control->late++;
                        }
                        if(plane->fuel == 0){
                            control->landingsWithZeroFuel++;
                        }
                        lanes[2].busy = TRUE;
                        plane->isLanded = TRUE;
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

    
    
    
    while(time != n){
        printf("Horario atual: %02d:%02d\n", (time) / 4, ((time) % 4) * 15);

        // Gerar solicitações de decolagem e aterrissagem
        requests(&control, &takeoffQueue, &landingQueue, &emergencyQueue, ALPHA, time);

        //solicitar decolagem / pousar emergencial;
        lane3(&control, &takeoffQueue, &landingQueue, &emergencyQueue, lanes, ALPHA, time);

        //solicitar aterrissagem
        lane1and2(&control, &takeoffQueue, &landingQueue, &emergencyQueue, lanes, ALPHA, time);

        //incrementa tempo em uma unidade
        time++;

        //combustivel eh decrementado em uma unidade
        updateFuel(&takeoffQueue, &landingQueue, &emergencyQueue, &control);

        printf("\n");
    }

    // Exibir informações de controle
    printf("Total de decolagens: %d\n", control.takeoffs);
    printf("Total de aterrissagens: %d\n", control.landings);
    printf("Total de aterrissagens sem reserva: %d\n", control.landingsWithZeroFuel);
    printf("Total de atrasos: %d\n", control.late);
    printf("Total de acidentes: %d\n", control.accidents);
    averageTime(&taxiQueue, control.landings);
    averageTime(&tookOffQueue, control.takeoffs);

    freeQueue(&takeoffQueue);
    freeQueue(&landingQueue);
    freeQueue(&emergencyQueue);
}