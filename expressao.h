#ifndef EXPRESSAO_H
#define EXPRESSAO_H

typedef struct {
    char posFixa[512];     // Expressão na forma pos-fixa, como "3 12 4 + *"
    char inFixa[512];      // Expressão na forma infixa, como "3*(12+4)"
    float Valor;           // Valor numérico da expressão
} Expressao;

/* Retorna a forma infixa (alocada; free() pelo chamador) de Str (que está em posfixa).
   Retorna NULL em caso de erro. */
char * getFormaInFixa(char *Str);

/* Calcula o valor de Str (na forma posFixa). Str NÃO é modificada pela função. */
float getValorPosFixa(char *StrPosFixa);

#endif
