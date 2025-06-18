/**
 * @file algorithms.h
 * @brief Definição das estruturas de dados do escalonamento e dos algoritmos.
 *
 * contem a declaração das structs para Operação e Escalonamento, e os
 * prototipos dos algoritmos de teste de seriabilidade por conflito e
 * equivalencia por visao.
 */
#ifndef ALGORITMOS_H
#define ALGORITMOS_H

/**
 * @struct Operation
 * @brief Representa uma unica operação em uma transacao.
 */
typedef struct {
    int time; // timestamp da operação.
    int trans_id; // ID da transação que pertence.
    char op; // Tipo de operação (R, W, C).
    char attr; // Atributo (item de dado) sendo acessado.
} Operation;

/**
 * @struct Schedule
 * @brief Representa um escalonamento completo de operacoes.
 */
typedef struct {
    Operation* ops; // Array dinâmico de operacoes.
    int op_count; // Número de operacoes no escalonamento.
    int op_capacity; // Capacidade atual do array de operacoes.
    int* trans_ids; // Array com os IDs únicos das transacoes.
    int trans_count; // Número de transacoes únicas.
} Schedule;

/**
 * @brief Cria e inicializa uma nova estrutura de escalonamento.
 * @return Ponteiro para o Schedule criado ou NULL em caso de erro.
 */
Schedule* create_schedule();

/**
 * @brief Libera toda a memoria associada a um escalonamento.
 * @param s O escalonamento a ser liberado.
 */
void free_schedule(Schedule* s);

/**
 * @brief Adiciona uma nova operação ao escalonamento.
 * @param s O escalonamento.
 * @param time O tempo de chegada.
 * @param trans_id O ID da transação.
 * @param op O tipo de operação.
 * @param attr O atributo.
 */
void add_operation(Schedule* s, int time, int trans_id, char op, char attr);

/**
 * @brief Encontra e armazena os IDs únicos de transação do escalonamento.
 * @param s O escalonamento a ser analisado.
 */
void find_unique_transactions(Schedule* s);

/**
 * @brief Testa se o escalonamento é serializável por conflito.
 *
 * Constrói um grafo de precedência e verifica se há ciclos.
 *
 * @param s O escalonamento a ser testado.
 * @return 1 se for serializável por conflito e 0 caso contrario.
 */
int is_conflict_serializable(Schedule* s);

/**
 * @brief Testa se o escalonamento é serializável por visão.
 *
 * Compara o escalonamento original com todas as possíveis permutações
 * de escalonamentos seriais formados pelas mesmas transações.
 *
 * @param s O escalonamento a ser testado.
 * @return 1 se for equivalente por visão a algum escalonamento serial e 0 caso contrario.
 */
int is_view_serializable(Schedule* s);

#endif // ALGORITMOS_H