#ifndef __DMB_MESSAGE_TYPE_H
#define __DMB_MESSAGE_TYPE_H
#define MAX_MESSAGE_LEN 8
typedef struct {
    uint8_t Message[MAX_MESSAGE_LEN];
} dmb_message;

extern volatile uint32_t DMBMessageReceived;
extern volatile dmb_message DMBMailBox;
extern volatile uint16_t DMBMessageSender;
extern volatile uint32_t DMBGameMessageReceived;
extern volatile dmb_message DMBGameMailBox;
extern volatile uint32_t DMBGameMessageResponseReceived;
extern volatile dmb_message DMBGameResponseMailBox;
#endif
