#include "modbusAP.h"
#include "modbusTCP.h"

#define protocolID 0 // é zero para o modbus
#define unitID 1

uint8_t transID;

int Send_Modbus_request(char *server_add, unsigned int port, uint8_t *APDU, uint16_t APDUlen, uint8_t *APDU_R)
{
    printf("SEND!\n");

    // uint8_t PDU[255], MBAP_R[7];
    int len = 0, i = 0, PDUlen = 0;

    // check consistency of parameters

    if (server_add == NULL)
        return -1;

    else if (port < port_MIN || port > port_MAX)
        return -1;

    else if (APDU == NULL)
        return -1;

    else if (APDUlen > APDU_MAX)
        return -1;

    // opens TCP client socket and connects to server

    int Csocket;

    struct sockaddr_in cAD;
    socklen_t cADsize = sizeof(cAD);

    Csocket = socket(PF_INET, SOCK_STREAM, 0);

    if (Csocket < 0)
    {
        printf("[-] ERRO A CRIAR O SOCKET\n");
        return -1;
    }

    cAD.sin_family = AF_INET;
    cAD.sin_port = htons(port);
    inet_aton(server_add, &cAD.sin_addr);

    if (connect(Csocket, (struct sockaddr *)&cAD, cADsize) < 0)
    {
        printf("Failed to connect\n");
        return -1;
    }
    printf("Successful connection\n");
    // generates TI (trans.ID →sequence number)

    transID++;
    // printf("TRANS ID: %d", (transID>>8));
    //  assembles PDU = MBAP+APDU

    unsigned char MBAP[7];

    len = APDUlen + 1;

    MBAP[0] = (uint8_t)(transID >> 8);
    MBAP[1] = (uint8_t)(transID & 0xFF);
    MBAP[2] = 0x00;
    MBAP[3] = 0x00;
    MBAP[4] = (uint8_t)(len >> 8);
    MBAP[5] = (uint8_t)(len & 0xFF);
    MBAP[6] = unitID;

    printf("[TCP]: MBAP\n");
    for (int k = 0; k < 7; k++)
        printf("MBAP[%.2x]=%.2x\n", k, MBAP[k]);
    printf("\n");

    // escrever MBAP (cabeçalho PDU)
    int w_mbap = write(Csocket, MBAP, 7);
    if (w_mbap < 0)
    {
        printf("[-] ERRO NA ESCRITA MBAP\n");
        return -1;
    }

    // escrever APDU (payload PDU)
    int w_apdu = write(Csocket, APDU, APDUlen);
    if (w_apdu < 0)
    {
        printf("[-] ERRO NA ESCRITA APDU\n");
        return -1;
    }

    printf("[TCP] Message send to slave: ");
    for (int k = 0; k < 7; k++)
        printf("MBAP[%.2x]=%.2x\n", k, MBAP[k]);
    for (int k = 0; k < APDUlen; k++)
        printf("APDU[%.2x]=%.2x\n", k, APDU[k]);
    printf("\n");
    // if response, remove MBAP, PDU_R → APDU_R

    int r_mbap = read(Csocket, MBAP, 7);
    if (r_mbap < 0)
    {
        printf("[-] ERRO NA LEITURA(MBAP)\n");
        return -1;
    }
    printf("[TCP] Message received from slave:");
    for (int k = 0; k < 7; k++)
        printf("MBAP[%.2x]=%.2x\n", k, MBAP[k]);

    // tamanho da resposta
    static int lenR = 0;

    lenR = (int)((MBAP[4] << 8) + MBAP[5]);

    int ra = read(Csocket, APDU_R, lenR - 1);
    if (ra < 0)
    {
        printf("[-] ERRO NA LEITURA(APDU)\n");
        return -1;
    }

    printf("[TCP] Message received from slave:");
    for (int k = 0; k < 7; k++)
        printf("MBAP[%.2x]=%.2x\n", k, MBAP[k]);
    for (int q = 0; q < lenR; q++)
        printf("APDU_R[%.2x]=%.2x\n", q, APDU_R[q]);
    printf("\n");

    // closes TCP client socket with server

    close(Csocket);
    return 0;
}