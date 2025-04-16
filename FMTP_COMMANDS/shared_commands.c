#include "shared_commands.h"


/* Example: user command handlers */
void handle_who   (uint16_t cmd, const uint8_t* payload, uint16_t length, void* user_ctx);
void handle_ping  (uint16_t cmd, const uint8_t* payload, uint16_t length, void* user_ctx);
void handle_getver(uint16_t cmd, const uint8_t* payload, uint16_t length, void* user_ctx);

/* Example: define a command table in your application code. */
const FMTP_CommandEntry my_command_table[] =
{
    { 0x0000, handle_who    },  /* WHO */
    { 0x0001, handle_ping   },  /* PING */
    { 0x0004, handle_getver },  /* GET_VERSION_STRING */
    // etc...
};

/* Example: define a command table size */
const size_t my_command_table_size = sizeof(my_command_table) / sizeof(my_command_table[0]);

/* Implement each command handler */
static void handle_who(uint16_t cmd, const uint8_t* payload, uint16_t length, void* user_ctx)
{
    (void)cmd; (void)payload; (void)length; (void)user_ctx;

    printf("Got WHO command!\n");
    // Possibly respond with a "device type" or something...
}

static void handle_ping(uint16_t cmd, const uint8_t* payload, uint16_t length, void* user_ctx)
{
    (void)cmd; (void)payload; (void)length; (void)user_ctx;
    printf("Got PING!\n");
    // Possibly respond with an "OK" packet, e.g.:
    // fmpt_send_packet(...);
}

static void handle_getver(uint16_t cmd, const uint8_t* payload, uint16_t length, void* user_ctx)
{
    (void)cmd; (void)payload; (void)length; (void)user_ctx;
    printf("Got GET_VERSION_STRING command!\n");
    // Possibly respond with version data
}

