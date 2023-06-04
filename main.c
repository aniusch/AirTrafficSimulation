#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "funcoes.h"
#include "tad.h"

int main(int argc, char* argv[]){
    int n;
    n = atoi(argv[1]);
    srand(time(NULL)); // Inicializa a semente para geração de números aleatórios
    if(n > MAX){
        printf("O valor de n deve ser menor ou igual a %d\n", MAX);
        return 1;
    }
    simulateAirTrafficControl(n, ALPHA);

    return 0;
}