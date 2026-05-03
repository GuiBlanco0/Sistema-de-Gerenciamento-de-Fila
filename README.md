# Sistema de Gerenciamento de Fila de Atendimento - Blua

Trabalho prático da Sprint 3 — implementação de Fila e Pilha com listas encadeadas em C, aplicado ao contexto da plataforma Blua (Care Plus).

## Sobre o projeto

O sistema simula o fluxo de atendimento remoto da Blua: pacientes entram em fila, emergências são priorizadas, médicos processam os atendimentos em ordem, e todas as ações ficam registradas em histórico.

## Estruturas implementadas

**Fila (FIFO)** — usada para a fila de teleconsultas comuns e a fila de emergências  
Operações: `enqueue`, `dequeue`, `fila_peek`, `fila_vazia`

**Pilha (LIFO)** — usada para o histórico de atendimentos e para desfazer ações administrativas  
Operações: `push`, `pop`, `pilha_peek`, `pilha_vazia`

## Cenários demonstrados

| Cenário | Descrição |
|---|---|
| 1 — Fluxo básico | Pacientes entram na fila comum e são atendidos em ordem FIFO |
| 2 — Emergência | Pacientes urgentes são priorizados e atendidos antes dos da fila normal |
| 3 — Desfazer/refazer | Administrador desfaz um atendimento; paciente volta para a fila |

## Como executar

### OnlineGDB

1. Acesse [onlinegdb.com](https://onlinegdb.com/WYDa52Jiz)
2. Selecione a linguagem **C**
4. Clique em **Run**

## Saída esperada

O programa imprime os 3 cenários no terminal e gera um arquivo `atendimentos.csv` com o registro de todos os atendimentos processados:

```
Paciente,Tipo,Medico
Ana Lima,COMUM,1
Carlos Souza,COMUM,2
Beatriz Nunes,COMUM,3
Lucia Ferreira,URGENTE,1
Marcos Dias,URGENTE,1
Roberto Alves,COMUM,1
Fernanda Costa,COMUM,1
Jorge Melo,COMUM,1
Patricia Gomes,COMUM,2
Thiago Ramos,COMUM,3
Camila Vieira,COMUM,2
Thiago Ramos,COMUM,2
```