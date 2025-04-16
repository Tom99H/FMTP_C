#include "deviceA_commands.h"

/* Example: user command handlers */
void handle_light_blick   (uint16_t cmd, const uint8_t* payload, uint16_t length, void* user_ctx);
void handle_regulation  (uint16_t cmd, const uint8_t* payload, uint16_t length, void* user_ctx);
void handle_example(uint16_t cmd, const uint8_t* payload, uint16_t length, void* user_ctx);

/* Example: define a command table in your application code. */
const FMTP_CommandEntry deviceA_command_table[] =
{
    { 0x1000, handle_light_blick },  /* LIGHT_BLICKING */
    { 0x1001, handle_regulation  },  /* REGULATION */
    { 0x1004, handle_example },  /* EXAMPLE */
    // etc...
};

void handle_light_blick   (uint16_t cmd, const uint8_t* payload, uint16_t length, void* user_ctx){

    (void)cmd; (void)payload; (void)length; (void)user_ctx;

    printf("Got LIGHT_BLICKING command!\n");
    // Start blicking
}

void handle_regulation  (uint16_t cmd, const uint8_t* payload, uint16_t length, void* user_ctx){

    (void)cmd; (void)payload; (void)length; (void)user_ctx;

    printf("Got REGULATION command!\n");
    // Start regulation
}

void handle_example(uint16_t cmd, const uint8_t* payload, uint16_t length, void* user_ctx){

    (void)cmd; (void)payload; (void)length; (void)user_ctx;

    printf("Got EXAMPLE command!\n");
    // Start example
}