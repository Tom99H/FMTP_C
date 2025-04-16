#include "fmtp.h"
#include <string.h>   // for memcpy
#include <stdio.h>    // for debug prints if needed (optional)

/* Example constants from your old code. Adapt as you wish. */
#define FMTP_HEADER_VALUE 0x464D5450U  /* 'FMTP' in ASCII */
#define FMTP_HEADER_SIZE  4
#define FMTP_MAX_PAYLOAD  (FMTP_MAX_BUFFER_SIZE - 10) /* e.g. minus header, etc. */

/* Helper union to interpret 4 bytes as a 32-bit integer for the header. */
typedef union
{
    uint8_t  bytes[4];
    uint32_t value;
} ByteToU32;

/* Internal function prototypes */
static void fmtp_process_packet(FMTP_Context* ctx, 
                                const uint8_t* packet_data, 
                                size_t packet_len);

/* ----------------------------------------------------------------
 * Initialize the FMTP context
 * ---------------------------------------------------------------- */
void fmtp_init(FMTP_Context* ctx, 
               const FMTP_Transport* transport,
               const FMTP_CommandEntry* cmd_table,
               size_t cmd_count)
{
    if(!ctx || !transport) return;

    memset(ctx, 0, sizeof(*ctx));
    ctx->transport = *transport;  // store a copy of the transport
    ctx->command_table = cmd_table;
    ctx->command_count = cmd_count;

    ctx->rx_length = 0;  // nothing in buffer yet
}

/* ----------------------------------------------------------------
 * Optionally set or change the command table
 * ---------------------------------------------------------------- */
void fmtp_set_command_table(FMTP_Context* ctx,
                            const FMTP_CommandEntry* cmd_table,
                            size_t cmd_count)
{
    if(!ctx) return;
    ctx->command_table = cmd_table;
    ctx->command_count = cmd_count;
}

/* ----------------------------------------------------------------
 * You call this function when you have new incoming bytes from your
 * hardware interface (UART, SPI, etc.). The idea is to parse or
 * accumulate those bytes in ctx->rx_buffer, looking for complete FMTP
 * packets. 
 *
 * This is a simplistic "search for header, read length, read data" approach.
 * If your real code is more advanced, copy that logic here.
 * ---------------------------------------------------------------- */
void fmtp_receive_data(FMTP_Context* ctx, 
                       const uint8_t* data, 
                       size_t length)
{
    if(!ctx || !data || length == 0) return;

    /* We just append the incoming data to our rx_buffer. */
    if(ctx->rx_length + length > FMTP_MAX_BUFFER_SIZE)
    {
        /* Overflow - in real code, handle it or flush it. */
        ctx->rx_length = 0; 
        return;
    }
    memcpy(&ctx->rx_buffer[ctx->rx_length], data, length);
    ctx->rx_length += length;

    /* Attempt to parse one (or more) complete FMTP packets from rx_buffer. 
       The simplest approach is to check if we see "FMTP" + length + ...
       If your old code had a while() loop to parse multiple packets in 
       the buffer, do that here. 
       For brevity, this example just tries to parse one packet and then stops.
    */
    if(ctx->rx_length < 10) 
    {
        /* Not enough to contain a header + length yet. */
        return;
    }

    /* Check for the header "FMTP" (0x464D5450). */
    ByteToU32 tmp;
    memcpy(tmp.bytes, ctx->rx_buffer, 4);
    if(tmp.value != FMTP_HEADER_VALUE)
    {
        /* Not a valid FMTP packet start. 
           In real code you might want to shift or flush or keep searching.
         */
        ctx->rx_length = 0;
        return;
    }

    /* The next 2 bytes might be the "seq" or "magic" or "packet number". 
       Then next 2 bytes might be the payload length. 
       This is just an example. 
    */
    uint16_t seq_num = (ctx->rx_buffer[5] << 8) | ctx->rx_buffer[4];
    uint16_t data_len = (ctx->rx_buffer[7] << 8) | ctx->rx_buffer[6];

    /* Make sure data_len doesn't exceed what's in the buffer. */
    if(data_len + 8 + 1 > ctx->rx_length)
    {
        /* We haven't received the entire payload + CRC yet. */
        return;
    }

    /* We have a full FMTP packet in the buffer. 
       The start of the payload is at offset 8: 
         4 bytes  => "FMTP"
         2 bytes  => seq
         2 bytes  => data_len
       Then data_len bytes of payload
       Then 1 byte of CRC at the end (??)
    */
    const uint8_t* payload_ptr = &ctx->rx_buffer[8];
    // data_len is the length of the payload
    // last byte is the CRC
    uint8_t crc_received = ctx->rx_buffer[8 + data_len];

    /* Here you would verify the CRC if you want. 
       We'll skip that step for brevity. 
    */

    /* Now we have the actual "command" and its payload in those data_len bytes. 
       Typically, your code stores the first 2 bytes of payload as 'command', etc. 
       We'll do it as an example:
    */
    if(data_len < 2)
    {
        /* Not enough for a command field, skip. */
        ctx->rx_length = 0;
        return;
    }
    uint16_t cmd = (payload_ptr[1] << 8) | payload_ptr[0];
    const uint8_t* actual_payload = &payload_ptr[2];
    uint16_t actual_len = data_len - 2; /* subtract the 2 bytes for 'cmd' */

    /* Dispatch to the library's command processor. */
    fmtp_process_packet(ctx, actual_payload, actual_len);

    /* If your library can handle multiple packets in the buffer, 
       you'd shift out the used bytes and keep going in a while loop. 
       For brevity, we just flush. 
    */
    ctx->rx_length = 0;
}

/* ----------------------------------------------------------------
 * Send an FMTP packet: builds the standard header "FMTP", writes seq,
 * writes payload length, writes payload, writes CRC, etc.
 * ---------------------------------------------------------------- */
int fmtp_send_packet(FMTP_Context* ctx,
                     uint16_t cmd_id,
                     const uint8_t* payload,
                     uint16_t payload_len,
                     int16_t seq_type)
{
    if(!ctx) return -1;

    uint8_t packet[FMTP_MAX_BUFFER_SIZE];
    size_t offset = 0;

    /* Check for overflow. For simplicity, 
       let's assume the user never tries to exceed FMTP_MAX_BUFFER_SIZE. */
    if(payload_len + 10 > FMTP_MAX_BUFFER_SIZE)
        return -2; 

    /* 1) Write the header 'FMTP' (4 bytes). */
    packet[offset++] = 'F';
    packet[offset++] = 'M';
    packet[offset++] = 'T';
    packet[offset++] = 'P';

    /* 2) Write seq_type (2 bytes, little-endian or big-endian as you wish). 
       Here we do little-endian, for example: */
    packet[offset++] = (uint8_t)(seq_type & 0xFF);
    packet[offset++] = (uint8_t)((seq_type >> 8) & 0xFF);

    /* 3) Write the payload length (2 bytes). */
    packet[offset++] = (uint8_t)(payload_len & 0xFF);
    packet[offset++] = (uint8_t)((payload_len >> 8) & 0xFF);

    /* 4) Write the command (2 bytes). */
    packet[offset++] = (uint8_t)(cmd_id & 0xFF);
    packet[offset++] = (uint8_t)((cmd_id >> 8) & 0xFF);

    /* 5) Write the payload. */
    if(payload_len > 0 && payload != NULL)
    {
        memcpy(&packet[offset], payload, payload_len);
        offset += payload_len;
    }

    /* 6) Compute a simple CRC if you want. 
       We'll just XOR everything, for example. 
    */
    uint8_t crc = 0;
    for(size_t i = 0; i < offset; i++)
    {
        crc ^= packet[i];
    }

    /* 7) Append the CRC. */
    packet[offset++] = crc;

    /* 8) Finally, write via the transport. */
    if(ctx->transport.write_bytes)
    {
        return ctx->transport.write_bytes(packet, offset, ctx->transport.user_ctx);
    }

    return -3; // transport not configured
}

/* ----------------------------------------------------------------
 * Helper to look up cmd in the user's command table
 * and call the handler with the payload data.
 * ---------------------------------------------------------------- */
static void fmtp_process_packet(FMTP_Context* ctx, 
                                const uint8_t* packet_data, 
                                size_t packet_len)
{
    if(!ctx || !ctx->command_table) 
    {
        /* If there's no table, we do nothing. */
        return;
    }

    /* The first 2 bytes of packet_data is presumably your "cmd". 
       That was already parsed in fmtp_receive_data, so let's assume 
       we are just calling that part here. 
       But to be consistent, let's do nothing. 
       (We've already got `cmd` in fmtp_receive_data.)
       This function is a placeholder if you'd prefer to do all
       command table lookups here. 
    */

    (void)packet_data; // avoid unused warnings
    (void)packet_len;  

    /* Another approach: You might parse the command ID here.
       If you do so, you'd do:
       
       if(packet_len < 2) return;
       uint16_t cmd = (packet_data[1] << 8) | packet_data[0];
       const uint8_t* actual_payload = &packet_data[2];
       uint16_t actual_len = (uint16_t)(packet_len - 2);

       // Then find that command in your table.
       for(size_t i=0; i<ctx->command_count; i++)
       {
          if(ctx->command_table[i].cmd_id == cmd)
          {
             ctx->command_table[i].handler(cmd, actual_payload, actual_len, ctx->transport.user_ctx);
             break;
          }
       }
    */
}
