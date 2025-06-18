/**
 * @file main.c
 * @brief inicio do programa escalona.
 *
 * le as transacoes do stdin e executa os testes de seriabilidade
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algoritmos.h"

/**
 * @brief Adiciona um ID de transação na lista de ativas, se ainda não estiver presente.
 * @param active_list Ponteiro para o array de IDs de transacoes ativas.
 * @param count Ponteiro para o número de transacoes ativas.
 * @param capacity Ponteiro para a capacidade do array.
 * @param trans_id O ID da transação a ser adicionado.
 */
void add_active_trans(int** active_list, int* count, int* capacity, int trans_id) {
    for (int i = 0; i < *count; i++) {
        if ((*active_list)[i] == trans_id) {
            return; // Transação já está na lista.
        }
    }

    if (*count >= *capacity) {
        *capacity = (*capacity == 0) ? 10 : *capacity * 2;
        int* new_list = (int*)realloc(*active_list, *capacity * sizeof(int));
        if (!new_list) {
            perror("Falha ao realocar lista de transacoes ativas");
            exit(EXIT_FAILURE);
        }
        *active_list = new_list;
    }

    (*active_list)[*count] = trans_id;
    (*count)++;
}

/**
 * @brief Remove um ID de transação da lista de transacoes ativas.
 * @param active_list O array de IDs de transacoes ativas.
 * @param count Ponteiro para o número de transacoes ativas.
 * @param trans_id O ID da transação a ser removido.
 */
void remove_active_trans(int* active_list, int* count, int trans_id) {
    int found_idx = -1;
    for (int i = 0; i < *count; i++) {
        if (active_list[i] == trans_id) {
            found_idx = i;
            break;
        }
    }

    if (found_idx != -1) {
        // Para remover eficientemente, move o último elemento para a posição do encontrado
        active_list[found_idx] = active_list[*count - 1];
        (*count)--;
    }
}

/**
 * @brief Processa um escalonamento completo: executa os testes e imprime o resultado.
 * @param s O escalonamento a ser processado.
 * @param schedule_id O identificador numérico do escalonamento.
 */
void process_schedule(Schedule* s, int schedule_id) {
    if (s == NULL || s->op_count == 0) return;

    find_unique_transactions(s);

    int conflict_serializable = is_conflict_serializable(s);
    int view_serializable = is_view_serializable(s);

    printf("%d ", schedule_id);
    for (int i = 0; i < s->trans_count; i++) {
        printf("%d%s", s->trans_ids[i], (i == s->trans_count - 1) ? "" : ",");
    }
    printf(" %s", conflict_serializable ? "SS" : "NS");
    printf(" %s\n", view_serializable ? "SV" : "NV");
}


int main() {
    int time, trans_id;
    char op_str[2], attr_str[2];
    
    int schedule_counter = 1;
    Schedule* current_schedule = create_schedule();

    // Estruturas para rastrear transacoes ativas no escalonamento atual
    int* active_trans = NULL;
    int active_trans_count = 0;
    int active_trans_capacity = 0;

    // Lê da entrada padrão ate o final do arquivo
    while (scanf("%d %d %1s %1s", &time, &trans_id, op_str, attr_str) == 4) {
        char op = op_str[0];
        char attr = attr_str[0];

        // Adiciona a operação ao escalonamento que está sendo construído
        add_operation(current_schedule, time, trans_id, op, attr);

        // Adiciona a transação na lista de ativas
        add_active_trans(&active_trans, &active_trans_count, &active_trans_capacity, trans_id);
        
        // Se a operação for um commit, a transação deixa de estar ativa
        if (op == 'C') {
            remove_active_trans(active_trans, &active_trans_count, trans_id);
        }

        // Se não houver mais transacoes ativas, o escalonamento atual terminou e pode ser processado
        if (active_trans_count == 0 && current_schedule->op_count > 0) {
            process_schedule(current_schedule, schedule_counter);
            
            // Limpa e prepara para o próximo escalonamento
            schedule_counter++;
            free_schedule(current_schedule);
            current_schedule = create_schedule();
            // A lista de transacoes ativas já está vazia, pronta para o próximo
        }
    }

    // Libera a memória alocada que não foi usada
    free_schedule(current_schedule);
    free(active_trans);

    return EXIT_SUCCESS;
}

