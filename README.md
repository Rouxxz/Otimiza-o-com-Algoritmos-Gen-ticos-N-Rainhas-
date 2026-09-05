# 👑 N-Rainhas com Algoritmo Genético (em C)

Projeto desenvolvido em **linguagem C** para implementação e estudo de **Algoritmos Genéticos (GA)** aplicados ao clássico problema das **N-Rainhas**.

O sistema utiliza conceitos de evolução inspirados na biologia (como seleção, mutação e elitismo) para encontrar uma configuração onde 8 rainhas sejam posicionadas em um tabuleiro de xadrez sem se atacarem mutuamente.

---

## 📌 Sobre o Projeto

O **Problema das N-Rainhas** (neste caso, N = 8) consiste em posicionar N rainhas em um tabuleiro N×N de forma que nenhuma rainha consiga atacar outra. Isso significa que duas rainhas não podem compartilhar a mesma linha, a mesma coluna ou a mesma diagonal.

O programa resolve o problema utilizando uma abordagem de **Algoritmo Genético** com a seguinte configuração:

* 🧬 **Representação por Permutação:** Cada indivíduo é um array onde o índice representa a linha e o valor representa a coluna da rainha. Isso impede nativamente os conflitos de linha e coluna.
* 👨‍👩‍👧 **População:** 200 indivíduos.
* 🔄 **Gerações Máximas:** 10.000.
* 🏆 **Elitismo:** Preserva os 4 melhores indivíduos para a próxima geração.
* ⚔️ **Torneio:** Seleciona pais através de torneios de tamanho K=3.

---

## 🚀 Funcionalidades

### 🧬 Representação do Cromossomo (Indivíduo)

Ao invés de mapear as $N^2$ posições do tabuleiro, a representação utiliza **Permutação**:

```c
typedef struct {
    int gene[N];     /* gene[i] = coluna da rainha na linha i */
    int fitness;     /* aptidão */
} Individual;
```

A inicialização usa o algoritmo de embaralhamento de **Fisher-Yates**, garantindo que não existam rainhas na mesma coluna inicialmente.

---

### 📏 Avaliação de Aptidão (Fitness)

A função `evaluate` e `compute_conflicts` formam o núcleo de cálculo de sucesso.
Como as linhas e colunas já estão protegidas pela representação, o programa só checa os **conflitos de diagonal**:

```c
if (abs(i - j) == abs(ind->gene[i] - ind->gene[j]))
```

A aptidão (fitness) máxima é o número total de pares de rainhas sem conflitos: `C(N, 2) = N * (N - 1) / 2`.
Para N=8, o fitness ótimo é **28**.

---

### 🎲 Evolução: Mutação e Crossover

Devido à natureza da Permutação, o Crossover de 1 ponto tradicional geraria clones defeituosos na mesma coluna. Portanto:

* 🚫 **Taxa de Crossover (`CROSS_RATE`):** Definida em `0.0`. O Crossover age apenas como copiador.
* ⚡ **Taxa de Mutação (`MUT_RATE`):** Definida alta em `0.80` (80%). A mutação atua como o motor principal de exploração (Swap Mutation).

A mutação de **Swap (Troca)** escolhe duas posições aleatórias e inverte suas colunas, garantindo a integridade da permutação.

---

## 🏗️ Estrutura do Loop Evolutivo

O núcleo do programa executa os seguintes passos por geração:

1. **Elitismo:** Copia os melhores indivíduos direto para a nova população.
2. **Seleção:** Usa `tournament_select` para escolher pais.
3. **Reprodução:** (No caso atual, copia e aplica Swap Mutation intensa).
4. **Substituição:** A nova população substitui a antiga.
5. **Ordenação:** Organiza a população usando `qsort` do maior para o menor fitness.
6. **Critério de Parada:** Interrompe se o fitness alcançar o valor perfeito (28).

---

## 🏁 Exibição da Solução

Ao encontrar a solução, o programa exibe no terminal as posições do cromossomo e a disposição em formato de tabuleiro ASCII:

```text
Melhor fitness: 28 (ótimo = 28)
Cromossomo (lin->col): 5 3 1 7 4 6 0 2 

Tabuleiro:
. . . . . Q . . 
. . . Q . . . . 
. Q . . . . . . 
. . . . . . . Q 
. . . . Q . . . 
. . . . . . Q . 
Q . . . . . . . 
. . Q . . . . . 

Solução ótima alcançada na geração: 142
```

---

## 🛠️ Tecnologias Utilizadas

* **C (Linguagem C)**
* Estruturas de dados personalizadas (Structs)
* Geração de números pseudo-aleatórios (`rand`, `srand`)
* Manipulação de memória e ordenação em bloco (`qsort`)

Bibliotecas utilizadas:

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
```

---

## ▶️ Como Executar

### 1. Salve o arquivo

Salve o código fornecido em um arquivo local, por exemplo, `nqueens.c`.

### 2. Compile o programa

Utilizando o GCC, compile o código (incluindo a biblioteca math se necessário em alguns ambientes UNIX):

```bash
gcc nqueens.c -o nqueens -lm
```

### 3. Execute

No Linux/macOS:

```bash
./nqueens
```

No Windows:

```bash
nqueens.exe
```

---

## 📚 Conceitos de Inteligência Artificial / Computação Evolutiva

Este projeto permite aplicar conceitos importantes de **Algoritmos Genéticos**, incluindo:

* Representação genotípica;
* Função de avaliação heurística;
* Mutações focadas (Swap) para problemas combinatórios;
* Seleção por torneio versus aptidão;
* Elitismo para evitar degradação geracional.

---

## 👨‍💻 Autor

**Arthur Bergamasco Constantino**

Estudante de **Ciência de Dados e Inteligência Artificial**.

---

## 📄 Licença

Este projeto está licenciado sob a Licença MIT - veja o arquivo LICENSE para obter detalhes.
