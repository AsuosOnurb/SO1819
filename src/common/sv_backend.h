#ifndef SO1819_SV_BACKEND_H
#define SO1819_SV_BACKEND_H

#define SV_FIFO_NAME "sv_fifo"

/** @brief Define que a instrução de mostrar o stock e o preço tem o ID 1. */
#define SV_INSTRUCTION_MOSTRAR_STOCK_E_PRECO (1)
/** @brief Define que a instrução de atualizar o stock e mostrar o novo stock tem o ID 2. */
#define SV_INSTRUCTION_ATUALIZAR_STOCK_E_MOSTRAR_NOVO_STOCK (2)
/** @brief Define que a instrução de correr o agregador tem o ID 3. */
#define SV_INSTRUCTION_EXECUTAR_AG (3)

int send_instruction(int instruction, void **params) {


    // Sucesso!
    return 0;
}

#endif //SO1819_SV_BACKEND_H
