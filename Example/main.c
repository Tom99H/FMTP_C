#include "../fmtp.h"
#include "../FMTP_COMMANDS/shared_commands.h"
#include "../FMTP_COMMANDS/deviceA_commands.h"
#include <stdio.h>


// Simulate UART handle (can be a FILE* or any custom struct as needed)
void* my_uart_handle = NULL;  // You could point this to a FILE*, socket, or custom object

// Simulated UART write function
int my_uart_write(const uint8_t* data, size_t length, void* user_ctx)
{
    /*Custom implementation of my_uart_write()*/
}

// Simulated UART read function
int my_uart_read(uint8_t* data, size_t length, void* user_ctx)
{
    /*Custom implementation of my_uart_read()*/
}

int main(void)
{
    /* 1) Create an array that can hold all commands from both tables. */
    // Make sure it’s large enough. E.g., 32 entries is just an example:
    static FMTP_CommandEntry combined_table[32];
    size_t offset = 0;

    /* 2) Copy the "shared" table. */
    memcpy(&combined_table[offset], 
        my_command_table, 
        my_command_table_size * sizeof(FMTP_CommandEntry));
    offset += my_command_table_size;

    /* 3) Copy the "deviceA" table. */
    memcpy(&combined_table[offset], 
            deviceA_command_table, 
            deviceA_command_table_size * sizeof(FMTP_CommandEntry));
    offset += deviceA_command_table_size;

    /* Suppose you have a function that writes via UART. */
    FMTP_Transport transport = {
        .write_bytes = my_uart_write,
        .read_bytes  = my_uart_read,
        .user_ctx    = (void*)my_uart_handle
    };

    static FMTP_Context fmtp_ctx;
    fmtp_init(&fmtp_ctx, &transport,
              my_command_table,
              my_command_table_size);

    /* ... your main loop ... when you get new bytes from UART, you do: */
    char incoming_data[32];
    int readcount = my_uart_read(incoming_data, sizeof(incoming_data), my_uart_handle);
    if(readcount > 0)
    {
        fmtp_receive_data(&fmtp_ctx, incoming_data, (size_t)readcount);
    }

    return 0;
}
