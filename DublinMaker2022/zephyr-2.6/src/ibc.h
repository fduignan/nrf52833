/* Inter-Badge-Communications protocol for DublinMakerBadge
 * The USART supports a single wire half duplex protocol. The TX pin only is used 
 * and is configured as an open drain with an external pull-up.  When it is transmitting
 * it becomes an output.  When not transmitting it is in receiver mode.
 * Given the probably poor cabling and connections the communications speed will be set
 * as low as possible but not so low that it gets in the way of gameplay.  The protocol should therefore
 * have as little overhead as possible.  
 * Addressing : 
 * Some applications will require sender and receiver to be identified so need to send the both addresses 
 * in each data packet.  Will use a single byte for device address - probably overkill to allow this
 * but it is simpler to process and could have special addresses e.g. broadcast for certain situations.
 * Indicating start of packet : The hardware link as designed has signal wires: One for half duplex data
 * the other for indicating that the bus is busy.  A sender pulls this line low just prior to starting
 * a transmission.  This is handy for a number of reasons:
 * It allows us know when the bus is busy.
 * It can be tied to an interrupt to put the chip into "packet receive mode".
 * When it goes high, packets are either processed or discarded.
 * No special start or end of packet signals are necessary
 * In some ways this is like the "carrier" in CSMA/CD
 * Flags/control byte: Allow 4 bits for flags (such as : ACK, NAK, ANNOUNCE + 1 more; 4 bits for length so up to 15 bytes
 * FLG_LEN byte formatting ACK NAK ANNONCE OTHER LEN3 LEN2 LEN1 LEN0
 * Finally a check value of some kind
 * The STM32F030 has a CRC unit that can be used for CRC calculation.  It has a 32 bit output however.
 * I only want to send an 8 bit CRC value.  Is it valid to use just the lower 8 bits of the CRC?
 * Data transmission packet (#=idle (pulled high))
 * 
 * tx/rx signal #########DST SRC FLG_LEN DATA0..DATAn CHK###########
 * busy signal  #######|__________________________________|#########
 * 
 * Transmission time: Lets say we use a baud rate of 9600 bps
 * DST+SRC+FLG_LEN+CHK + N*DATA = 4+N bytes
 * This is basically RS232 so start and stop bits are sent also so 10 bits to send 1 byte
 * for a 4 byte transfer this takes 8.33ms
 * for a 15 byte transfer this takes 19.79ms
 * May have to drop the speed down to accommodate longer wire runs
 * 
 * Address assignment: On power up, random wait, send an ANNOUNCE broadcast packet
 * 0xff SRC 0x20 CHK
 * If some other device has this address then a NAK will be sent and a new ANNOUNCE with the next address is sent.
 * NAKS may get lost but not dealing with this - its not that critical a protocol
 * 
 * Error detection.  The CHK character will be used to validate packet reception and transmission.  When a sender 
 * sends data it also listens.  CRC checking can be performed on what it receives 
 * 
 * Reciver code:
 * When busy goes low -> busy start interrupt
 * On busy-start-interrupt
 *      reset packet buffer (zero the buffer to avoid the risk of repeated processing of same packet)
 *      On each subsequent receive interrupt fill buffer up to maximum
 * When busy signal goes high -> busy end interrupt
 *      Check the packet using CHK.  If OK then set "PACKET_RECEIVED" flag, if not then no action.
 *      Who is it for?
 *          Broadcast -> ANNOUNCE ? Is so send NAK if address = this address
 *          For this device? -> Yes, Inform higher layer?; No then no action
 * Sender code
 * Line busy? If so then wait for busy end interrupt then delay for a random time.
 * If line always busy (retry counter?) then exit (LINE FAULT ERROR?)
 * Line free? If so, drive line busy low.  output packet, check for valid "PACKET_RECEIVED" flag.
 * If valid packet not received then retry later.
 * If valid packet received then clear packet queue. 
 * 
 * Hardware 
 * PA9 will be used as TX in half duplex, opencollector mode
 * PA10 will be used as the "Busy" signal (busy = low)
 * PA10 is wired through EXIT10.  
 *      
 */
#include <stdint.h>
#include "timer.h"
#define MAX_PAYLOAD_SIZE 15
#define MAX_PACKET_SIZE (MAX_PAYLOAD_SIZE + 5)
#define BROADCAST_ADDRESS 0xff
#define UNASSIGNED_ADDRESS 0
#define NAK_FLAG 0x80
#define ACK_FLAG 0x40
#define ANNOUNCE_FLAG 0x20
#define LINE_IDLE_TIMEOUT 500
#define LINE_SPEED 1200
enum States {
    IdleState,
    ParsingState,    
    SendingState,
    AnnouncingState
};
class ibc {
public: 
        ibc() {};
        void begin(timer *t);
        void setAddress(uint8_t newAddress) 
        {
            this->MyAddress = newAddress;
        }
        int sendPacket(uint8_t Destination,uint8_t Flags,uint8_t len,uint8_t *Packet);    
        int getPacket(uint8_t len, uint8_t *Packet);
        uint8_t getAddress() 
        {
            return this->MyAddress;
        }
private:
        void handle_rxdata(void);  // called in response to receive interrupt
        void OnLineBecomesFree();  // called when line is idle
        void OnLineBecomesBusy();  // called when line is busy
        void assertLineBusy();
        void deassertLineBusy();
        int lineIsFree(); // returns line state (1 => free)            
        uint8_t getCHK(uint8_t len,uint8_t *Packet);
        uint8_t validatePacket(uint8_t len, uint8_t *Packet);        
        void sendNAK();            
        timer * Timer;
        uint8_t MyAddress;
        uint8_t InputBuffer[MAX_PACKET_SIZE];
        uint8_t OutputBuffer[MAX_PACKET_SIZE];
        volatile States State;
        volatile uint32_t InputIndex;        
        volatile uint32_t OutputPacketLength;
        volatile uint32_t TXPacketWaiting;
        volatile uint32_t RXPacketWaiting;
        
        
// frient functions that are used to handle interrupts        
        friend void USART1_Handler(void);
        friend void Line_State_Handler(void);       
};
