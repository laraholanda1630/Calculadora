#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include <math.h>
#include "expressao.h"

/* ---------- helpers de tokenização e identificação ---------- */

static int eh_numero(const char *t) {
    if (!t || *t == '\0') return 0;
    const char *p = t;
    if (*p=='+'||*p=='-') p++;
    int dig=0, dot=0;
    while (*p) {
        if (*p=='.' || *p==',') {
            if (dot) return 0;
            dot = 1;
        } else if (isdigit((unsigned char)*p)) {
            dig = 1;
        } else return 0;
        p++;
    }
    return dig;
}

static double tok_to_double(const char *t) {
    char tmp[64]; size_t j=0;
    for (size_t i=0; t[i] && j+1<sizeof(tmp); ++i)
        tmp[j++] = (t[i]==',') ? '.' : t[i];
    tmp[j]='\0';
    return atof(tmp);
}

static int eh_binario(const char *t) {
    return t && (
        strcmp(t,"+")==0 ||
        strcmp(t,"-")==0 ||
        strcmp(t,"*")==0 ||
        strcmp(t,"/")==0 ||
        strcmp(t,"%")==0 ||
        strcmp(t,"^")==0
    );
}

static int eh_unario(const char *t) {
    return t && (
        strcmp(t,"raiz")==0 ||
        strcmp(t,"sen")==0  ||
        strcmp(t,"cos")==0  ||
        strcmp(t,"tg")==0   ||
        strcmp(t,"log")==0
    );
}

/* ---------- pilha de strings ---------- */

typedef struct NoS {
    char *s;
    struct NoS *prox;
} NoS;

static void pushS(NoS **top, char *s) {
    NoS *n = (NoS*) malloc(sizeof(NoS));
    n->s = s;
    n->prox = *top;
    *top = n;
}

static char *popS(NoS **top) {
    if (*top == NULL) return NULL;
    NoS *t = *top;
    char *s = t->s;
    *top = t->prox;
    free(t);
    return s;
}

static void freePilhaS(NoS **top) {
    while (*top) {
        NoS *t = *top;
        *top = t->prox;
        free(t->s);
        free(t);
    }
}

/* ---------- pilha de doubles para avaliação ---------- */

typedef struct NoF {
    double v;
    struct NoF *prox;
} NoF;

static void pushF(NoF **top, double x) {
    NoF *n = (NoF*) malloc(sizeof(NoF));
    n->v = x;
    n->prox = *top;
    *top = n;
}

static double popF(NoF **top) {
    if (*top == NULL) return 0.0;
    NoF *t = *top;
    double v = t->v;
    *top = t->prox;
    free(t);
    return v;
}

static int emptyF(NoF *top) { return top == NULL; }

static void freePilhaF(NoF **top) {
    while (*top) popF(top);
}

/* ---------- getValorPosFixa ---------- */

float getValorPosFixa(char *StrPosFixa) {
    if (!StrPosFixa) return 0;

    char buf[1024];
    strncpy(buf, StrPosFixa, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';

    NoF *pilha = NULL;
    char *tok = strtok(buf, " ");

    while (tok) {

        if (eh_numero(tok)) {
            pushF(&pilha, tok_to_double(tok));
        }
        else if (eh_binario(tok)) {
            if (emptyF(pilha)) { freePilhaF(&pilha); return 0; }
            double b = popF(&pilha);

            if (emptyF(pilha)) { freePilhaF(&pilha); return 0; }
            double a = popF(&pilha);

            if (strcmp(tok,"+")==0) pushF(&pilha, a+b);
            else if (strcmp(tok,"-")==0) pushF(&pilha, a-b);
            else if (strcmp(tok,"*")==0) pushF(&pilha, a*b);
            else if (strcmp(tok,"/")==0) pushF(&pilha, a/b);
            else if (strcmp(tok,"%")==0) pushF(&pilha, fmod(a,b));
            else if (strcmp(tok,"^")==0) pushF(&pilha, pow(a,b));
        }
        else if (eh_unario(tok)) {
            if (emptyF(pilha)) { freePilhaF(&pilha); return 0; }
            double a = popF(&pilha);

            if (strcmp(tok,"raiz")==0) pushF(&pilha, sqrt(a));
            else if (strcmp(tok,"sen")==0) pushF(&pilha, sin(a * M_PI / 180.0));
            else if (strcmp(tok,"cos")==0) pushF(&pilha, cos(a * M_PI / 180.0));
            else if (strcmp(tok,"tg")==0)  pushF(&pilha, tan(a * M_PI / 180.0));
            else if (strcmp(tok,"log")==0) pushF(&pilha, log10(a));
        }
        else {
            freePilhaF(&pilha);
            return 0;
        }

        tok = strtok(NULL, " ");
    }

    if (pilha == NULL) return 0;
    double r = popF(&pilha);
    freePilhaF(&pilha);
    return (float)r;
}

/* ---------- getFormaInFixa ------------- */

char *getFormaInFixa(char *Str) {
    if (!Str) return NULL;

    char buf[1024];
    strncpy(buf, Str, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';

    NoS *stack = NULL;
    char *tok = strtok(buf, " ");

    while (tok) {

        if (eh_numero(tok)) {
            char *s = strdup(tok);
            pushS(&stack, s);
        }

        else if (eh_unario(tok)) {
            char *a = popS(&stack);
            if (!a) { freePilhaS(&stack); return NULL; }

            size_t L = strlen(tok) + strlen(a) + 3;
            char *out = malloc(L);
            snprintf(out, L, "%s(%s)", tok, a);

            free(a);
            pushS(&stack, out);
        }

        else if (eh_binario(tok)) {
            char *b = popS(&stack);
            char *a = popS(&stack);
            if (!a || !b) {
                free(a); free(b);
                freePilhaS(&stack);
                return NULL;
            }

            size_t L = strlen(a) + strlen(b) + strlen(tok) + 4;
            char *out = malloc(L);
            snprintf(out, L, "(%s%s%s)", a, tok, b);

            free(a); free(b);
            pushS(&stack, out);
        }

        else {
            freePilhaS(&stack);
            return NULL;
        }

        tok = strtok(NULL, " ");
    }

    char *res = popS(&stack);

    if (stack != NULL) {
        freePilhaS(&stack);
        free(res);
        return NULL;
    }

    return res;
}
