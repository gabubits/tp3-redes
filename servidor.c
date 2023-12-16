#include <stdio.h>  
#include <string.h>
#include <strings.h>
#include <stdlib.h>  
#include <errno.h>  
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>  
#include <sys/socket.h>
#include <netinet/in.h>  
#include <sys/time.h>
#include <time.h>

int main(int argc, char** argv) {
    /*
    argv[1] = porta
    argv[2] = tam_buffer
    */

    if (argc != 3) {
        puts("[-] Entrada Invalida!");
        return 0;
    }

    // Processa argumentos
    int porta = atoi(argv[1]);
    int tam_buffer = atoi(argv[2]);

    struct timeval inicio, fim;
    char* buffer = (char*) malloc((tam_buffer + 2) * sizeof(char));
    bzero(buffer, tam_buffer + 1);
    int skt_servidor;
    struct sockaddr_in endereco;
    int bytes_totais_traf = 0, bytes_enviados;
    char* arquivo_req = (char*) malloc(100 * sizeof(char));

    // Realiza o processo padrao de abertura de conexao do servidor
    // atraves da arquitetura cliente-servidor TCP.
    if ((skt_servidor = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        puts("[-] Criacao do socket do servidor falhou!");
        return 0;
    }
    puts("[+] Servidor inicializado!");

    if (setsockopt(skt_servidor, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
        puts("[-] Erro na funcao setsockopt!");
        return 0;
    }

    endereco.sin_family = AF_INET;
    endereco.sin_addr.s_addr = INADDR_ANY;
    endereco.sin_port = htons(porta);

    if (bind(skt_servidor, (struct sockaddr*)&endereco, sizeof(endereco)) < 0) {
		puts("[-] Servidor: Bind falhou!");
		return 0;
	}
    puts("[+] Servidor: Bind feito com sucesso!");

    if(listen(skt_servidor, 3) < 0){
		perror("[-] Servidor: falha na funcao de listen");
		return 0;
	}

    struct sockaddr_in endereco_cliente;
    int tam_end;
    int skt_cliente = accept(skt_servidor, (struct sockaddr*) &endereco_cliente, (socklen_t*)&tam_end);
    puts("[+] Cliente conectado com sucesso!");
    
    gettimeofday(&inicio, NULL);

    // Recebe a string com o nome do arquivo requisitado
    // pelo lado do cliente.
    recv(skt_cliente, arquivo_req, 100 * sizeof(char), 0);
    
    // Realiza a abertura do arquivo requisitado.
    FILE* arq = fopen(arquivo_req, "r");
    if (arq == NULL) {
        puts("[-] Falha na abertura do arquivo requisitado pelo cliente!");
        printf("\tIsso esta correto? %s\n", arquivo_req);
        close(skt_cliente); close(skt_servidor);
        return 0;
    }
    free(arquivo_req); arquivo_req = NULL;

    // Realiza o loop de leitura do arquivo, lendo tam_buffer bytes
    // do arquivo. Em cada iteracao, são enviados os primeiros tam_buffer
    // bytes do arquivo, que eh a parte interessada.
    // Da mesma forma que no cliente, ha uma variavel que acumula
    // os bytes enviados para exibir no final se todos os
    // bytes foram enviados.
    while(fread(buffer, sizeof(char), tam_buffer, arq) > 0) {
        bytes_enviados = send(skt_cliente, buffer, tam_buffer, 0);
        bytes_totais_traf += bytes_enviados;
        bzero(buffer, tam_buffer + 1);
    }

    puts("[+] Arquivo transferido ao cliente com sucesso!");
    
    // Encerra a conexao do servidor e fecha o arquivo requisitado.
    // Alem disso, encerra a contagem para calcular o tempo final.
    close(skt_servidor);
    fclose(arq);
    gettimeofday(&fim, NULL);

    double tempo = (fim.tv_sec - inicio.tv_sec) + 1e-6 * (fim.tv_usec - inicio.tv_usec);
    
    // Imprime o resultado ao servidor usuario.
    puts("\nInformacoes do servidor:");
    printf("\tTamanho do arquivo: %d MB\n\tBuffer = %u byte(s)\n\t%.2f kBps\n\t%d bytes enviados em %.8f s\n",
            bytes_totais_traf / (1024 * 1024),
            tam_buffer, 
            bytes_totais_traf / (tempo * 1024),
            bytes_totais_traf, tempo);

    return 0;
}