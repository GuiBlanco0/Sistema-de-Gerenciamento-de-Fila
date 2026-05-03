#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HISTORICO 10

typedef struct NoPaciente {
    char nome[50];
    struct NoPaciente *prox;
} NoPaciente;

typedef struct {
    NoPaciente *inicio;
    NoPaciente *fim;
    int tamanho;
} Fila;

void fila_inicializar(Fila *f) {
    f->inicio = NULL;
    f->fim    = NULL;
    f->tamanho = 0;
}

int fila_vazia(Fila *f) {
    return f->inicio == NULL;
}

int fila_peek(Fila *f, char *out) {
    if (fila_vazia(f)) return 0;
    strcpy(out, f->inicio->nome);
    return 1;
}

void enqueue(Fila *f, const char *nome) {
    NoPaciente *novo = malloc(sizeof(NoPaciente));
    strcpy(novo->nome, nome);
    novo->prox = NULL;

    if (f->fim == NULL) {
        f->inicio = f->fim = novo;
    } else {
        f->fim->prox = novo;
        f->fim = novo;
    }
    f->tamanho++;
}

int dequeue(Fila *f, char *out) {
    if (fila_vazia(f)) return 0;

    NoPaciente *tmp = f->inicio;
    strcpy(out, tmp->nome);
    f->inicio = tmp->prox;
    if (f->inicio == NULL) f->fim = NULL;
    free(tmp);
    f->tamanho--;
    return 1;
}

void fila_imprimir(Fila *f, const char *label) {
    printf("  [%s] ", label);
    if (fila_vazia(f)) {
        printf("(vazia)\n");
        return;
    }
    NoPaciente *atual = f->inicio;
    while (atual) {
        printf("%s", atual->nome);
        if (atual->prox) printf(" -> ");
        atual = atual->prox;
    }
    printf("\n");
}

typedef struct NoAcao {
    char descricao[100];
    struct NoAcao *prox;
} NoAcao;

typedef struct {
    NoAcao *topo;
    int tamanho;
} Pilha;

void pilha_inicializar(Pilha *p) {
    p->topo = NULL;
    p->tamanho = 0;
}

int pilha_vazia(Pilha *p) {
    return p->topo == NULL;
}

int pilha_peek(Pilha *p, char *out) {
    if (pilha_vazia(p)) return 0;
    strcpy(out, p->topo->descricao);
    return 1;
}

void push(Pilha *p, const char *descricao) {
    NoAcao *novo = malloc(sizeof(NoAcao));
    strcpy(novo->descricao, descricao);
    novo->prox = p->topo;
    p->topo = novo;
    p->tamanho++;
}

int pop(Pilha *p, char *out) {
    if (pilha_vazia(p)) return 0;

    NoAcao *tmp = p->topo;
    strcpy(out, tmp->descricao);
    p->topo = tmp->prox;
    free(tmp);
    p->tamanho--;
    return 1;
}

void pilha_imprimir(Pilha *p, const char *label) {
    printf("  [%s] (topo -> base)\n", label);
    if (pilha_vazia(p)) {
        printf("    (vazia)\n");
        return;
    }
    NoAcao *atual = p->topo;
    int i = 1;
    while (atual) {
        printf("    %d. %s\n", i++, atual->descricao);
        atual = atual->prox;
    }
}

void processar_atendimento(Fila *prioridade, Fila *normal,
                           Pilha *historico, int num_medico) {
    char paciente[50];
    char tipo[20];
    char registro[100];

    if (dequeue(prioridade, paciente)) {
        strcpy(tipo, "URGENTE");
    } else if (dequeue(normal, paciente)) {
        strcpy(tipo, "COMUM");
    } else {
        printf("  Nenhum paciente aguardando.\n");
        return;
    }

    printf("  Dr. %d chamando: %s [%s]\n", num_medico, paciente, tipo);

    snprintf(registro, sizeof(registro), "%s | %s | Dr.%d", paciente, tipo, num_medico);
    push(historico, registro);
    FILE *f = fopen("atendimentos.csv", "a");
    if (f) {
        fprintf(f, "%s,%s,%d\n", paciente, tipo, num_medico);
        fclose(f);
    }
}

void desfazer_ultima_acao(Pilha *historico, Pilha *acoes_desfeitas,
                           Fila *prioridade, Fila *normal) {
    char registro[100];

    if (!pop(historico, registro)) {
        printf("  Nada para desfazer.\n");
        return;
    }

    push(acoes_desfeitas, registro);

    char nome[50], tipo[20], resto[50];
    sscanf(registro, "%[^|]| %[^|]| %s", nome, tipo, resto);


    int len = strlen(nome);
    while (len > 0 && nome[len-1] == ' ') nome[--len] = '\0';

    printf("  Acao desfeita. Paciente '%s' recolocado na fila.\n", nome);

    if (strncmp(tipo, "URGENTE", 7) == 0) {
        enqueue(prioridade, nome);
    } else {
        enqueue(normal, nome);
    }
}

void separador(const char *titulo) {
    printf("\n");
    printf("==========================================================\n");
    printf("  CENARIO: %s\n", titulo);
    printf("==========================================================\n");
}

void cenario_fluxo_basico() {
    separador("Fluxo basico de teleconsultas");

    Fila normal, prioridade;
    Pilha historico, desfeitas;

    fila_inicializar(&normal);
    fila_inicializar(&prioridade);
    pilha_inicializar(&historico);
    pilha_inicializar(&desfeitas);

    printf("\n>> Pacientes entrando na fila comum:\n");
    enqueue(&normal, "Ana Lima");
    enqueue(&normal, "Carlos Souza");
    enqueue(&normal, "Beatriz Nunes");

    fila_imprimir(&normal, "Fila normal");

    char proximo[50];
    if (fila_peek(&normal, proximo))
        printf("  Proximo na fila: %s\n", proximo);

    printf("\n>> Medicos chamando pacientes:\n");
    processar_atendimento(&prioridade, &normal, &historico, 1);
    processar_atendimento(&prioridade, &normal, &historico, 2);
    processar_atendimento(&prioridade, &normal, &historico, 3);

    printf("\n>> Historico de atendimentos:\n");
    pilha_imprimir(&historico, "Historico");
}

void cenario_emergencia() {
    separador("Emergencia com fila de prioridade");

    Fila normal, prioridade;
    Pilha historico, desfeitas;

    fila_inicializar(&normal);
    fila_inicializar(&prioridade);
    pilha_inicializar(&historico);
    pilha_inicializar(&desfeitas);

    printf("\n>> Fila comum ja formada:\n");
    enqueue(&normal, "Roberto Alves");
    enqueue(&normal, "Fernanda Costa");
    enqueue(&normal, "Jorge Melo");

    printf("\n>> Alerta critico: dois pacientes em emergencia!\n");
    enqueue(&prioridade, "Lucia Ferreira");  
    enqueue(&prioridade, "Marcos Dias");

    fila_imprimir(&prioridade, "Fila urgente");
    fila_imprimir(&normal,     "Fila normal ");

    printf("\n>> Medico processando fila (urgentes primeiro):\n");
    int i;
    for (i = 1; i <= 5; i++) {
        processar_atendimento(&prioridade, &normal, &historico, 1);
    }

    printf("\n>> Historico (ultimo atendido = topo):\n");
    pilha_imprimir(&historico, "Historico");
}

void cenario_desfazer_refazer() {
    separador("Desfazer e refazer acao administrativa");

    Fila normal, prioridade;
    Pilha historico, desfeitas;

    fila_inicializar(&normal);
    fila_inicializar(&prioridade);
    pilha_inicializar(&historico);
    pilha_inicializar(&desfeitas);

    enqueue(&normal, "Patricia Gomes");
    enqueue(&normal, "Thiago Ramos");
    enqueue(&normal, "Camila Vieira");

    printf("\n>> Processando 2 atendimentos:\n");
    processar_atendimento(&prioridade, &normal, &historico, 2);
    processar_atendimento(&prioridade, &normal, &historico, 3);

    printf("\n>> Estado antes do desfazer:\n");
    pilha_imprimir(&historico, "Historico");
    fila_imprimir(&normal, "Fila normal");

    printf("\n>> Administrador desfaz ultimo atendimento:\n");
    desfazer_ultima_acao(&historico, &desfeitas, &prioridade, &normal);

    printf("\n>> Estado apos desfazer:\n");
    pilha_imprimir(&historico, "Historico");
    fila_imprimir(&normal, "Fila normal");

    char ultima[100];
    if (pilha_peek(&desfeitas, ultima))
        printf("\n  Acao que pode ser refeita: %s\n", ultima);

    printf("\n>> Processando fila apos correcao:\n");
    processar_atendimento(&prioridade, &normal, &historico, 2);
    processar_atendimento(&prioridade, &normal, &historico, 2);

    printf("\n>> Historico final:\n");
    pilha_imprimir(&historico, "Historico");
}


// MAIN

int main() {
    FILE *csv = fopen("atendimentos.csv", "w");
    if (csv) {
        fprintf(csv, "Paciente,Tipo,Medico\n");
        fclose(csv);
    }

    cenario_fluxo_basico();
    cenario_emergencia();
    cenario_desfazer_refazer();

    printf("\n\nTodos os atendimentos salvos em atendimentos.csv\n");
    return 0;
}