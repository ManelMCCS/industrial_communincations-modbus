#include "modbusTCP.h"

#define protocolID 0 // é zero para o modbus
#define UNIT_ID 51
#define APDU_MAX_LEN 128

int Send_Modbus_request(char *server_add, unsigned int port, uint8_t *APDU, uint16_t APDUlen, uint8_t *APDU_R)
{
    // check consistency of parameters

    if (server_add == NULL)
        return -1;

    else if (port < 0 || port > PORT_MAX_N)
        return -1;

    else if (APDU == NULL)
        return -1;

    else if (APDUlen > APDU_MAX_LEN)
        return -1;

    // opens TCP client socket and connects to server

    int Csocket;

    struct sockaddr_in cAD;
    socklen_t cADsize = sizeof(cAD);

    Csocket = socket(PF_INET, SOCK_STREAM, 0);

    if (Csocket < 0)
    {
        return -1;
    }

    cAD.sin_family = AF_INET;
    cAD.sin_port = htons(port);
    inet_aton(server_add, &cAD.sin_addr);

    if (connect(Csocket, (struct sockaddr *)&cAD, cADsize) < 0)
    {
        return -1;
    }
    static uint8_t transaction_identifier = 0;

    transaction_identifier++;
    //  assembles PDU = MBAP+APDU

    unsigned char MBAP[7];

    int len = APDUlen + 1;

    MBAP[0] = (uint8_t)(transaction_identifier >> 8);
    MBAP[1] = (uint8_t)(transaction_identifier & 0xFF);
    MBAP[2] = 0x00;
    MBAP[3] = 0x00;
    MBAP[4] = (uint8_t)(len >> 8);
    MBAP[5] = (uint8_t)(len & 0xFF);
    MBAP[6] = UNIT_ID;

    // escrever MBAP (cabeçalho PDU)
    if (write(Csocket, MBAP, 7) < 0)
    {
        // printf("[-] ERRO NA ESCRITA MBAP\n");
        return -1;
    }

    // escrever APDU (payload PDU)
    if (write(Csocket, APDU, APDUlen) < 0)
    {
        return -1;
    }

    if (read(Csocket, MBAP, 7) < 0)
    {
        return -1;
    }
    // tamanho da resposta
    int response_len = 3;

    // FIX THIS
    // TODO
    if (0x10 != APDU[0])
    {
        response_len = (int)((MBAP[4] << 8) + MBAP[5]);
    }

    if (read(Csocket, APDU_R, response_len - 1) < 0)
    {
        return -1;
    }

    close(Csocket);
    return 0;
}