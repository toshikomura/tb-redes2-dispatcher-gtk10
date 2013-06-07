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
#include <regex.h>
#include "log.h"
#define TAMFILA      5
#define MAXNOMEHOST 30
#define MAX_SERVER_NUMBER 3

int add_server(struct sockaddr_in ip,int port);
void Arrange_list_servers();

typedef struct
{
	struct sockaddr_in ip; 									// Ip do servidor
	int port; 															// Porta do servidor
}server; 																	// Estrutura para a lista de servidores

char str[BUFSIZ + 1]; 								// String temporária

int counter = 0; 													// Contador do número de servidores na lista
int list_i = 0; 													// Índice para o vetor de servidores
int check = 0; 														// Variável de controle

server* server_list[MAX_SERVER_NUMBER]; 	// Lista de servidores

int main (int argc, char *argv[])
{
		char buffer[BUFSIZ + 1];
		char NomeHost[MAXNOMEHOST];

		unsigned int aux;

		int port;
		int i;
		int sock_escuta, sock_atende, sock_temp;

		struct sockaddr_in EnderecLocal, EnderecClient;
		struct hostent *RegistroDNS;

		srand(time(NULL)); 											// Inicializa os números pseudo-aleatórios

		for (i = 0; i < MAX_SERVER_NUMBER; i++)
		{
			server_list[i] = NULL; 								// Limpa a lista de servidores
		}

		if (argc != 3)
		{
				puts("Uso correto: ./dispatcher <porta-dispatcher> <tipo-de-esclha-do-servidor(aleatorio/round-robin)>");
				exit(1);
		}

		log_printf("Dispatcher iniciando");
		gethostname (NomeHost, MAXNOMEHOST);

		if ((RegistroDNS = gethostbyname(NomeHost)) == NULL)
		{
				log_printf ("Nao consegui meu proprio IP");
				exit (1);
		}

		EnderecLocal.sin_port = htons(atoi(argv[1]));
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

		listen (sock_escuta, TAMFILA); 					// Entra em modo servidor

		log_printf("Dispatcher entranado em modo servidor");

		while (1)
		{
				aux = sizeof(EnderecLocal);
				if ((sock_atende=accept(sock_escuta, (struct sockaddr *) &EnderecClient, &aux))<0)
				{
						log_printf ("Nao consegui fazer conexao");
						exit (1);
				}
				sprintf(buffer, "\0");

				bzero(buffer, BUFSIZ);
				read(sock_atende, buffer, BUFSIZ);
				if (strcmp(buffer, "JOIN") == 0) 			// Atende a requisição de um servidor para entrar no grupo de servidores
				{
						log_printf("Servidor requisitando entrada no grupo");

						sprintf(buffer, "\0");

						bzero(buffer, BUFSIZ);
						read(sock_atende, buffer, BUFSIZ);

						port = atoi(buffer);
						if (add_server(EnderecClient, port) == 0) // Adiciona o servidor na lista
						{
								//sprintf(buffer, "%d", ACCEPTED);
								write(sock_atende, "ACCEPTED", BUFSIZ);
						}
						else
						{
								log_printf("Lista de servidores cheia");
								//sprintf(buffer, "%d", LIST_FULL);
								write(sock_atende, "LIST_FULL", BUFSIZ);
						}
				}

				if ((strncmp(buffer, "DATA", 4) == 0) || (strncmp(buffer, "PERGUNTA", 3) == 0) || (strncmp(buffer, "FALHA", 3) == 0)) // Atende uma requisição do cliente
				{
						log_printf("Cliente requisitando algo");
						struct sockaddr_in EnderecRemoto;

						bzero(str, BUFSIZ + 1);
						sprintf(str, "Cliente enviou mensagem, passando mensagem para o servidor %d", list_i);
						log_printf(str);
						check = 0;
						if (counter > 0) // Existe servidores na lista
						{
								while (check == 0) // Controle
								{
										if (strcmp(argv[2], "aleatorio") == 0) // Escolha aleatória
										{
                                                log_printf("Modo aleatorio");
												list_i = rand();
												list_i = list_i % counter; //MAX_SERVER_NUMBER;
										}

										if (strcmp(argv[2], "round-robin") == 0)  // Round-Robin
										{
                                                log_printf("Modo Round-Robin");
												list_i = list_i + 1;
												list_i = list_i % counter; //MAX_SERVER_NUMBER;
										}

										bzero(str, BUFSIZ + 1);
										sprintf(str, "Escolhendo servidor %d\n", list_i);
										log_printf(str);

										if (server_list[list_i] != NULL) // Servidor existe
										{
												check = 1;
												EnderecRemoto = server_list[list_i]->ip;

											    EnderecRemoto.sin_family = AF_INET;
												EnderecRemoto.sin_port = htons(server_list[list_i]->port);

												if((sock_temp=socket(AF_INET, SOCK_STREAM, 0)) < 0)
												{
														log_printf("Nao consegui abrir o socket para comunicar com o servidor");
														exit(1);
												}

												// Se não conseguir conectar com um servidor, remove ele da lista
												if(connect(sock_temp, (struct sockaddr *) &EnderecRemoto, sizeof(EnderecRemoto)) < 0)
												{
														Arrange_list_servers();;
												}
												else
												{
														if (strncmp(buffer, "DATA", 4) == 0)
														{
																log_printf("REQUISICAO de DATA");
																write(sock_temp, buffer, BUFSIZ); // Envia requisição do cliente para o servidor
				 										}
														else
														{
																if (strncmp(buffer, "PERGUNTA", 3) == 0)
																{
																		log_printf("CLIENTE PERGUNTA PARA SERVIDOR");
														                bzero(str, BUFSIZ + 1);
														                sprintf(str, "Mensagem %s \n", buffer);
                                                                        log_printf(str);
																		write(sock_temp, buffer, BUFSIZ); // Envia requisição do cliente para o servidor
																}
																else
																{
																		log_printf("REQUISICAO de FALHA");
																		write(sock_temp, buffer, BUFSIZ); // Envia requisição do cliente para o servidor
																}
														}

														sprintf(buffer, "\0");
														bzero(buffer, BUFSIZ);
														read(sock_temp, buffer, BUFSIZ);
														log_printf("SERVIDOR RESPOSTA PARA CLIENTE");
													    bzero(str, BUFSIZ + 1);
														sprintf(str, "Mensagem %s \n", buffer);
													    log_printf(str);
														write(sock_atende, buffer, BUFSIZ);
												}

												close(sock_temp);
										}
								}
						}
						else
						{
								log_printf("Lista de servidores vazia");
								write(sock_atende, "ERROR", BUFSIZ);
						}
				}

				write(sock_atende, "FALHA", BUFSIZ);
				close(sock_atende);
		}
}

int add_server(struct sockaddr_in ip, int port) // Coloca um servidor na lista de servidores
{
		int i;
	    server *temp;

		if (counter < MAX_SERVER_NUMBER) // Lista não está cheia
		{
				temp = (server*)malloc(sizeof(server));

			    memcpy(&temp->ip, &ip, sizeof(ip));
				temp->port = port;
				for (i = 0; i < MAX_SERVER_NUMBER; i++)
				{
						if (server_list[i] == NULL)
						{
							    server_list[i] = temp;
								break;
						}
				}
				counter = counter + 1;

				return 0; // Conseguiu colocar um servidor na lista
		}

		return 1; // Lista de servidores está cheia
}


void Arrange_list_servers()
{
		int i;

		log_printf("Nao consegui conectar ao servidor");
		for (i = 0; i < counter; i++)
		{
				if (i == list_i)
				{
						for (i = list_i; i < counter; i++)
						{
								server_list[list_i] = server_list[list_i + 1];
						}
						break;
				}
		}
	    counter = counter - 1;
		server_list[counter] = NULL;
		if (counter != 0)
		{
				check = 0;
		}
		else
		{
				log_printf("Nao existem servidores");
		}
}
