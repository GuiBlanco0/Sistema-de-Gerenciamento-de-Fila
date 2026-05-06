#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    if (fila_vazia(f)) { printf("(vazia)\n"); return; }
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
    if (pilha_vazia(p)) { printf("    (vazia)\n"); return; }
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

    printf("  Dr.%d chamando: %s [%s]\n", num_medico, paciente, tipo);
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

    if (strncmp(tipo, "URGENTE", 7) == 0)
        enqueue(prioridade, nome);
    else
        enqueue(normal, nome);
}

void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void exibir_estado(Fila *prioridade, Fila *normal, Pilha *historico) {
    printf("\n--- Estado atual ---\n");
    fila_imprimir(prioridade, "Fila urgente");
    fila_imprimir(normal,     "Fila normal ");
    pilha_imprimir(historico, "Historico   ");
    printf("--------------------\n");
}

void menu_adicionar(Fila *prioridade, Fila *normal) {
    char nome[50];
    int tipo;

    printf("  Nome do paciente: ");
    fgets(nome, sizeof(nome), stdin);
    nome[strcspn(nome, "\n")] = '\0';
    if (strlen(nome) == 0) { printf("  Nome invalido.\n"); return; }

    printf("  Tipo: 1 - Comum  2 - Urgente\n  > ");
    if (scanf("%d", &tipo) != 1) { limpar_buffer(); printf("  Opcao invalida.\n"); return; }
    limpar_buffer();

    if (tipo == 1) {
        enqueue(normal, nome);
        printf("  '%s' adicionado na fila comum.\n", nome);
    } else if (tipo == 2) {
        enqueue(prioridade, nome);
        printf("  '%s' adicionado na fila urgente.\n", nome);
    } else {
        printf("  Opcao invalida.\n");
    }
}

void menu_chamar(Fila *prioridade, Fila *normal, Pilha *historico) {
    int num;
    printf("  Numero do medico: ");
    if (scanf("%d", &num) != 1 || num <= 0) {
        limpar_buffer();
        printf("  Numero invalido.\n");
        return;
    }
    limpar_buffer();
    processar_atendimento(prioridade, normal, historico, num);
}

int main() {
    Fila normal, prioridade;
    Pilha historico, desfeitas;

    fila_inicializar(&normal);
    fila_inicializar(&prioridade);
    pilha_inicializar(&historico);
    pilha_inicializar(&desfeitas);

    FILE *csv = fopen("atendimentos.csv", "w");
    if (csv) { fprintf(csv, "Paciente,Tipo,Medico\n"); fclose(csv); }

    int opcao;

    printf("=========================================\n");
    printf("   SISTEMA DE TELECONSULTAS\n");
    printf("=========================================\n");

    do {
        printf("\n[1] Adicionar paciente\n");
        printf("[2] Chamar proximo paciente\n");
        printf("[3] Ver proximo da fila\n");
        printf("[4] Ver estado completo\n");
        printf("[5] Desfazer ultimo atendimento\n");
        printf("[0] Sair\n");
        printf("> ");

        if (scanf("%d", &opcao) != 1) { limpar_buffer(); continue; }
        limpar_buffer();
        printf("\n");

        switch (opcao) {
            case 1:
                menu_adicionar(&prioridade, &normal);
                break;

            case 2:
                menu_chamar(&prioridade, &normal, &historico);
                break;

            case 3: {
                char prox[50];
                if (fila_peek(&prioridade, prox))
                    printf("  Proximo (urgente): %s\n", prox);
                else if (fila_peek(&normal, prox))
                    printf("  Proximo (comum): %s\n", prox);
                else
                    printf("  Nenhum paciente aguardando.\n");
                break;
            }

            case 4:
                exibir_estado(&prioridade, &normal, &historico);
                break;

            case 5:
                desfazer_ultima_acao(&historico, &desfeitas, &prioridade, &normal);
                break;

            case 0:
                printf("  Encerrando. Atendimentos salvos em atendimentos.csv\n");
                break;

            default:
                printf("  Opcao invalida.\n");
        }

    } while (opcao != 0);

    return 0;
}
