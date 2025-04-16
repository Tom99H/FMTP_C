#ifndef DEVICEA_COMMANDS_H
#define DEVICEA_COMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../fmtp.h"

/* Example: user command handlers */
void handle_light_blick   (uint16_t cmd, const uint8_t* payload, uint16_t length, void* user_ctx);
void handle_regulation  (uint16_t cmd, const uint8_t* payload, uint16_t length, void* user_ctx);
void handle_example(uint16_t cmd, const uint8_t* payload, uint16_t length, void* user_ctx);

extern const FMTP_CommandEntry deviceA_command_table[];
extern size_t deviceA_command_table_size;

#ifdef __cplusplus
}
#endif

#endif /* DEVICEA_H */