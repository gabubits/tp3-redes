#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

int main(int argc, char** argv) {
    /*
    argv[1] = host
    argv[2] = porta
    argv[3] = nome_arq
    argv[4] = tam_buffer
    */

    if (argc != 5) {
        printf("[-] Entrada Invalida - %d argumentos passados", argc);
        return 0;
    }

    // Processa argumentos
    char* host = (char*) malloc(strlen(argv[1]) * sizeof(char));
    strcpy(host, argv[1]);
    int porta = atoi(argv[2]);
    char* nome_arq = (char*) malloc(strlen(argv[3]) * sizeof(char));
    strcpy(nome_arq, argv[3]);
    int tam_buffer = atoi(argv[4]);

    struct timeval inicio, fim;

    int skt_cliente;
    struct sockaddr_in servidor;

    char* buffer = (char*) malloc((tam_buffer + 1) * sizeof(char));
    bzero(buffer, tam_buffer + 1);
    int bytes_recebidos;
    int bytes_totais_traf = 0;

    // Verifica a existencia da pasta "resultados" no diretorio
    // principal. Esse diretorio eh utilizado para armazenar
    // os arquivos .txt resultantes da transferencia
    // entre cliente e servidor. Se a pasta nao existir,
    // ela eh criada.
    struct stat pasta_res = {0};
    if (stat("resultados", &pasta_res) < 0)
        mkdir("resultados", 0700);
    
    // Arquivo .csv que ira guardar informacoes relevantes ao trabalho
    // para auxiliar o processo de analise. O arquivo .csv tera, para cada execucao,
    // o tamanho do buffer, o tamanho do arquivo, em MB, a velocidade de transmissao,
    // a quantidade total de bytes recebidos do servidor e
    // o tempo decorrido naquela execução.
    int tabela_existe = access("tabela_de_res.csv", F_OK);
    FILE* tabela_de_res = fopen("tabela_de_res.csv", "a+");

    if (tabela_existe < 0)
        fprintf(tabela_de_res, "Tamanho do arquivo (MB),Tamanho do buffer (bytes),Taxa de transferencia (kBps),Bytes recebidos,Tempo (s)\n");
    
    // Inicia contagem de tempo inicial
    gettimeofday(&inicio, NULL);

    // Realiza o processo padrao de conexao com o servidor
    // atraves da arquitetura cliente-servidor TCP.
    skt_cliente = socket(AF_INET, SOCK_STREAM, 0);
    if (skt_cliente == -1) {
        puts("[-] Falha em criar o socket do cliente!");
        return 0;
    }
    puts("[+] Socket do cliente criado!");

    servidor.sin_addr.s_addr = inet_addr(host);
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(porta);

    if (connect(skt_cliente, (struct sockaddr*)&servidor, sizeof(servidor)) < 0) {
        puts("[-] Falha na conexao com o servidor!");
        return 0;
    }

    // Envia ao servidor o nome do arquivo requisitado
    char* arquivo_req = (char*) malloc(50 * sizeof(char));
    strcpy(arquivo_req, "arquivos/");
    strcat(arquivo_req,  nome_arq);
    puts("[+] Conexao com o servidor bem sucedida!");
    send(skt_cliente, arquivo_req, strlen(arquivo_req), 0);
    puts("[+] Nome do arquivo enviado com sucesso!");

    // Abre arquivo que vai ser gravado
    // O nome do arquivo segue o padrao:
    // {nome_arq sem .txt}_res.txt.
    // Alem disso, o arquivo sera guardado na pasta resultados,
    // entao eh necessario se certificar de que a pasta
    // resultados esteja criada.
    char* arq_resultado = (char*) malloc(100 * sizeof(char));
    char* nome_arq_formt = (char*) malloc(50 * sizeof(char));
    strcpy(nome_arq_formt, nome_arq);
    nome_arq_formt[strlen(nome_arq) - 4] = '\0';
    sprintf(arq_resultado, "resultados/%s_res.txt", nome_arq_formt);

    // Se quiser salvar um arquivo para cada execucao e eliminar a 
    // substitucao, retire o comentario da linha abaixo e comente
    // a linha de cima.

    //sprintf(arq_resultado, "resultados/%s_res%d.txt", nome_arq_formt, tam_buffer);
    FILE* arquivo = fopen(arq_resultado, "w");
    if (arquivo == NULL) {
        puts("[-] Falha na abertura do arquivo!");
        printf("\tIsso esta correto? %s\n", arquivo_req);
        return 0;
    }
    free(arquivo_req); arquivo_req = NULL;
    free(arq_resultado); arq_resultado = NULL;
    free(nome_arq_formt); nome_arq_formt = NULL;

    // Inicia o processo de transferencia do arquivo requisitado
    // pelo cliente atraves do servidor. A cada recepcao de tama-
    // nho tam_buffer, a quantidade de bytes totais eh atualizada.
    int i;
    while (1) {
        bytes_recebidos = recv(skt_cliente, buffer, tam_buffer + 1, 0);
        if (bytes_recebidos <= 0) break;
        bytes_totais_traf += bytes_recebidos;
        fwrite(buffer, sizeof(char), bytes_recebidos, arquivo);
        bzero(buffer, tam_buffer + 1);
    }

    puts("[+] Recebido o arquivo do servidor com sucesso! (resultado.txt)");
    
    // Encerra a conexao do servidor e do cliente;
    // Alem disso, encerra a contagem para calcular o tempo final.
    close(skt_cliente); fclose(arquivo);
    gettimeofday(&fim, NULL);
    
    double tempo = (fim.tv_sec - inicio.tv_sec) + 1e-6 * (fim.tv_usec - inicio.tv_usec);

    // Grava no arquivo .csv as informacoes necessarias.
    fprintf(tabela_de_res, "%d,%u,%.2f,%d,%.8f\n",
            bytes_totais_traf / (1024 * 1024),
            tam_buffer, 
            bytes_totais_traf / (tempo * 1024), 
            bytes_totais_traf, tempo);

    // Imprime o resultado ao cliente usuario
    puts("\nInformacoes do cliente:");
    printf("\tTamanho do arquivo: %d MB\n\tBuffer = %u byte(s)\n\t%.2f kBps\n\t%d bytes recebidos em %.8f s\n",
            bytes_totais_traf / (1024 * 1024),
            tam_buffer, 
            bytes_totais_traf / (tempo * 1024),
            bytes_totais_traf, tempo);

    fclose(tabela_de_res);
    free(host); host = NULL;
    free(nome_arq); nome_arq = NULL;
    return 0;
}