#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TRUE 1
#define FALSE 0
#define MAX 96
#define ALPHA 3

typedef enum { RON, SIN, ALF, LRV, BRS, BH, SP, RJ, SCLS, BA, ASS } Airport;
char airports[11][10] = { "RON", "SIN", "ALF", "LRV", "BRS", "BH", "SP", "RJ", "SCLS", "BA", "ASS" };

typedef struct SCTA {
    int landings;
    int takeoffs;
    int late;
    int lateTakeoffTime;
    int lateLandingTime;
    int accidents;
} Control;

typedef struct AIRPLANE {
    int id;
    int fuel;
    Airport origDest;
    int isLanded;
    int type;  // 0 para aterrissagem, 1 para decolagem
    struct AIRPLANE* next;
} Plane;

typedef struct LANE {
    int busy;
} Lane;

typedef struct QUEUE {
    Plane* first;
    Plane* last;
} Queue;

int genRandom(int max, int min);
Airport getRandomAirport();
void enqueue(Queue* queue, Plane* newPlane);
Plane* dequeue(Queue* queue);
void simulateAirTrafficControl(int n, int alpha);

// Gera um número aleatório entre min e max
int genRandom(int max, int min) {
    return ((rand() % (max - min + 1)) + min);
}

// Retorna um aeroporto aleatório
Airport getRandomAirport() {
    return genRandom(ASS, RON);
}

// Insere um avião no final da fila
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

// Remove e retorna o avião do início da fila
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

// Simula o controle de tráfego aéreo
void simulateAirTrafficControl(int n, int alpha) {
    Queue takeoffQueue;
    takeoffQueue.first = NULL;
    takeoffQueue.last = NULL;

    Queue landingQueue;
    landingQueue.first = NULL;
    landingQueue.last = NULL;

    Lane lanes[3];

    Control control;
    control.landings = 0;
    control.takeoffs = 0;
    control.late = 0;
    control.lateTakeoffTime = 0;
    control.lateLandingTime = 0;
    control.accidents = 0;

    for (int time = 1; time <= n; time++) {
        printf("Tempo: %02d:%02d\n", (time - 1) / 4, ((time - 1) % 4) * 15);

        // Gerar solicitações de decolagem
        int takeoffRequests = genRandom(3, 0);
        for (int i = 0; i < takeoffRequests; i++) {
            Plane* plane = (Plane*)malloc(sizeof(Plane));
            plane->id = (control.takeoffs % 2 == 0) ? control.takeoffs + 1 : control.takeoffs;
            plane->fuel = genRandom(8, 1);
            plane->origDest = getRandomAirport();
            plane->isLanded = FALSE;
            plane->type = 1;  // Decolagem
            enqueue(&takeoffQueue, plane);
            control.takeoffs++;
        }

        // Gerar solicitações de aterrissagem
        int landingRequests = genRandom(4, 0);
        for (int i = 0; i < landingRequests; i++) {
            Plane* plane = (Plane*)malloc(sizeof(Plane));
            plane->id = (control.landings % 2 == 0) ? control.landings : control.landings + 1;
            plane->fuel = genRandom(8, 1);
            plane->origDest = getRandomAirport();
            plane->isLanded = FALSE;
            plane->type = 0;  // Aterrissagem
            enqueue(&landingQueue, plane);
            control.landings++;
        }

        for (int i = 0; i < 3; i++) {
            if (landingQueue.first != NULL) {
                Plane* plane = dequeue(&landingQueue);
                if (plane->fuel <= 1) {
                    control.accidents++; // Incrementa o contador de acidentes
                    printf("Acidente! Avião com identificador %d caiu por falta de combustível.\n", plane->id);
                }
                else if (plane->fuel <= alpha || lanes[2].busy) {
                    lanes[i].busy = 1;
                    plane->isLanded = TRUE;
                    printf("Aterrissagem (Pista %d): Origem: %s, Horário: %02d:%02d, Situação: Confirmado\n", i + 1, airports[plane->origDest], (time - 1) / 4, ((time - 1) % 4) * 15);
                }
                else if (!lanes[i].busy && takeoffQueue.first != NULL) {
                    Plane* takeoffPlane = dequeue(&takeoffQueue);
                    lanes[i].busy = 1;
                    takeoffPlane->isLanded = TRUE;
                    printf("Decolagem (Pista %d): Destino: %s, Horário: %02d:%02d, Situação: Confirmado\n", i + 1, airports[takeoffPlane->origDest], (time - 1) / 4, ((time - 1) % 4) * 15);
                    enqueue(&takeoffQueue, takeoffPlane);
                }
                else {
                    control.lateLandingTime += time; // Atualiza o tempo total de espera para aterrissagem
                    enqueue(&landingQueue, plane);
                }
            }
        }

        // Atualizar o estado das pistas
        for (int i = 0; i < 3; i++) {
            if (lanes[i].busy) {
                lanes[i].busy = 0;
            }
        }

        // Atualizar o nível de combustível das aeronaves
        Plane* plane = takeoffQueue.first;
        while (plane != NULL) {
            plane->fuel--;
            plane = plane->next;
        }

        plane = landingQueue.first;
        while (plane != NULL) {
            plane->fuel--;
            plane = plane->next;
        }

        printf("\n");
    }

    // Exibir informações de controle
    printf("Total de decolagens: %d\n", control.takeoffs);
    printf("Total de aterrissagens: %d\n", control.landings);
    printf("Total de atrasos: %d\n", control.late);
    printf("Total de acidentes: %d\n", control.accidents);
    if (control.takeoffs > 0) {
        printf("Tempo médio de espera para decolagem: %.2f\n", (float)control.lateTakeoffTime / control.takeoffs);
    }
    if (control.landings > 0) {
        printf("Tempo médio de espera para aterrissagem: %.2f\n", (float)control.lateLandingTime / control.landings);
    }
}

int main(int argc, char* argv[]) {
    int n;
    n = atoi(argv[1]);
    srand(time(NULL)); // Inicializa a semente para geração de números aleatórios
    simulateAirTrafficControl(n, ALPHA);

    return 0;
}
