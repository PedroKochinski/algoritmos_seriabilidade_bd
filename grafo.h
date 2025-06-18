/**
 * @file grafo.h
 * @brief Definição da estrutura de um Grafo e protótipos de suas funções.
 *
 * Este arquivo contém as estruturas de dados e as assinaturas das funções
 * para criar, manipular e analisar um grafo direcionado, usado para o teste
 * de seriabilidade por conflito.
 */
#ifndef GRAFO_H
#define GRAFO_H

/**
 * @struct Graph
 * @brief Representa um grafo direcionado usando uma matriz de adjacência.
 * @var Graph::num_vertices O número de vértices no grafo.
 * @var Graph::matrix A matriz de adjacência (ponteiro para ponteiro de int).
 */
typedef struct {
    int num_vertices;
    int** matrix;
} Graph;

// --- Protótipos das Funções ---

/**
 * @brief Aloca memória e inicializa um novo grafo.
 * @param num_vertices O número de vértices que o grafo terá.
 * @return Um ponteiro para o novo grafo criado, ou NULL em caso de falha.
 */
Graph* create_graph(int num_vertices);

/**
 * @brief Libera toda a memória alocada para o grafo.
 * @param g Ponteiro para o grafo a ser liberado.
 */
void free_graph(Graph* g);

/**
 * @brief Adiciona uma aresta direcionada no grafo.
 * @param g O grafo onde a aresta será adicionada.
 * @param from O vértice de origem (índice).
 * @param to O vértice de destino (índice).
 */
void add_edge(Graph* g, int from, int to);

/**
 * @brief Verifica se o grafo contém algum ciclo.
 *
 * Utiliza um algoritmo de busca em profundidade (DFS) para detectar
 * a presença de arestas de retorno (back edges), que indicam um ciclo.
 *
 * @param g O grafo a ser verificado.
 * @return 1 se um ciclo for encontrado, 0 caso contrário.
 */
int has_cycle(Graph* g);

#endif // GRAFO_H