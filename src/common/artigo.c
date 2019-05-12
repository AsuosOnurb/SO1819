#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "fdb.h"
#include "artigo.h"
#include "util.h"

fdb_t g_pFdbArtigos = NULL;

long g_iProximoCodigoUtilizavel;

// Cache atualmente desativada
// #define DISABLE_CACHE

#define ARTIGOS_CACHE_SIZE (512)
#define ARTIGOS_CACHE_THRESHOLD (10)
#define ARTIGOS_CACHE_START (3 * ARTIGOS_CACHE_THRESHOLD)

typedef struct artigo_cache_entry {
    artigo_t artigo;
    int hits;
} artigo_cache_entry_t;

static artigo_cache_entry_t cacheArtigos[ARTIGOS_CACHE_SIZE];

/**
 * @brief Calcula a posição em cache de um determinado artigo, dado o seu código.<br>
 * Utiliza linear probing para resolver conflitos.<br>
 *
 * @param codigo O código do artigo a procurar a sua hash na cache
 *
 * @return a hash do artigo na cache, que pode não corresponder ao prório artigo se não existir espaço para ele
 */
long hash(long codigo) {
#ifdef DISABLE_CACHE
    return -1;
#else
    long iFirstTimedout = -1;

    for(long i = 0; i < ARTIGOS_CACHE_SIZE; i++) {
        artigo_cache_entry_t *entry = &cacheArtigos[i];

        if(iFirstTimedout == -1 && entry->hits < ARTIGOS_CACHE_THRESHOLD)
            iFirstTimedout = i;

        if(entry->artigo != NULL && entry->artigo->codigo == codigo)
            return i;

        entry->hits--;
    }

    return iFirstTimedout;
#endif
}

void artigo_cache_invalidate(long codigo) {
    long h = hash(codigo);

    if(h >= 0 && cacheArtigos[h].artigo != NULL && cacheArtigos[h].artigo->codigo == codigo) {
        cacheArtigos[h].hits = 0;
        artigo_free(cacheArtigos[h].artigo);

        // fprintf(stderr, "Invalidada a cache do artigo %ld\n", codigo);
    } else {
        // fprintf(stderr, "O artigo %ld não está em cache!\n", codigo);
    }
}

int inicializar_ficheiro_artigos() {
    // Abrir um file descriptor associado ao ficheiro
    if(file_open(&g_pFdbArtigos, NOME_FICHEIRO_ARTIGOS, true, true) != 0)
        return -1;

    // Carregar o código do próximo código utilizável:
    // calculado a partir de ler a última entrada presente no ficheiro
    // e somando uma unidade ao código da última entrada

    // Fazer lseek para o fim do ficheiro
    ssize_t offset = fdb_lseek(g_pFdbArtigos, 0, SEEK_END);
    if(offset < 0)
        return -2;

    // Verificar se o ficheiro acaba de ser inicializado
    if(offset == 0) {
        // O ficheiro acabou de ser inicializado, logo g_iProximoCodigoUtilizavel = 0
        g_iProximoCodigoUtilizavel = 0;
        return 0;
    }

    // Calcular o offset da última entrada
    offset -= TAMANHO_ENTRADA_ARTIGO;

    // Calcular o código a partir do offset da última entrada
    g_iProximoCodigoUtilizavel = (long) ((offset - INICIO_ENTRADAS_ARTIGO) / TAMANHO_ENTRADA_ARTIGO);

    // O próximo código utilizável é o código da última entrada no disco somado de uma unidade
    g_iProximoCodigoUtilizavel++;

    // Sucesso!
    return 0;
}

artigo_t artigo_new(long codigo, ssize_t offsetNome, double preco) {
    // Criar a estrutura com o artigo
    artigo_t artigo = (artigo_t) malloc(sizeof(struct artigo));

    if(codigo == -1)
        artigo->offset = -1;
    else
        artigo->offset = TAMANHO_ENTRADA_ARTIGO * codigo + INICIO_ENTRADAS_ARTIGO;

    artigo->codigo = codigo;
    // artigo->nome = nome;
    artigo->offsetNome = offsetNome;
    artigo->preco = preco;

    return artigo;
}

void artigo_free(artigo_t artigo) {
    // Verificar parâmetros
    if(artigo == NULL)
        return;

    // Não eliminar artigos que estejam em cache e que esta ainda não tenha expirado
    time_t the_time;
    time(&the_time);

    long h = hash(artigo->codigo);
    if(h >= 0 && cacheArtigos[h].artigo != NULL && cacheArtigos[h].artigo->codigo == artigo->codigo) {
        // este artigo ainda está em cache, verificar se jaá expirou!
        if(cacheArtigos[h].hits < ARTIGOS_CACHE_THRESHOLD) {
            // O artigo pode ser eliminado da cache
            cacheArtigos[h].artigo = NULL;
            cacheArtigos[h].hits = 0;
        } else return; // Don't let the item be freed
    }

    // Libertar a memória do artigo
    free(artigo);
}

int artigo_load(long codigo, artigo_t *artigoRef) {
    // Verificar parâmetros
    if(codigo < 0)
        return -1;

    if(artigoRef == NULL)
        return -2;

    // Verificar se o artigo já está em cache
    long h = hash(codigo);
    if(h >= 0 && cacheArtigos[h].artigo != NULL && cacheArtigos[h].artigo->codigo == codigo) {
        // Cache hit: dar refresh no artigo na cache
        *artigoRef = cacheArtigos[h].artigo;
        cacheArtigos[h].hits++;

        return 0;
    }

    // Verificar se o ficheiro está aberto
    if(g_pFdbArtigos == NULL)
        if(inicializar_ficheiro_artigos() != 0)
            return -3;

    // Verificar se o código gera um offset válido
    ssize_t offset = TAMANHO_ENTRADA_ARTIGO * codigo + INICIO_ENTRADAS_ARTIGO;

    // Mover o apontador para a entrada correta para escrita
    if(fdb_lseek(g_pFdbArtigos, offset, SEEK_SET) != offset)
        return -4;

    // Criar uma referência para o artigo
    artigo_t artigo = artigo_new(codigo, -1, -1);

    // Carregar os vários membros do artigo

    /*
    // Carregar codigo
    if(fdb_read(g_pFdbArtigos, &artigo->codigo, sizeof(artigo->codigo)) <= 0) {
        artigo_free(artigo);
        return -5;
    }

    if(codigo != artigo->codigo) {
        // O codigo lido não corresponde ao código fornecido!
        // Logo alguma coisa está errada
        artigo_free(artigo);
        return -6;
    }
    */

    // Carregar offset do nome
    if(fdb_read(g_pFdbArtigos, &artigo->offsetNome, sizeof(artigo->offsetNome)) <= 0) {
        artigo_free(artigo);
        return -7;
    }

    // Carregar preço
    if(fdb_read(g_pFdbArtigos, &artigo->preco, sizeof(artigo->preco)) <= 0) {
        artigo_free(artigo);
        return -8;
    }

    // Sucesso!
    *artigoRef = artigo;

    // Colocar o artigo em cache
    if(h >= 0) {
        cacheArtigos[h].artigo = artigo;
        cacheArtigos[h].hits = ARTIGOS_CACHE_START;
        // fprintf(stderr, "Colocado em cache o artigo %ld na posição %ld\n", codigo, h);
    }
    return 0;
}

int artigo_save(artigo_t artigo) {
    // Verificar parâmetros
    if(artigo == NULL)
        return -1;

    // Verificar se o ficheiro está aberto
    if(g_pFdbArtigos == NULL)
        if(inicializar_ficheiro_artigos() != 0)
            return -2;

    // Verificar se o código do artigo é válido (ou seja, se este artigo já existe no ficheiro)
    if(artigo->codigo == -1) {
        // Atribuir um novo código
        artigo->codigo = g_iProximoCodigoUtilizavel++;
    }

    // Verificar se o offset do artigo é válido
    // caso não seja válido, calcular a partir do código
    if(artigo->offset == -1) {
        // O artigo não existe no ficheiro,
        // então temos de arranjar um novo offset, logo, fazer lseek(..., SEEK_END)
        artigo->offset = TAMANHO_ENTRADA_ARTIGO * artigo->codigo + INICIO_ENTRADAS_ARTIGO;
    }

    // Mover o apontador para a entrada correta para escrita
    if(fdb_lseek(g_pFdbArtigos, artigo->offset, SEEK_SET) != artigo->offset)
        return -5;

    // Efetivamente escrever o artigo no ficheiro
    // O formato de uma entrada deverá ser: codigo, endereço do nome, preco por unidade

    /*
    // Escrever código
    if(fdb_write(g_pFdbArtigos, &artigo->codigo, sizeof(artigo->codigo)) != 0)
        return -6;
    */
    
    // Escrever endereço do nome
    if(fdb_write(g_pFdbArtigos, &artigo->offsetNome, sizeof(artigo->offsetNome)) != 0)
        return -7;

    // Escrever preço por unidade
    if(fdb_write(g_pFdbArtigos, &artigo->preco, sizeof(artigo->preco)) != 0)
        return -8;

    // Sucesso!
    return 0;
}
