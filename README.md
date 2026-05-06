# Blua — Sistema de Gerenciamento de Fila de Teleconsultas

Trabalho prático desenvolvido para a disciplina de Estruturas de Dados. Simula o gerenciamento de atendimentos remotos da plataforma **Blua** da Care Plus, utilizando **Fila** e **Pilha** implementadas do zero com listas encadeadas em C.

---

## Contexto

A Care Plus está expandindo a plataforma Blua para oferecer cuidado remoto proativo. Um dos desafios centrais é gerenciar, de forma eficiente e segura, o fluxo de teleconsultas: solicitações chegam continuamente, médicos precisam atender na ordem correta, e alertas críticos de saúde devem ser priorizados imediatamente.

---

## Funcionalidades

- Enfileirar pacientes que solicitam teleconsulta (fila comum ou urgente)
- Priorizar emergências com fila diferenciada — pacientes urgentes sempre são atendidos antes
- Processar atendimentos simulando a chamada sequencial por um médico disponível
- Registrar histórico das consultas finalizadas em uma pilha (auditoria LIFO)
- Desfazer o último atendimento, recolocando o paciente na fila correta
- Persistir os atendimentos em arquivo `atendimentos.csv`
- Menu interativo para uso em tempo real

---

## Estruturas de Dados

Ambas as estruturas foram implementadas manualmente com listas encadeadas, **sem uso de bibliotecas prontas**.

### Fila (FIFO)
Utilizada para gerenciar a ordem de chegada dos pacientes. O sistema mantém duas filas simultâneas:

| Fila | Uso |
|---|---|
| `normal` | Pacientes comuns em ordem de chegada |
| `prioridade` | Pacientes em situação de emergência |

Operações implementadas: `enqueue`, `dequeue`, `fila_peek`, `fila_vazia`

### Pilha (LIFO)
Utilizada para o histórico de atendimentos e para a funcionalidade de desfazer ações administrativas.

| Pilha | Uso |
|---|---|
| `historico` | Registra cada atendimento finalizado |
| `acoes_desfeitas` | Guarda ações desfeitas para auditoria |

Operações implementadas: `push`, `pop`, `pilha_peek`, `pilha_vazia`

---

## Cenários Demonstrados

**Cenário 1 — Fluxo básico de teleconsultas**
Três pacientes entram na fila comum e são atendidos sequencialmente por médicos disponíveis. Demonstra o comportamento FIFO da fila e o registro no histórico.

**Cenário 2 — Emergência com fila de prioridade**
Com a fila comum já formada, dois pacientes em situação crítica são inseridos na fila urgente. O médico processa todos os atendimentos, sempre dando preferência aos urgentes.

**Cenário 3 — Desfazer e refazer ação administrativa**
Dois atendimentos são processados. O administrador desfaz o último, o paciente é recolocado na fila e o sistema segue normalmente. Demonstra o uso da pilha para controle de ações.

---

## Como executar

**Pré-requisito:** GCC instalado.

```bash
# Compilar
gcc teleconsultas.c -o teleconsultas

# Executar
./teleconsultas
```

No Windows:
```bash
gcc teleconsultas.c -o teleconsultas.exe
teleconsultas.exe
```

---

## Menu interativo

```
=========================================
   SISTEMA DE TELECONSULTAS
=========================================

[1] Adicionar paciente
[2] Chamar proximo paciente
[3] Ver proximo da fila
[4] Ver estado completo
[5] Desfazer ultimo atendimento
[0] Sair
```

Ao adicionar um paciente, o sistema pergunta o nome e se é atendimento **comum** ou **urgente**. Ao chamar o próximo, informa o número do médico responsável.

---

## Arquivos

```
.
├── teleconsultas.c     # Código-fonte principal
├── atendimentos.csv    # Gerado automaticamente ao executar
└── README.md
```

O arquivo `atendimentos.csv` é criado/sobrescrito a cada execução e registra todos os atendimentos no formato:

```
Paciente,Tipo,Medico
Ana Lima,COMUM,1
Lucia Ferreira,URGENTE,1
```

---

## Tecnologia

- Linguagem: **C (C99)**
- Compilador: **GCC**
- Estruturas: listas encadeadas com alocação dinâmica (`malloc`/`free`)
