#include "modbusAP.h"
#include "modbusTCP.h"

#include "stdint.h"

#define MAX_REGISTERS 125

#define PORT 502
// the port can be 5502

#define PRINT_RESPONSE_CODE(__RESPONSE_CODE__)      \
    {                                               \
        printf("result = %d\n", __RESPONSE_CODE__); \
    }

#define READ_REGS(__IP__, __PORT__, __STARTING__ADDR__, __NUMBER_OF_REGISTERS__, __REGISTERS__)                                   \
    {                                                                                                                             \
        int __read_h_regs_response__ = Read_h_regs(__IP__, __PORT__, __STARTING__ADDR__, __NUMBER_OF_REGISTERS__, __REGISTERS__); \
        if (__read_h_regs_response__ < 0)                                                                                         \
        {                                                                                                                         \
            PRINT_RESPONSE_CODE(__read_h_regs_response__);                                                                        \
            return __read_h_regs_response__;                                                                                      \
        }                                                                                                                         \
    }

#define WRITE_REGS(__IP__, __PORT__, __STARTING__ADDR__, __NUMBER_OF_REGISTERS__, __REGISTERS__)                                           \
    {                                                                                                                                      \
        int __write_h_regs_response__ = Write_multiple_regs(__IP__, __PORT__, __STARTING__ADDR__, __NUMBER_OF_REGISTERS__, __REGISTERS__); \
        if (__write_h_regs_response__ < 0)                                                                                                 \
        {                                                                                                                                  \
            PRINT_RESPONSE_CODE(__write_h_regs_response__);                                                                                \
            return __write_h_regs_response__;                                                                                              \
        }                                                                                                                                  \
    }

int main(void)
{
    uint16_t registers[MAX_REGISTERS] = {};
    // BEGINS HERE

    READ_REGS("127.0.0.1", PORT, 1, 1, registers);
    registers[0] = registers[0] + registers[1] + 1;

    uint16_t starting_register = 1;

    WRITE_REGS("127.0.0.1", PORT, starting_register, 1, registers);

    // ENDS HERE
    printf("%d \n", registers[0]);
    PRINT_RESPONSE_CODE(0);
    return 0;
}
