/**
 * @file algorithms.c
 * @brief Implementação dos algoritmos de seriabilidade.
 *
 * logica para os testes de seriabilidade por conflito e equivalência por visão
 *  alem de funcoes auxiliares para manipular estrutura de escalonamento.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algoritmos.h"
#include "grafo.h"

Schedule* create_schedule() {
    Schedule* s = (Schedule*)malloc(sizeof(Schedule));
    if (!s) {
        perror("Falha ao alocar Schedule");
        return NULL;
    }
    s->op_count = 0;
    s->op_capacity = 10; // Capacidade inicial
    s->ops = (Operation*)malloc(s->op_capacity * sizeof(Operation));
    if (!s->ops) {
        perror("Falha ao alocar array de operações");
        free(s);
        return NULL;
    }
    s->trans_ids = NULL;
    s->trans_count = 0;
    return s;
}

void free_schedule(Schedule* s) {
    if (!s) return;
    free(s->ops);
    free(s->trans_ids);
    free(s);
}

void add_operation(Schedule* s, int time, int trans_id, char op, char attr) {
    // Redimensiona o array de operações se necessário
    if (s->op_count >= s->op_capacity) {
        s->op_capacity *= 2;
        Operation* new_ops = (Operation*)realloc(s->ops, s->op_capacity * sizeof(Operation));
        if (!new_ops) {
            perror("Falha ao realocar array de operações");
            // A aplicação irá parar, mas em um sistema real, trataria o erro
            return;
        }
        s->ops = new_ops;
    }
    s->ops[s->op_count++] = (Operation){time, trans_id, op, attr};
}

// Função de comparação para qsort
int compare_ints(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

void find_unique_transactions(Schedule* s) {
    if (s->op_count == 0) return;

    int* temp_ids = (int*)malloc(s->op_count * sizeof(int));
    int count = 0;
    for (int i = 0; i < s->op_count; i++) {
        int found = 0;
        for (int j = 0; j < count; j++) {
            if (temp_ids[j] == s->ops[i].trans_id) {
                found = 1;
                break;
            }
        }
        if (!found) {
            temp_ids[count++] = s->ops[i].trans_id;
        }
    }

    s->trans_ids = (int*)malloc(count * sizeof(int));
    memcpy(s->trans_ids, temp_ids, count * sizeof(int));
    s->trans_count = count;
    free(temp_ids);

    // Ordena os IDs para consistência
    qsort(s->trans_ids, s->trans_count, sizeof(int), compare_ints);
}

// --- Mapeamento de ID de Transação para Índice do Grafo ---

int get_trans_index(Schedule* s, int trans_id) {
    for (int i = 0; i < s->trans_count; i++) {
        if (s->trans_ids[i] == trans_id) {
            return i;
        }
    }
    return -1; // Não deve acontecer
}

// --- Algoritmo de Seriabilidade por Conflito ---

int is_conflict_serializable(Schedule* s) {
    if (s->trans_count <= 1) return 1;

    Graph* g = create_graph(s->trans_count);
    if (!g) return 0; // Assume não serializável em caso de erro

    // Itera por todos os pares de operações para encontrar conflitos
    for (int i = 0; i < s->op_count; i++) {
        for (int j = i + 1; j < s->op_count; j++) {
            Operation op1 = s->ops[i];
            Operation op2 = s->ops[j];

            // Conflitos só ocorrem entre transações diferentes no mesmo atributo
            if (op1.trans_id == op2.trans_id || op1.attr != op2.attr || op1.attr == '-') {
                continue;
            }

            // Pelo menos uma deve ser escrita (R-R não é conflito)
            if (op1.op == 'W' || op2.op == 'W') {
                int idx1 = get_trans_index(s, op1.trans_id);
                int idx2 = get_trans_index(s, op2.trans_id);
                add_edge(g, idx1, idx2);
            }
        }
    }

    int has_cycle_result = has_cycle(g);
    free_graph(g);
    return !has_cycle_result;
}

// --- funcoes Auxiliares para Equivalência por Visão ---

// Estrutura para representar a relação "Lido-De" (Read-From)
typedef struct {
    int reader_trans_id;
    int writer_trans_id;
    char attr;
} ReadFrom;

// Encontra a última escrita de um atributo 'attr' antes do índice 'before_op_idx'
int find_last_writer(Schedule* s, char attr, int before_op_idx) {
    int last_writer_id = 0; // 0 representa o valor inicial no banco
    for (int i = 0; i < before_op_idx; i++) {
        if (s->ops[i].op == 'W' && s->ops[i].attr == attr) {
            last_writer_id = s->ops[i].trans_id;
        }
    }
    return last_writer_id;
}

// Encontra a última escrita de um atributo em um escalonamento serial
int find_last_writer_serial(Schedule* s, const int* serial_order, int order_len, char attr) {
     int last_writer_id = 0;
     for (int i = 0; i < order_len; i++) {
        int current_tid = serial_order[i];
        for(int j = 0; j < s->op_count; j++) {
            if(s->ops[j].trans_id == current_tid && s->ops[j].op == 'W' && s->ops[j].attr == attr) {
                last_writer_id = current_tid;
            }
        }
     }
     return last_writer_id;
}


// Compara as escritas finais entre o escalonamento original e um serial
int compare_final_writes(Schedule* s, const int* serial_order, int order_len) {
    char processed_attrs[26] = {0}; // Assumindo atributos de 'A' a 'Z'
    for (int i = 0; i < s->op_count; i++) {
        char attr = s->ops[i].attr;
        if (attr != '-' && !processed_attrs[attr - 'A']) {
            int original_final_writer = find_last_writer(s, attr, s->op_count);
            int serial_final_writer = find_last_writer_serial(s, serial_order, order_len, attr);
            if(original_final_writer != serial_final_writer) {
                return 0;
            }
            processed_attrs[attr - 'A'] = 1;
        }
    }
    return 1;
}


int compare_read_from(Schedule* s, const int* serial_order, int order_len) {
    for (int i = 0; i < s->op_count; i++) {
        if (s->ops[i].op == 'R') {
            Operation read_op = s->ops[i];
            
            // Relação no escalonamento original
            int original_writer = find_last_writer(s, read_op.attr, i);

            // Relação no escalonamento serial
            int serial_writer = 0; 
            int reader_pos = -1;

            for(int j = 0; j < order_len; j++) {
                if(serial_order[j] == read_op.trans_id) reader_pos = j;
            }

            for(int j = 0; j < reader_pos; j++) {
                int current_tid = serial_order[j];
                 for(int k = 0; k < s->op_count; k++) {
                    if (s->ops[k].trans_id == current_tid && s->ops[k].op == 'W' && s->ops[k].attr == read_op.attr) {
                        serial_writer = current_tid;
                    }
                }
            }

            if (original_writer != serial_writer) {
                return 0;
            }
        }
    }
    return 1;
}


// Função para trocar dois inteiros
void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Função recursiva para gerar e testar permutações
int check_all_permutations(Schedule* s, int* arr, int start, int end) {
    if (start == end) {
        // Uma permutação (escalonamento serial) foi gerada. Testá-la.
        if (compare_final_writes(s, arr, end + 1) && compare_read_from(s, arr, end + 1)) {
            return 1;
        }
        return 0;
    }
    for (int i = start; i <= end; i++) {
        swap((arr + start), (arr + i));
        if (check_all_permutations(s, arr, start + 1, end)) {
            return 1;
        }
        swap((arr + start), (arr + i)); // Backtrack
    }
    return 0;
}

// --- Algoritmo de Seriabilidade por Visão ---

int is_view_serializable(Schedule* s) {
    // Teorema: Todo escalonamento serializável por conflito é também serializável por visão.
    if (is_conflict_serializable(s)) {
        return 1;
    }

    // Se não for serializável por conflito, pode ser por visão (ex: com escritas cegas).
    // Testamos a equivalência com todas as permutações seriais.
    int* trans_ids_copy = (int*)malloc(s->trans_count * sizeof(int));
    if (!trans_ids_copy) return 0;
    
    memcpy(trans_ids_copy, s->trans_ids, s->trans_count * sizeof(int));

    int result = check_all_permutations(s, trans_ids_copy, 0, s->trans_count - 1);

    free(trans_ids_copy);
    return result;
}
