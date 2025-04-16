#ifndef FMTP_H
#define FMTP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * If the user wants to override the default buffer size,
 * they can define FMTP_MAX_BUFFER_SIZE before including this header.
 */
#ifndef FMTP_MAX_BUFFER_SIZE
  #define FMTP_MAX_BUFFER_SIZE 1024
#endif

/**
 * Basic data types used by FMTP
 */
typedef union
{
    struct {
        uint8_t high;
        uint8_t low;
    } bytes;
    uint16_t value;
} FMTP_Word;

typedef union
{
    struct {
        uint8_t b1;
        uint8_t b2;
        uint8_t b3;
        uint8_t b4;
    } bytes;
    float value;
} FMTP_Float;

typedef union
{
    struct {
        uint8_t b1;
        uint8_t b2;
        uint8_t b3;
        uint8_t b4;
    } bytes;
    uint32_t value;
} FMTP_LongWord;

/**
 * \brief A user-provided interface for reading/writing bytes.
 * The library never includes <stm32XX_hal_...> or FreeRTOS, etc.
 * You supply these callbacks to do the actual I/O.
 */
typedef struct
{
    /**
     * \brief Write/Send data. 
     * Return number of bytes actually written (or a negative error).
     */
    int (*write_bytes)(const uint8_t* data, size_t length, void* user_ctx);

    /**
     * \brief Read/Receive data. 
     * Return number of bytes actually read (or a negative error).
     */
    int (*read_bytes)(uint8_t* data, size_t length, void* user_ctx);

    /**
     * \brief An opaque pointer for your user context (e.g. a handle to UART).
     * This is passed back to you in the callbacks.
     */
    void* user_ctx;
} FMTP_Transport;

/**
 * \brief A command callback that is invoked when a recognized command arrives.
 * 
 * \param cmd       The command ID (16-bit) that was recognized.
 * \param payload   Pointer to payload bytes
 * \param length    Length of payload
 * \param user_ctx  User context pointer you can pass in if you want.
 */
typedef void (*FMTP_CommandHandler)(uint16_t cmd,
                                    const uint8_t* payload,
                                    uint16_t length,
                                    void* user_ctx);

/**
 * \brief A single command entry in your "command table."
 */
typedef struct
{
    uint16_t cmd_id;
    FMTP_CommandHandler handler;
} FMTP_CommandEntry;

/**
 * \brief The main FMTP context. 
 * Put all library-level data here instead of using file-level globals.
 */
typedef struct
{
    /// Transport callbacks
    FMTP_Transport transport;

    /// Optional pointer to a user-defined command table
    const FMTP_CommandEntry* command_table;
    /// Number of entries in the user-defined command table
    size_t command_count;

    /// A local buffer for assembling or parsing messages
    uint8_t rx_buffer[FMTP_MAX_BUFFER_SIZE];
    size_t  rx_length;  // how many bytes are currently in rx_buffer?

    /// Some place to store partial commands, etc.
    // You can add additional internal states as needed.
} FMTP_Context;

/**
 * \brief Initialize the FMTP context with the given transport interface.
 *        Also optionally set a command table if you have it at init time.
 */
void fmtp_init(FMTP_Context* ctx, 
               const FMTP_Transport* transport,
               const FMTP_CommandEntry* cmd_table,
               size_t cmd_count);

/**
 * \brief Provide a command table at any time if you want to update it later.
 */
void fmtp_set_command_table(FMTP_Context* ctx,
                            const FMTP_CommandEntry* cmd_table,
                            size_t cmd_count);

/**
 * \brief Process incoming bytes. 
 * Call this whenever you have new data from your hardware interface.
 */
void fmtp_receive_data(FMTP_Context* ctx, 
                       const uint8_t* data, 
                       size_t length);

/**
 * \brief Send an FMTP packet with the given command, payload, and type.
 *
 * \param ctx        FMTP context
 * \param cmd_id     16-bit command ID
 * \param payload    Pointer to data payload
 * \param payload_len Number of bytes in payload
 * \param seq_type   Typically the "magic" or seq number (like -1, -2, or 0..).
 * \return 0 if OK, negative on error.
 *
 * In your old code: 
 * -   type = -2 for EVENT, 
 * -   type = -1 for RESPONSE, 
 * -   type >= 0 for MASTER, etc.
 */
int fmtp_send_packet(FMTP_Context* ctx,
                     uint16_t cmd_id,
                     const uint8_t* payload,
                     uint16_t payload_len,
                     int16_t seq_type);

#ifdef __cplusplus
}
#endif

#endif /* FMTP_H */
