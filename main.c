#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expressao.h"

int main() {
    char expr[300];
    char *infixa;
    float valor;

    printf("Digite a expressao posfixa: ");
    fgets(expr, 300, stdin);

    // remove \n
    expr[strcspn(expr, "\n")] = 0;

    // forma infixa
    infixa = getFormaInFixa(expr);
    if (infixa == NULL) {
        printf("Erro na conversao para infixa.\n");
        return 0;
    }

    // valor da posfixa
    valor = getValorPosFixa(expr);

    printf("Forma Infixa : %s\n", infixa);
    printf("Valor        : %.6f\n", valor);

    free(infixa);
    return 0;
}
