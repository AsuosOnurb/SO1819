#ifndef SO1819_SV_PROTOCOL_H
#define SO1819_SV_PROTOCOL_H

#include <stddef.h>
#include "fdb.h"

/** @brief Nome da FIFO do servidor. */
#define SV_FIFO_NAME "fifo_sv"

/** @brief Define que a instrução de mostrar o stock e o preço tem o ID 1. */
#define SV_INSTRUCTION_MOSTRAR_STOCK_E_PRECO ((instruction_t) 1)
/** @brief Define que a instrução de atualizar o stock e mostrar o novo stock tem o ID 2. */
#define SV_INSTRUCTION_ATUALIZAR_STOCK_E_MOSTRAR_NOVO_STOCK ((instruction_t) 2)
/** @brief Define que a instrução de correr o agregador tem o ID 3. */
#define SV_INSTRUCTION_EXECUTAR_AG ((instruction_t) 3)

/** @brief Define o tipo de uma instrução a enviar ao servidor, para este ser consistente entre todos os que necessitam dele. */
typedef char instruction_t;

/** @brief Fdbuffer de leitura do FIFO do servidor. */
extern fdb_t g_pFdbServerFifo;

/**
 * @brief Macro de utilizadade para automaticamente e consistentemente calcular o formato e nome do FIFO de resposta de um PID.
 *
 * @param pid Pid do processo que vai receber a resposta
 * @param strPidResposta Onde guardar a string formatada com o nome do FIFO de resposta
 */
#define calcularFifoResposta(p, s) sprintf((s), "fifo_%d", (p))

/**
 * @brief Envia uma instrução ao servidor de vendas.
 *
 * @param instructionCode O ID da instrução a enviar
 * @param params Os parâmetros da instrução a enviar
 * @param dataSize O tamanho do buffer de parâmetros
 * @param currentPid O PID atual, para identificar o nome da FIFO de resposta
 *
 * @return 0 se tudo correu bem; <0 se algo correu mal
 */
int sv_send_instruction(instruction_t instructionCode, const char *params, size_t dataSize, pid_t currentPid);

/**
 * @brief Obtém o stock e o preço de um artigo a partir do servidor.
 *
 * @param codigoArtigo O código do artigo cuja informação pretendemos mostrar
 * @param quantidade Onde guardar a quantidade do artigo
 * @param preco Onde guardar o preço do artigo
 *
 * @return 0 se correr tudo bem; <0 em caso de erro
 */
int sv_get_info_artigo(long codigoArtigo, long *quantidade, double *preco);

/**
 * @brief Pede ao servidor para atualizar o stock de um artigo, retornando o novo stock.
 *
 * @param codigoArtigo O código do artigo cujo stock é para ser atualizado
 * @param acrescento A quantidade a acrescentar ao stock. (Pode ser negativa positiva)
 * @param novoStock Onde guardar o novo stock que o servidor retornou
 *
 * @return 0 se tudo correr bem; <0 em caso de erro
 */
int sv_update_mostra_stock(long codigoArtigo, long acrescento, long *novoStock);

#endif //SO1819_SV_PROTOCOL_H
