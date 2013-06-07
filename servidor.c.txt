/* Diego Aron Poplade - GRR20101352
 * Gustavo Toshi Komura - GRR20102342
 * Prof. Elias P. Duarte Jr.
   Servidor TCP Iterativo    */

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "log.h"
#define TAMFILA      5
#define MAXNOMEHOST 30
#define NOME "qual_seu_nome_?"
#define IDADE "qual_sua_idade_?"
#define CALCULAR "voce_pode_calcular_?"
#define SOMA "quanto_e_soma_"
#define SUBTRACAO "quanto_e_subtracao_"
#define DIVISAO "quanto_e_divisao_"
#define MULTIPLICACAO "quanto_e_multiplicacao_"
#define PERGUNTA_INXESITENTE "inexistente"
#define RNOME "Joao"
#define RIDADE "23"
#define RCALCULAR "Sim"
#define RSOMA "Pensando_no_valor_da_soma"
#define RSUBTRACAO "Pensando_no_valor_da_subtracao"
#define RDIVISAO "Pensando_no_valor_da_divisao"
#define RMULTIPLICACAO "Pensando_no_valor_da_multiplicacao"
#define RPERGUNTA_INXESITENTE "pergunta_inexistente"
#define INEXISTENTE "Opcao_inexistente"

void boot(char *argv[]); 						//Função de inicialização do servidor.
char *strcopy(char *dest, const char *src, size_t st, size_t n);
char *Answer_Question(char *ask, char *buffer_answer);
void Finda_a_b(char *ask, int *a, int *b, int i);

int sock_descr;
char str[BUFSIZ + 1]; 								// String temporária

int main (int argc, char *argv[])
{
		char NomeHost[MAXNOMEHOST];
		char buffer[BUFSIZ + 1];
		char *ask = malloc(sizeof(char)*(BUFSIZ + 1));
		char *buffer_answer = malloc(sizeof(char)*(BUFSIZ + 1));

		unsigned int aux;

		int i; 													// Contador
		int sock_escuta, sock_atende;

		struct sockaddr_in EnderecLocal, EnderecClient;
		struct hostent *RegistroDNS;

		time_t day; 										// Varíavel para guardar o dia e o horário corrente


		if (argc != 4)
		{
				puts("Uso correto: ./servidor <nome-dispatcher> <porta-dispatcher> <porta-servidor>");
				exit(1);
		}

		gethostname (NomeHost, MAXNOMEHOST);

		if ((RegistroDNS = gethostbyname(NomeHost)) == NULL)
		{
				puts ("Nao consegui meu proprio IP");
				exit (1);
		}

		EnderecLocal.sin_port = htons(atoi(argv[3]));
		EnderecLocal.sin_family = AF_INET;
		bcopy ((char *) RegistroDNS->h_addr, (char *) &EnderecLocal.sin_addr, RegistroDNS->h_length);

		if ((sock_escuta = socket(AF_INET,SOCK_STREAM,0)) < 0)
		{
				log_printf ("Nao consegui abrir o socket");
				exit ( 1 );
		}

		if (bind(sock_escuta, (struct sockaddr *) &EnderecLocal, sizeof(EnderecLocal)) < 0)
		{
				log_printf ("Nao consegui fazer o bind");
				exit ( 1 );
		}

		boot(argv); 										// Envia mensagem para o dispatcher para entrar no grupo de servidores
		listen (sock_escuta, TAMFILA); 	// Entra em modo servidor

		while (1)
		{
				aux = sizeof(EnderecLocal);
				if ((sock_atende=accept(sock_escuta, (struct sockaddr *) &EnderecClient, &aux))<0)
				{
						log_printf ("Nao consegui fazer conexao");
						exit (1);
				}

				bzero(buffer, BUFSIZ + 1);
				read(sock_atende, buffer, BUFSIZ + 1);

				bzero(str, BUFSIZ + 1);
				sprintf(str, "Mensagem %s", buffer);
				if (strncmp(buffer, "DATA", 4) == 0) // Atende se o cliente fez uma requisição
				{
						//strcpy(buffer, ":D");
						log_printf("REQUISICAO de DATA");
						day = time(NULL);
						sprintf(buffer, "%s", ctime(&day)); // Prepara a mensagem
						bzero(str, BUFSIZ + 1);
						log_printf("RESPOSTA");
						sprintf(str, "Mensagem %s", buffer);
						log_printf(str);
						write(sock_atende, buffer, strlen(buffer)); // Envia a mensagem para o dispatcher
				}
				if (strncmp(buffer, "PERGUNTA", 3) == 0) // Atende se o cliente fez uma requisição
				{
						//strcpy(buffer, ":D");
						log_printf("REQUISICAO de PERGUNTA");
						strcopy(ask, buffer, 9, strlen(buffer));
						bzero(buffer_answer, BUFSIZ + 1);
						Answer_Question(ask, buffer_answer);
						bzero(ask, BUFSIZ + 1);
						write(sock_atende, buffer_answer, strlen(buffer_answer)); // Envia a mensagem para o dispatcher
				}
				if (strncmp(buffer, "FALHA", 4) == 0) // Atende se o cliente fez uma requisição
				{
						//strcpy(buffer, ":D");
						log_printf("REQUICAO de FALHA");
                        log_printf("RESPOSTA");
                        bzero(str, BUFSIZ + 1);
                        sprintf(str, "%s", INEXISTENTE);
                        log_printf(str);
						write(sock_atende, INEXISTENTE, BUFSIZ + 1); // Envia a mensagem para o dispatcher
				}
				close(sock_atende);
		}
}

void boot(char *argv[])
{
	    struct sockaddr_in EnderecRemoto;
	    struct hostent *RegistroDNS;
	    char buffer[BUFSIZ+1];
	    char *NomeHost;
	    char *dados;
	    char *port;

        NomeHost = argv[1];
        dados = "JOIN";
        port = argv[3];

	    if((RegistroDNS = gethostbyname(NomeHost)) == NULL)
		{
      	        log_printf("Nao consegui obter endereco IP do dispatcher.");
      	        exit(1);
        }

        bcopy((char *)RegistroDNS->h_addr, (char *)&EnderecRemoto.sin_addr, RegistroDNS->h_length);
        EnderecRemoto.sin_family = AF_INET;
        EnderecRemoto.sin_port = htons(atoi(argv[2]));

        if((sock_descr=socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
      	        log_printf("Nao consegui abrir o socket.");
      	        exit(1);
        }

        if(connect(sock_descr, (struct sockaddr *) &EnderecRemoto, sizeof(EnderecRemoto)) < 0)
		{
      	        log_printf("Nao consegui conectar ao dispatcher");
      	        exit(1);
        }

        if(write(sock_descr, dados, strlen(dados)) != strlen(dados))
		{
      	        log_printf("Nao consegui mandar os dados");
      	        exit(1);
        }

        if(write(sock_descr, port, strlen(dados)) != strlen(dados))
		{
      	        log_printf("Nao consegui mandar a porta");
      	        exit(1);
        }

		bzero(buffer, BUFSIZ + 1);
		read(sock_descr, buffer, BUFSIZ + 1);

        if (strcmp(buffer, "ACCEPTED") == 0) // Mensagem que o dispatcher envia quando a requisição de entrada no grupo de servidores foi aceita
		{
				log_printf("Conexão com o Dispatcher aceita");
                printf("\n");
		}

		if (strcmp(buffer, "LIST_FULL") == 0) // Mensagem que o dispatcher envia quando a lista de servidores do grupo está cheia
		{
				log_printf("Dispatcher com a lista cheia");
				log_printf("Encerrando servidor");
				exit(1);
		}

   	    close(sock_descr);
}

char *strcopy(char *dest, const char *src, size_t st, size_t n)
{
		size_t i;
		size_t j = 0;

		for (i = st ; i < n && src[i] != '\0' ; i++)
		{
					dest[j] = src[i];
					j++;
		}
		for ( ; i < n ; i++)
		{
					dest[j] = '\0';
		}
}

char *Answer_Question(char *ask, char *buffer_answer)
{
        int a, b, result;

 		if (strncmp(ask, NOME, strlen(NOME)) == 0)
		{
				strcpy(buffer_answer, RNOME);
				bzero(str, BUFSIZ + 1);
				sprintf(str, "PERGUNTA: %s", NOME);
				log_printf(str);
				bzero(str, BUFSIZ + 1);
				sprintf(str, "RESPOSTA: %s\n", RNOME);
				log_printf(str);
		}
 		if (strncmp(ask, IDADE, strlen(IDADE)) == 0)
		{
				strcpy(buffer_answer, RIDADE);
				bzero(str, BUFSIZ + 1);
				sprintf(str, "PERGUNTA: %s", IDADE);
				log_printf(str);
				bzero(str, BUFSIZ + 1);
				sprintf(str, "RESPOSTA: %s\n", RIDADE);
				log_printf(str);
		}
 		if (strncmp(ask, CALCULAR, strlen(CALCULAR)) == 0)
		{
				strcpy(buffer_answer, RCALCULAR);
				bzero(str, BUFSIZ + 1);
				sprintf(str, "PERGUNTA: %s", CALCULAR);
				log_printf(str);
				bzero(str, BUFSIZ + 1);
				sprintf(str, "RESPOSTA: %s\n", RCALCULAR);
				log_printf(str);
		}
 		if (strncmp(ask, SOMA, strlen(SOMA)) == 0)
		{
	            Finda_a_b(ask, &a, &b, strlen(SOMA));
   	   	        result = a + b;
   			    bzero(str, BUFSIZ + 1);
				sprintf(str,"%d", result);
			    strcpy(buffer_answer, str);
				bzero(str, BUFSIZ + 1);
				sprintf(str, "PERGUNTA: %s", SOMA);
				log_printf(str);
				bzero(str, BUFSIZ + 1);
				sprintf(str, "RESPOSTA: %s\n", buffer_answer);
				log_printf(str);
		}
 		if (strncmp(ask, SUBTRACAO, strlen(SUBTRACAO)) == 0)
		{
                Finda_a_b(ask, &a, &b, strlen(SUBTRACAO));
   	   	        result = a - b;
   			    bzero(str, BUFSIZ + 1);
				sprintf(str,"%d", result);
				strcpy(buffer_answer, str);
				bzero(str, BUFSIZ + 1);
				sprintf(str, "PERGUNTA: %s", SUBTRACAO);
				log_printf(str);
				bzero(str, BUFSIZ + 1);
				sprintf(str, "RESPOSTA: %s\n", buffer_answer);
				log_printf(str);
		}
 		if (strncmp(ask, DIVISAO, strlen(DIVISAO)) == 0)
		{
                Finda_a_b(ask, &a, &b, strlen(DIVISAO));
   	   	        result = a / b;
   			    bzero(str, BUFSIZ + 1);
				sprintf(str,"%d", result);
				strcpy(buffer_answer, str);
				bzero(str, BUFSIZ + 1);
				sprintf(str, "PERGUNTA: %s", DIVISAO);
				log_printf(str);
				bzero(str, BUFSIZ + 1);
				sprintf(str, "RESPOSTA: %s\n", buffer_answer);
				log_printf(str);
		}
 		if (strncmp(ask, MULTIPLICACAO, strlen(MULTIPLICACAO)) == 0)
		{
                Finda_a_b(ask, &a, &b, strlen(MULTIPLICACAO));
   	   	        result = a * b;
			    bzero(str, BUFSIZ + 1);
				sprintf(str,"%d", result);
				strcpy(buffer_answer, str);
				bzero(str, BUFSIZ + 1);
				sprintf(str, "PERGUNTA: %s", MULTIPLICACAO);
				log_printf(str);
				bzero(str, BUFSIZ + 1);
				sprintf(str, "RESPOSTA: %s\n", buffer_answer);
				log_printf(str);
		}
 		if (strncmp(ask, PERGUNTA_INXESITENTE, strlen(PERGUNTA_INXESITENTE)) == 0)
		{
				strcpy(buffer_answer, RPERGUNTA_INXESITENTE);
				bzero(str, BUFSIZ + 1);
				sprintf(str, "PERGUNTA: %s", PERGUNTA_INXESITENTE);
				log_printf(str);
				bzero(str, BUFSIZ + 1);
				sprintf(str, "RESPOSTA: %s\n", RPERGUNTA_INXESITENTE);
				log_printf(str);
		}

		return buffer_answer;
}


void Finda_a_b(char *ask, int *a, int *b, int i)
{
        char aux[BUFSIZ + 1];

        int j = 0;

        bzero(aux, BUFSIZ + 1);
	    while (ask[i] != '_')
        {
                aux[j] = ask[i];
                i++;
                j++;
        }

        *a = atoi(aux);

        i++;
        j = 0;
        bzero(aux, BUFSIZ + 1);
	    while (ask[i] != '_' && ask[i] != '\0')
        {
                aux[j] = ask[i];
                i++;
                j++;
        }

        *b = atoi(aux);
}
