#include "modbusAP.h"
#include "modbusTCP.h"

#define WRITE_REGISTER_CODE 0x10
#define READ_REGISTER_CODE 0x03

#define MAX_REG_WRITE (256 / 2 - 5)
// 5 bytes for the APDU header
#define MAX_REG_READ (256 / 2 - 3)
// 3 bytes for the response

#define FIRST_REGISTER 0

#define READ_APDU_LEN 5

#define WRITE_RESPONSE_LEN 3

int Write_multiple_regs(char *server_add, uint16_t port, uint16_t st_r, uint16_t n_r, uint16_t *val)
{
    // check consistency of parameters

    if (server_add == NULL)
    {
        // printf("Invalid server address");
        return -1;
    }
    else if (port < 0 || port > PORT_MAX_N)
    {
        // printf("Invalid port number");
        return -1;
    }
    else if (n_r < 1 || n_r > MAX_REG_WRITE)
    {
        // printf("Invalid number of registers");
        return -1;
    }
    else if (val == NULL)
    {
        // printf("Values to write is NULL");
        return -1;
    }

    st_r -= FIRST_REGISTER;

    // assembles APDU (MODBUS PDU)
    size_t apdu_len = 6 + 2 * n_r;
    uint8_t *apdu = (uint8_t *)malloc(apdu_len);
    uint8_t apdu_response[WRITE_RESPONSE_LEN] = {};

    apdu[0] = WRITE_REGISTER_CODE;  // function code
    apdu[1] = (uint8_t)(st_r >> 8); // high
    apdu[2] = (uint8_t)st_r;        // low
    apdu[3] = (uint8_t)(n_r >> 8);  // high
    apdu[4] = (uint8_t)n_r;         // low
    apdu[5] = (uint8_t)(2 * n_r);   // numero dos restantes bytes na frame

    // payload build
    for (int i = 0; i < n_r; i++)
    {
        apdu[6 + 2 * i] = (uint8_t)(val[i] >> 8);       // low
        apdu[6 + 2 * i + 1] = (uint8_t)(val[i] & 0xFF); // high
    }

    // send modbus request
    int snd = Send_Modbus_request(server_add, port, apdu, apdu_len, apdu_response);
    if (snd < 0)
    {
        // printf("[-] ERRO:Send_Modbus_request\n");
        return -1;
    }

    if (apdu_response[0] == apdu[0] + 0x80) // exception code
    {
        // printf("[+] POSITIVE MODBUS CONFIRMATION: exepction code %u\n", apdu_response[1]);
        return apdu[1];
    }

    else if (apdu_response[0] != apdu[0])
    {
        // printf("[-] NEGATIVE MODBUS CONFIRMATION\n");
        return -1;
    }

    free(apdu);

    return 0;

    // int response = Send_Modbus_request(server_add, port, apdu, apdu_len, apdu_response);

    // free(apdu);

    // return response;
}

// TODO
// falta adicionar frees
int Read_h_regs(char *server_add, uint16_t port, uint16_t st_r, uint16_t n_r, uint16_t *val)
{
    uint8_t APDU[READ_APDU_LEN];
    uint8_t *apdu_response = (uint8_t *)calloc(n_r * 2 + 2, sizeof(uint8_t));

    // check consistency of parameters
    if (server_add == NULL)
    {
        // printf("Invalid server address");
        return -1;
    }
    else if (port < 0 || port > PORT_MAX_N)
    {
        // printf("Invalid port number");
        return -1;
    }
    else if (n_r < 1 || n_r > MAX_REG_WRITE)
    {
        // printf("Invalid number of registers");
        return -1;
    }

    else if (val == NULL)
    {
        // printf("Failed in parameters check");
        return -1;
    }

    // assembles APDU (MODBUS PDU)

    APDU[0] = READ_REGISTER_CODE;     // function code
    APDU[1] = (uint8_t)(st_r >> 8);   // high
    APDU[2] = (uint8_t)(st_r & 0xFF); // low
    APDU[3] = (uint8_t)(n_r >> 8);    // high
    APDU[4] = (uint8_t)(n_r & 0xFF);  // low

    // send modbus request
    if (Send_Modbus_request(server_add, port, APDU, 5, apdu_response) < 0)
    {
        return -1;
    }

    // response check (APDU_R or error_code)

    if (APDU[0] == apdu_response[0])
    {
        // printf("[+] POSITIVE MODBUS CONFIRMATION\n");

        for (int j = 0; j < apdu_response[1] / 2; j++)
        {
            val[j] = (uint16_t)((apdu_response[2 + 2 * j] << 8) + apdu_response[3 + 2 * j]);
        }
    }

    else if (apdu_response[0] == APDU[0] + 0x80) // exception code
    {
        // printf("[+] POSITIVE MODBUS CONFIRMATION: exepction code %u\n", apdu_response[1]);
        return APDU[1];
    }

    else
    {
        // printf("[-] NEGATIVE MODBUS CONFIRMATION\n");
        return -1;
    }

    free(apdu_response);

    return n_r;
}