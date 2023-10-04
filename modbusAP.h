#ifndef MODBUSAP
#define MODBUSAP

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>


//limites dos parametros
#define port_MIN 0
#define port_MAX 65535
#define st_r_MIN 0
#define st_r_MAX 65536
#define n_r_MIN 1
#define wr_reg_MAX 123
#define rd_reg_MAX 125


#define APDU_MAX 256




//funções

int Write_multiple_regs (char * server_add, uint16_t port, uint16_t st_r, uint16_t n_r, uint16_t* val);
// server_add:  struct que tem o endereço do servidor  
// port:        porta usada para fazer a conexão
// st_r:        endereço inicial dos registos   
// n_r:         número de registos a escrever
// val:         vetor que é passado com os valores a escrever/enviar

int Read_h_regs (char * server_add, uint16_t port, uint16_t st_r, uint16_t n_r, uint16_t* val);
// server_add:  struct que tem o endereço do servidor  
// port:        porta usada para fazer a conexão
// st_r:        endereço inicial dos registos   
// n_r:         número de registos a ler
// val:         vetor que é devolvido com os valores lidos


#endif