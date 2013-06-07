/* Diego Aron Poplade - GRR20101352
 * Gustavo Toshi Komura - GRR20102342
 * Prof. Elias P. Duarte Jr.
   Servidor TCP Iterativo    */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "log.h"
#define NOME "qual_seu_nome_?"
#define IDADE "qual_sua_idade_?"
#define CALCULAR "voce_pode_calcular_?"
#define SOMA "quanto_e_soma_"
#define SUBTRACAO "quanto_e_subtracao_"
#define DIVISAO "quanto_e_divisao_"
#define MULTIPLICACAO "quanto_e_multiplicacao_"
#define PERGUNTA_INXESITENTE "inexistente"

char *Choose(char *dados, char *complemento, char *novo_dados);
void flush_in();

char str[BUFSIZ + 1]; 								// String temporária

int main(int argc, char *argv[])
{
		char buffer[BUFSIZ+1];
		char *NomeHost;
		char *dados =(char *)malloc(sizeof(char)*(BUFSIZ + 1));
		char *complemento =(char *)malloc(sizeof(char)*(BUFSIZ + 1));
		char *novo_dados =(char *)malloc(sizeof(char)*(BUFSIZ + 1));

		int i;
		int sock_descr;

		struct sockaddr_in EnderecRemoto;
		struct hostent *RegistroDNS;

		if(argc != 3)
		{
				puts("Uso correto: ./cliente <nome-dispatcher> <porta-dispatcher>");
				exit(1);
		}

		NomeHost = argv[1];

		if((RegistroDNS = gethostbyname(NomeHost)) == NULL)
		{
				puts("Nao consegui obter endereco IP do servidor.");
				exit(1);
		}

		bcopy((char *)RegistroDNS->h_addr, (char *)&EnderecRemoto.sin_addr, 
		RegistroDNS->h_length);
		EnderecRemoto.sin_family = AF_INET;
		EnderecRemoto.sin_port = htons(atoi(argv[2]));

		while (1)
		{
				if((sock_descr=socket(AF_INET, SOCK_STREAM, 0)) < 0)
				{
						puts("Nao consegui abrir o socket.");
						exit(1);
				}

				if(connect(sock_descr, (struct sockaddr *) &EnderecRemoto, sizeof(EnderecRemoto)) < 0)
				{
						puts("Nao consegui conectar ao servidor");
						exit(1);
				}

				bzero(complemento, BUFSIZ + 1);
				bzero(novo_dados, BUFSIZ + 1);
				dados = Choose(dados, complemento, novo_dados);

				printf("\n");
				bzero(str, BUFSIZ + 1);
				sprintf(str, "REQUISCAO: %s", dados);
				log_printf(str);

				if(write(sock_descr, dados, strlen(dados)) != strlen(dados))
				{
						puts("Nao consegui mandar os dados");
						exit(1);
				}

				bzero(buffer, BUFSIZ + 1);
				read(sock_descr, buffer, BUFSIZ + 1);

				bzero(str, BUFSIZ + 1);
				sprintf(str, "RESPOSTA: %s", buffer);
				log_printf(str);
				printf("\n");
		}

		close(sock_descr);
		exit(0);
}

char *Choose(char *dados, char *complemento, char *novo_dados)
{

		char op;
		char *aux = malloc(sizeof(char)*BUFSIZ + 1);
		char *str = malloc(sizeof(char)*BUFSIZ + 1);

		int tam;
		int pergunta_n;
		int a, b;

		printf("---------------------------\n");
		printf("Deseja fazer o que agora ? \n");
		printf("---------------------------\n");
		printf("d - para ver a data\n");
		printf("p - para perguntar algo \n");
		printf("s - sair\n");
		printf("---------------------------\n");
		scanf(" %c", &op);
		flush_in();
		printf("---------------------------\n");

   	    switch (op)
   	    {
    		    case 'd':
       			        printf("você ecolheu ver a data\n");
						strcpy(dados, "DATA");
						return dados;

   			    case 'p':
					    printf("---------------------------\n");
       			        printf("Você escolheu perguntar algo\n");
					    printf("Você tem a opção de perguntar:\n");
					    printf("---------------------------\n");
				    	printf("1 - %s\n", NOME);
					    printf("2 - %s\n", IDADE);
					    printf("3 - %s\n", CALCULAR);
					    printf("4 - %s\n", SOMA);
					    printf("5 - %s\n", SUBTRACAO);
					    printf("6 - %s\n", DIVISAO);
					    printf("7 - %s\n", MULTIPLICACAO);
					    printf("---------------------------\n");
       			        printf("Qual o numero da sua pergunta ?\n");
					    scanf(" %d", &pergunta_n);
					    flush_in();
					    printf("---------------------------\n");
   					    switch (pergunta_n)
   					    {
    						    case 1:
										strcpy(complemento, NOME);
       							        break;
    						    case 2:
										strcpy(complemento, IDADE);
       							        break;
    						    case 3:
										strcpy(complemento, CALCULAR);
       							        break;
    						    case 4:
									    strcpy(complemento, SOMA);
       							        printf("Digite o primeiro valor\n");
									    scanf(" %d", &a);
       							        printf("Digite o segundo valor\n");
									    scanf(" %d", &b);
									    bzero(str, BUFSIZ + 1);
									    sprintf(str,"%d",a);
									    strcpy(aux, str);
									    strcat(aux, "_");
									    bzero(str, BUFSIZ + 1);
									    sprintf(str,"%d",b);
									    strcat(aux, str);
									    strcat(complemento, aux);
       							        break;
    						    case 5:
									    strcpy(complemento, SUBTRACAO);
       							        printf("Digite o primeiro valor\n");
									    scanf(" %d", &a);
       							        printf("Digite o segundo valor\n");
									    scanf(" %d", &b);
									    bzero(str, BUFSIZ + 1);
									    sprintf(str,"%d",a);
									    strcpy(aux, str);
									    strcat(aux, "_");
									    bzero(str, BUFSIZ + 1);
									    sprintf(str,"%d",b);
									    strcat(aux, str);
									    strcat(complemento, aux);
       							        break;
    						    case 6:
									    strcpy(complemento, DIVISAO);
       							        printf("Digite o primeiro valor\n");
									    scanf(" %d", &a);
       							        printf("Digite o segundo valor\n");
									    scanf(" %d", &b);
									    bzero(str, BUFSIZ + 1);
									    sprintf(str,"%d",a);
									    strcpy(aux, str);
									    strcat(aux, "_");
									    bzero(str, BUFSIZ + 1);
									    sprintf(str,"%d",b);
									    strcat(aux, str);
									    strcat(complemento, aux);
       							        break;
    			                case 7:
									    strcpy(complemento, MULTIPLICACAO);
       							        printf("Digite o primeiro valor\n");
									    scanf(" %d", &a);
       							        printf("Digite o segundo valor\n");
									    scanf(" %d", &b);
									    bzero(str, BUFSIZ + 1);
									    sprintf(str,"%d",a);
									    strcpy(aux, str);
									    strcat(aux, "_");
									    bzero(str, BUFSIZ + 1);
									    sprintf(str,"%d",b);
									    strcat(aux, str);
									    strcat(complemento, aux);
       							        break;
							    default:
									    strcpy(complemento, PERGUNTA_INXESITENTE);
										break;
						}

						strcpy(dados, "PERGUNTA ");
						strcat(dados, complemento	);

						return dados;

     		    case 's':
      			        printf("Você escolheu terminar\n");
     				    exit(0);

				default:
       			        printf("Opção inexistente\n");
				        strcpy(dados, "FALHA");
				        return dados;
   	    }

}

void flush_in()
{
        int ch;

        while( (ch = fgetc(stdin)) != EOF && ch != '\n' ){}
}
