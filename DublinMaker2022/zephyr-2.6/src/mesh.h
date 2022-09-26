#ifndef __mesh_h
#define __mesh_h
#include "dmb_message_type.h"
#define MAX_NAME_LEN MAX_MESSAGE_LEN
extern "C" void mesh_begin(uint16_t address);
extern "C" void sendDMBMessage(uint16_t target, dmb_message * dmbmsg);
extern "C" void sendDMBGameMessage(uint16_t the_target, dmb_message * dmbmsg);
extern "C" void sendDMBGameResponseMessage(uint16_t the_target, dmb_message * dmbmsg);
extern "C" void mesh_clearReplay();
extern "C" void mesh_suspend();
extern "C" void mesh_resume();
extern volatile uint32_t DMBMessageReceived;
extern volatile dmb_message DMBMailBox;
extern volatile uint32_t DMBGameMessageReceived;
extern volatile dmb_message DMBGameMailBox;
extern volatile uint32_t DMBGameMessageResponseReceived;
extern volatile dmb_message DMBGameResponseMailBox;
#endif
