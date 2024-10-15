#ifndef XMODEM_H
#define XMODEM_H

#include "SIGMA_uart.h"
#include "SIGMA_iflash.h"

// XMODEM Protocol Constants
#define SOH               0x01  // Start of 128-byte packet
#define STX               0x02  // Start of 1024-byte packet
#define EOT               0x04  // End of transmission
#define ACK               0x06  // Acknowledge
#define NAK               0x15  // Negative acknowledge
#define CAN               0x18  // Cancel transmission
#define PACKET_SIZE_128   128
#define PACKET_SIZE_1024  1024

// Xmodem Packet Structure
typedef struct {
    uint8_t start_byte;
    uint8_t packet_number;
    uint8_t packet_number_complement;
    uint8_t data[PACKET_SIZE_1024]; // Max size of a packet (1024 bytes)
    uint16_t crc;
} XmodemPacket;

// Xmodem Status Enum
typedef enum {
    XMODEM_OK,               // Packet received successfully
    XMODEM_CRC_ERROR,        // CRC check failed
    XMODEM_TIMEOUT_ERROR,    // UART timeout
    XMODEM_PACKET_NUM_ERROR, // Packet number mismatch
    XMODEM_FLASH_ERROR,      // Flash write/erase error
    XMODEM_UNKNOWN_ERROR     // Any unknown error
} XmodemStatus;

// Function Prototypes
XmodemStatus xmodem_receive_file_to_flash(uint32_t flash_address, uint32_t flash_size);
XmodemStatus xmodem_receive_packet(XmodemPacket *packet);
uint16_t xmodem_calculate_crc16(const uint8_t *data, uint16_t length);
void xmodem_send_ack(void);
void xmodem_send_nak(void);

#endif // XMODEM_H
