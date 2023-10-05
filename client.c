#include "modbusAP.h"
#include "modbusTCP.h"

#include "stdint.h"

#define N_REGISTERS_TO_READ 2
#define PORT 502

// probalby the port should be 5502

int main(void)
{
    uint16_t registers[N_REGISTERS_TO_READ] = {};

    if (Read_h_regs("127.0.0.1", PORT, 0, 2, registers) < 0)
    {
        printf("Falied to read the holding registers\n");
        return -1;
    }

    uint16_t soma;
    soma = registers[0] + registers[1] + 1;
    uint16_t starting_register = 1;

    int reg_wri = Write_multiple_regs("127.0.0.1", PORT, starting_register, 1, &soma);
    if (reg_wri < 0)
    {
        return -1;
    }

    if (Read_h_regs("127.0.0.1", PORT, 1, 1, registers) < 0)
    {
        printf("Falied to read the holding registers\n");
        return -1;
    }

    printf("Register n%d: %d", starting_register, registers[starting_register]);

    return 0;
}
