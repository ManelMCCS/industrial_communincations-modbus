#ifndef MODBUSTCP
#define MODBUSTCP

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>


int Send_Modbus_request (char * server_add, unsigned int port, uint8_t* APDU, uint16_t APDUlen, uint8_t* APDU_R);
// server_add:  struct que tem o endereço do servidor  
// port:        porta usada para fazer a conexão
// APDU:        mensagem a enviar para o servidor 
// APDUlen:     tamanho da mensagem 
// APDU_R:      mensagem de resposta 


#endif