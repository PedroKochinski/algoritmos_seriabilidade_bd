
/**
 * @file graph.c
 * @brief Implementação das funções de manipulação de grafo.
 *
 * Contém a lógica para criar, destruir, adicionar arestas e detectar
 * ciclos em um grafo direcionado. A detecção de ciclo é feita com DFS.
 */
#include <stdlib.h>
#include <stdio.h>
#include "grafo.h"

// Enum para as cores dos vertices usadas na detecção de ciclo por DFS
typedef enum { WHITE, GRAY, BLACK } Color;

int has_cycle_util(Graph* g, int u, Color* color);

// --- Implementações ---

Graph* create_graph(int num_vertices) {
    if (num_vertices <= 0) return NULL;

    Graph* g = (Graph*)malloc(sizeof(Graph));
    if (!g) {
        perror("Falha ao alocar memória para o grafo");
        return NULL;
    }
    g->num_vertices = num_vertices;

    g->matrix = (int**)malloc(num_vertices * sizeof(int*));
    if (!g->matrix) {
        perror("Falha ao alocar memória para as linhas da matriz");
        free(g);
        return NULL;
    }

    for (int i = 0; i < num_vertices; i++) {
        g->matrix[i] = (int*)calloc(num_vertices, sizeof(int));
        if (!g->matrix[i]) {
            perror("Falha ao alocar memória para as colunas da matriz");
            // Libera o que já foi alocado
            for (int j = 0; j < i; j++) {
                free(g->matrix[j]);
            }
            free(g->matrix);
            free(g);
            return NULL;
        }
    }
    return g;
}

void free_graph(Graph* g) {
    if (!g) return;
    for (int i = 0; i < g->num_vertices; i++) {
        free(g->matrix[i]);
    }
    free(g->matrix);
    free(g);
}

void add_edge(Graph* g, int from, int to) {
    if (g && from < g->num_vertices && to < g->num_vertices) {
        g->matrix[from][to] = 1;
    }
}

/**
 * @brief Função recursiva auxiliar para a detecção de ciclo (DFS).
 * @param g O grafo.
 * @param u O vertice atual sendo visitado.
 * @param color Array de cores que rastreia o estado de cada vertice.
 * @return 1 se um ciclo for detectado, 0 caso contrário.
 */
int has_cycle_util(Graph* g, int u, Color* color) {
    color[u] = GRAY; // Marca o vertice como visitando

    for (int v = 0; v < g->num_vertices; v++) {
        if (g->matrix[u][v] == 1) { // Se existe uma aresta de u para v
            if (color[v] == GRAY) {
                // vertice v está na pilha de recursão entao tem ciclo
                return 1;
            }
            if (color[v] == WHITE && has_cycle_util(g, v, color)) {
                // se v não foi visitado, visita e verifica por ciclos a partir dele.
                return 1;
            }
        }
    }

    color[u] = BLACK; // Marca o vertice como totalmente visitado.
    return 0;
}

int has_cycle(Graph* g) {
    if (!g) return 0;

    // Array para armazenar as cores de cada vertice.
    // WHITE: não visitado, GRAY: visitando, BLACK: já visitado.
    Color* color = (Color*)calloc(g->num_vertices, sizeof(Color));
    if (!color) {
        perror("Falha ao alocar memória para o vetor de cores");
        return 1; // Assume o pior caso para segurança
    }

    // Chama a função de busca para cada vertice não visitado
    for (int i = 0; i < g->num_vertices; i++) {
        if (color[i] == WHITE) {
            if (has_cycle_util(g, i, color)) {
                free(color);
                return 1;
            }
        }
    }

    free(color);
    return 0;
}
