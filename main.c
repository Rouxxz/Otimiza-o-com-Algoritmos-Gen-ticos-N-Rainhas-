/*
 * nqueens_ga_final.c
 * Algoritmo Genético para N-Rainhas
 * 
 * Versão Estendida:
 * - N = 8
 * - Representação por Permutação (evita conflitos de linha/coluna nativamente)
 * - main() sem parâmetros de linha de comando
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/* ---------------- Parâmetros do problema e do GA ---------------- */
#define N               8           /* tamanho do tabuleiro (modificado para 8) */
#define POP_SIZE        200         /* tamanho da população (aumentado para N=8) */
#define MAX_GEN         10000       /* máx. gerações (aumentado para N=8) */
#define CROSS_RATE      0.00f       /* taxa de crossover (0.0 para manter a permutação) */
#define MUT_RATE        0.80f       /* taxa de mutação (alta, pois é o motor principal agora) */
#define ELITE_COUNT     4           /* nº de elites preservados */
#define TOURNAMENT_K    3           /* tamanho do torneio */

/* ---------------- Representação de indivíduo -------------------- */
typedef struct {
    int gene[N];     /* gene[i] = coluna da rainha na linha i */
    int fitness;     /* aptidão */
} Individual;

/* -------------------- Utilidades aleatórias --------------------- */
static inline int rand_int(int a, int b) {
    /* inteiro uniforme em [a,b] (assume a <= b) */
    return a + rand() % (b - a + 1);
}

static inline double rand_unit() {
    /* real uniforme em [0,1) */
    return (double)rand() / (double)(RAND_MAX);
}

/* -------------------- Protótipos das funções -------------------- */
void random_individual(Individual *ind);
int  compute_conflicts(const Individual *ind);
int  evaluate(Individual *ind);
int  cmp_desc_fitness(const void *a, const void *b);
void copy_individual(const Individual *src, Individual *dst);
int  tournament_select(Individual pop[], int pop_size);
void one_point_crossover(const Individual *p1, const Individual *p2,
                         Individual *c1, Individual *c2);
void mutate(Individual *ind);
void print_board_ascii(const Individual *ind);

/* -------------------- Implementações -------------------- */

void random_individual(Individual *ind) {
    /* Representação por Permutação: preenche com 0 a N-1 e embaralha */
    for (int i = 0; i < N; ++i) {
        ind->gene[i] = i;
    }
    
    /* Algoritmo de embaralhamento de Fisher-Yates */
    for (int i = N - 1; i > 0; --i) {
        int j = rand_int(0, i);
        int temp = ind->gene[i];
        ind->gene[i] = ind->gene[j];
        ind->gene[j] = temp;
    }
    ind->fitness = 0;
}

int compute_conflicts(const Individual *ind) {
    /* Como é uma permutação, não existem conflitos de linha ou coluna.
       Só precisamos checar as diagonais. */
    int conflicts = 0;
    for (int i = 0; i < N; ++i) {
        for (int j = i + 1; j < N; ++j) {
            if (abs(i - j) == abs(ind->gene[i] - ind->gene[j])) {
                conflicts++;
            }
        }
    }
    return conflicts;
}

int evaluate(Individual *ind) {
    const int TOTAL_PAIRS = N * (N - 1) / 2;
    int conflicts = compute_conflicts(ind);
    ind->fitness = TOTAL_PAIRS - conflicts;
    return ind->fitness;
}

int cmp_desc_fitness(const void *a, const void *b) {
    const Individual *ia = (const Individual*)a;
    const Individual *ib = (const Individual*)b;
    return ib->fitness - ia->fitness;
}

void copy_individual(const Individual *src, Individual *dst) {
    for (int i = 0; i < N; ++i) {
        dst->gene[i] = src->gene[i];
    }
    dst->fitness = src->fitness;
}

int tournament_select(Individual pop[], int pop_size) {
    int best_idx = -1;
    int best_fitness = -1;

    for (int k = 0; k < TOURNAMENT_K; ++k) {
        int current_idx = rand_int(0, pop_size - 1);
        if (pop[current_idx].fitness > best_fitness || best_idx == -1) {
            best_fitness = pop[current_idx].fitness;
            best_idx = current_idx;
        }
    }
    return best_idx;
}

void one_point_crossover(const Individual *p1, const Individual *p2,
                         Individual *c1, Individual *c2) {
    /* Como usamos permutação, o crossover de 1 ponto criaria clones (genes repetidos).
       Por isso, setamos CROSS_RATE para 0.0. Quando cai aqui, ele apenas copia. */
    if (rand_unit() < CROSS_RATE) {
        int cutoff = rand_int(1, N - 1); 
        for (int i = 0; i < cutoff; ++i) c1->gene[i] = p1->gene[i];
        for (int i = cutoff; i < N; ++i) c1->gene[i] = p2->gene[i];
        
        for (int i = 0; i < cutoff; ++i) c2->gene[i] = p2->gene[i];
        for (int i = cutoff; i < N; ++i) c2->gene[i] = p1->gene[i];
    } else {
        copy_individual(p1, c1);
        copy_individual(p2, c2);
    }
    c1->fitness = 0;
    c2->fitness = 0;
}

void mutate(Individual *ind) {
    /* Mutação de troca (Swap): preserva a propriedade de permutação */
    if (rand_unit() < MUT_RATE) {
        int i = rand_int(0, N - 1);
        int j = rand_int(0, N - 1);
        
        int temp = ind->gene[i];
        ind->gene[i] = ind->gene[j];
        ind->gene[j] = temp;
    }
    ind->fitness = 0;
}

void print_board_ascii(const Individual *ind) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (ind->gene[i] == j) printf("Q ");
            else                   printf(". ");
        }
        printf("\n");
    }
}

/* --------------------------- main ------------------------------- */
int main() {
    /* Seed baseada no tempo local */
    srand((unsigned int)time(NULL));

    Individual pop[POP_SIZE], new_pop[POP_SIZE];

    /* 1) Inicialização e avaliação */
    for (int i = 0; i < POP_SIZE; ++i) {
        random_individual(&pop[i]);
        evaluate(&pop[i]);
    }

    /* 2) Ordenação inicial */
    qsort(pop, POP_SIZE, sizeof(Individual), cmp_desc_fitness);

    const int TOTAL_PAIRS = N * (N - 1) / 2;
    int best_gen_found = -1;

    /* 3) Loop evolutivo */
    for (int gen = 1; gen <= MAX_GEN; ++gen) {

        /* 3a) Elitismo */
        for (int e = 0; e < ELITE_COUNT; ++e) {
            copy_individual(&pop[e], &new_pop[e]);
        }

        /* 3b) Reprodução: seleção, crossover, mutação */
        for (int i = ELITE_COUNT; i < POP_SIZE; i += 2) {
            int p1 = tournament_select(pop, POP_SIZE);
            int p2 = tournament_select(pop, POP_SIZE);

            Individual c1, c2;
            one_point_crossover(&pop[p1], &pop[p2], &c1, &c2);
            mutate(&c1);
            mutate(&c2);
            evaluate(&c1);
            evaluate(&c2);

            new_pop[i] = c1;
            if (i + 1 < POP_SIZE) new_pop[i + 1] = c2;
        }

        /* 3c) Substituição e ordenação */
        for (int i = 0; i < POP_SIZE; ++i) pop[i] = new_pop[i];
        qsort(pop, POP_SIZE, sizeof(Individual), cmp_desc_fitness);

        /* 3d) Parada por solução ótima (fitness = C(N,2)) */
        if (pop[0].fitness == TOTAL_PAIRS) {
            best_gen_found = gen;
            break;
        }
    }

    /* 4) Saída no terminal */
    Individual best = pop[0];
    printf("Melhor fitness: %d (ótimo = %d)\n", best.fitness, TOTAL_PAIRS);
    printf("Cromossomo (lin->col): ");
    for (int i = 0; i < N; ++i) printf("%d ", best.gene[i]);
    printf("\n\nTabuleiro:\n");
    print_board_ascii(&best);

    if (best_gen_found >= 0)
        printf("\nSolução ótima alcançada na geração: %d\n", best_gen_found);
    else
        printf("\nSolução ótima NÃO alcançada. Melhor encontrada:\n");

    return 0;
}
