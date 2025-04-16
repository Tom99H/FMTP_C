#ifndef SHARED_COMMANDS_H
#define SHARED_COMMANDS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "../fmtp.h"

void handle_who   (uint16_t cmd, const uint8_t* payload, uint16_t length, void* user_ctx);
void handle_ping  (uint16_t cmd, const uint8_t* payload, uint16_t length, void* user_ctx);
void handle_getver(uint16_t cmd, const uint8_t* payload, uint16_t length, void* user_ctx);

extern const FMTP_CommandEntry my_command_table[];
extern const size_t my_command_table_size;

#ifdef __cplusplus
}
#endif

#endif /* SHARED_COMMANDS_H */