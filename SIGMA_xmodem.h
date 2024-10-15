/**
 * @file xmodem.h
 * @brief XMODEM protocol receiver implementation for STM32 over UART.
 * @author SIGMA Embedded 
 * This file contains the function declarations and data structures required for 
 * implementing the XMODEM protocol, designed to receive files over UART on an STM32 microcontroller.
 *    
 * @Copyright 2024 SIGMA Embedded
 */
#ifndef XMODEM_H
#define XMODEM_H

#include "SIGMA_uart.h"
#include "SIGMA_iflash.h"

/**
 * @brief XMODEM protocol Constants 
 */
#define SOH               0x01  // Start of 128-byte packet
#define STX               0x02  // Start of 1024-byte packet
#define EOT               0x04  // End of transmission
#define ACK               0x06  // Acknowledge
#define NACK              0x15  // Negative acknowledge
#define CAN               0x18  // Cancel transmission
#define PACKET_SIZE_128   128
#define PACKET_SIZE_1024  1024

/**
 * @brief Structure representing an XMODEM packet.
 * 
 * This structure holds the relevant fields of an XMODEM packet, including the
 * start byte, sequence number, payload, and checksum.
 */
typedef struct {
    uint8_t start_byte;
    uint8_t packet_number;
    uint8_t packet_number_complement;
    uint8_t data[PACKET_SIZE_1024]; // Max size of a packet (1024 bytes)
    uint16_t crc;
} XmodemPacket;

/**
 * @brief Status codes used in the XMODEM protocol.
 */
typedef enum {
    XMODEM_OK,               // Packet received successfully
    XMODEM_CRC_ERROR,        // CRC check failed
    XMODEM_TIMEOUT_ERROR,    // UART timeout
    XMODEM_PACKET_NUM_ERROR, // Packet number mismatch
    XMODEM_FLASH_ERROR,      // Flash write/erase error
    XMODEM_UNKNOWN_ERROR     // Any unknown error
} XmodemStatus;

// Function Prototypes

/**
 * @brief Receives a file using the XMODEM protocol and writes it to flash memory.
 * 
 * This function initiates the file transfer by repeatedly sending 'C' to request 
 * the first data packet from the host. Once the transfer starts, it continues 
 * receiving packets, verifying their integrity using CRC, and writing the payload 
 * to the flash memory location of the Application. It handles errors like timeouts and 
 * CRC mismatches by sending appropriate responses (NACK) to the host and retries the 
 * reception when necessary.
 * 
 * The file transfer ends when an End of Transmission (EOT) character is received 
 * from the host, and the function responds with an ACK.
 * 
 * @return XmodemStatus Status of the operation. Possible return values:
 *         - XMODEM_OK: File received successfully.
 *         - XMODEM_TIMEOUT: Timeout while waiting for packets.
 *         - XMODEM_ERROR: Error occurred during the reception process.
 */
XmodemStatus xmodem_receive_file(void);

/**
 * @brief Receives a single XMODEM packet.
 * 
 * This function receives a packet from the UART, checks the CRC for integrity, and
 * handles sequence numbers.
 * 
 * @param[out] packet Pointer to the XmodemPacket structure that will hold the received data.
 * 
 * @return XmodemStatus Status of the packet reception.
 */
XmodemStatus xmodem_receive_packet(XmodemPacket *packet);

/**
 * @brief Calculates the CRC-16 checksum for a given data buffer.
 * 
 * This function implements the standard CRC-16-CCITT algorithm used by XMODEM to 
 * verify data integrity.
 * 
 * @param[in] data   Pointer to the data buffer.
 * @param[in] length Length of the data buffer.
 * 
 * @return The calculated CRC-16 value.
 */
uint16_t xmodem_calculate_crc16(const uint8_t *data, uint16_t length);

/**
 * @brief Sends an ACK character over UART to the host.
 * 
 * The ACK (0x06) character is used to acknowledge successful receipt of a packet.
 */
void xmodem_send_ack(void);

/**
 * @brief Sends a NAK character over UART to the host.
 * 
 * The NAK (0x15) character indicates that there was an error in receiving the packet.
 */
void xmodem_send_nack(void);

#endif // XMODEM_H
