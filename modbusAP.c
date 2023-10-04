#include "modbusAP.h"
#include "modbusTCP.h"

#define WRITE_REGISTER_CODE 0x10
#define READ_REGISTER_CODE 0x03


#define READ_APDU_LEN 5 

int Write_multiple_regs(char *server_add, uint16_t port, uint16_t st_r, uint16_t n_r, uint16_t *val)
{
    // check consistency of parameters

    if (server_add == NULL)
        return -1;

    else if (port < port_MIN || port > port_MAX)
        return -1;

    else if (st_r < st_r_MIN || st_r > st_r_MAX)
        return -1;

    else if (n_r < n_r_MIN || n_r > wr_reg_MAX)
        return -1;

    else if (val == NULL)
        return -1;

    // assembles APDU (MODBUS PDU)
    size_t APDUlen = 6 + 2 * n_r;
    uint8_t *APDU = (uint8_t *)malloc(APDUlen);
    uint8_t APDU_R[APDU_MAX];

    APDU[0] = WRITE_REGISTER_CODE;  // function code
    APDU[1] = (uint8_t)(st_r >> 8); // high
    APDU[2] = (uint8_t)st_r;        // low
    APDU[3] = (uint8_t)(n_r >> 8);  // high
    APDU[4] = (uint8_t)n_r;         // low
    APDU[5] = (uint8_t)(2 * n_r);   // numero dos restantes bytes na frame

    // payload build
    for (int i = 0; i < n_r; i++)
    {
        APDU[6 + 2 * i] = (uint8_t)(val[i] >> 8);       // low
        APDU[6 + 2 * i + 1] = (uint8_t)(val[i] & 0xFF); // high
    }
    for (int k = 0; k < 7; k++)
    {
        printf("APDU[%.2x]=%.2x\n", k, APDU[k]);
    }

    printf("PASSOU APDU!\n");

    // send modbus request
    int snd = Send_Modbus_request(server_add, port, APDU, APDUlen, APDU_R);
    if (snd < 0)
    {
        printf("[-] ERRO:Send_Modbus_request\n");
        return -1;
    }

    printf("SENT!\n");

    // response check (APDU_R or error_code)

    if (APDU[0] == APDU_R[0])
    {
        printf("[+] POSITIVE MODBUS CONFIRMATION\n");
        uint16_t reg_wri = (int)((APDU_R[3] << 8) + APDU_R[4]);
    }

    else if (APDU_R[0] = APDU[0] + 0x80) // exception code
    {
        printf("[+] POSITIVE MODBUS CONFIRMATION: exepction code %u\n", APDU_R[1]);
        return APDU[1];
    }

    else
    {
        printf("[-] NEGATIVE MODBUS CONFIRMATION\n");
        return -1;
    }

    printf("PASSOU RESPOSTA!\n");

    free(APDU);

    return 0;
}

int Read_h_regs(char *server_add, uint16_t port, uint16_t st_r, uint16_t n_r, uint16_t *val)
{
    printf("READ\n");
    uint8_t APDU[5], APDU_R[APDU_MAX];
    uint16_t ADUPlen = 5;

    // check consistency of parameters

    if (server_add == NULL)
        return -1;

    else if (port < port_MIN || port > port_MAX)
        return -1;

    else if (st_r < st_r_MIN || st_r > st_r_MAX)
        return -1;

    else if (n_r < n_r_MIN || n_r > rd_reg_MAX)
        return -1;

    else if (val == NULL)
    {
        printf("Failed in parameters check");
        return -1;
    }

    // assembles APDU (MODBUS PDU)

    APDU[0] = READ_REGISTER_CODE;       // function code
    APDU[1] = (uint8_t)(st_r >> 8);     // high
    APDU[2] = (uint8_t)(st_r & 0xFF);   // low
    APDU[3] = (uint8_t)(n_r >> 8);      // high
    APDU[4] = (uint8_t)(n_r & 0xFF);    // low

    printf("PASSOU APDU!\n");

    // send modbus request
    int snr = Send_Modbus_request(server_add, port, APDU, 5, APDU_R);
    if (snr < 0)
    {
        printf("[-] ERRO:Send_Modbus_request\n");
        return -1;
    }
    printf("READ APDU:");
    for (int z = 0; z < 7; z++)
        printf("APDU[%.2x]=%.2x\n", z, APDU[z]);

    printf("SENT!\n");

    // response check (APDU_R or error_code)

    if (APDU[0] == APDU_R[0])
    {
        printf("[+] POSITIVE MODBUS CONFIRMATION\n");

        for (int j = 0; j < APDU_R[1] / 2; j++)
        {
            val[j] = (uint16_t)((APDU_R[2 + 2 * j] << 8) + APDU_R[3 + 2 * j]);
        }
    }

    else if (APDU_R[0] = APDU[0] + 0x80) // exception code
    {
        printf("[+] POSITIVE MODBUS CONFIRMATION: exepction code %u\n", APDU_R[1]);
        return APDU[1];
    }

    else
    {
        printf("[-] NEGATIVE MODBUS CONFIRMATION\n");
        return -1;
    }

    printf("PASSOU RESPOSTA!\n");

    return n_r;
}