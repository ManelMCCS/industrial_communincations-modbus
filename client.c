#include "modbusAP.h"
#include "modbusTCP.h"

#include "stdint.h"

#define N_REGISTERS_TO_READ 2
#define PORT 502

// probalby the port should be 5502

int main(void)
{
    uint16_t registers [N_REGISTERS_TO_READ];

    registers[0] = 5;
    registers[1] = 63;

    if (Read_h_regs("127.0.0.1", PORT, 0, N_REGISTERS_TO_READ, registers) < 0)
    {
        printf("Falied to read the holding registers\n");
        return -1;
    }

    uint16_t soma;
    soma = registers[0] + registers[1];
    uint16_t starting_register = 1;

    int reg_wri = Write_multiple_regs("127.0.0.1", PORT, starting_register, 1, &soma);
    if (reg_wri < 0)
        return -1;

    return 0;
}
