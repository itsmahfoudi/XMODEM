#include "xmodem.h"
#include "SIGMA_uart.h"  // Include the UART interface header

// XMODEM receive file to flash function
XmodemStatus xmodem_receive_file_to_flash(uint32_t flash_address, uint32_t flash_size) {
    XmodemPacket packet;
    uint8_t expected_packet_number = 1;
    uint32_t bytes_received = 0;
    HAL_StatusTypeDef flash_status;
    XmodemStatus receive_status;
    uint8_t first_packet_received = 0x00;

    // Send 'C' to initiate the transfer
    while (!first_packet_received) {
        SIGMA_Uart_Transmit_ch('C');  // Send 'C' to the host to start transmission
        receive_status = xmodem_receive_packet(&packet);

        if (receive_status == XMODEM_OK && (packet.start_byte == SOH || packet.start_byte == STX)) {
            first_packet_received = 0x01;
        } else {
            // Small delay between sending 'C'
            HAL_Delay(100);
        }
    }

    // Erase flash memory at the start
    flash_status = SIGMA_Iflash_Erase(flash_address);
    if (flash_status != HAL_OK) {
        return XMODEM_FLASH_ERROR;  // Flash erase failed
    }

    while (1) {
        if (receive_status == XMODEM_OK) {
            if (packet.start_byte == EOT) {
                xmodem_send_ack();  // End of transmission
                break;
            }

            if (packet.packet_number == expected_packet_number) {
                uint16_t packet_size = (packet.start_byte == SOH) ? PACKET_SIZE_128 : PACKET_SIZE_1024;

                // Ensure data fits within flash size
                if (bytes_received + packet_size > flash_size) {
                    return XMODEM_FLASH_ERROR;  // Exceeds flash size
                }

                // Write data to flash memory
                flash_status = SIGMA_Iflash_Write(flash_address + bytes_received, packet.data, packet_size);
                if (flash_status != HAL_OK) {
                    return XMODEM_FLASH_ERROR;  // Flash write failed
                }

                bytes_received += packet_size;
                expected_packet_number++;
                xmodem_send_ack();  // Acknowledge successful packet reception
            } else {
                xmodem_send_nak();  // Packet number mismatch
            }
        } else {
            // Handle error status
            switch (receive_status) {
                case XMODEM_CRC_ERROR:
                    xmodem_send_nak();  // CRC error, request retransmission
                    break;
                case XMODEM_TIMEOUT_ERROR:
                    xmodem_send_nak();  // Timeout error
                    break;
                case XMODEM_PACKET_NUM_ERROR:
                    xmodem_send_nak();  // Packet number mismatch
                    break;
                default:
                    return XMODEM_UNKNOWN_ERROR;  // Unknown error occurred
            }
        }

        // Get next packet
        receive_status = xmodem_receive_packet(&packet);
    }

    return XMODEM_OK;  // File received successfully
}

// Function to receive a packet over UART
XmodemStatus xmodem_receive_packet(XmodemPacket *packet) {
    uint8_t header[3];  // Header: start byte, packet number, and complement

    if (SIGMA_Uart_Receive(header, 3) != UART_OK) {
        return XMODEM_TIMEOUT_ERROR;  // Timeout or UART error
    }

    packet->start_byte = header[0];
    packet->packet_number = header[1];
    packet->packet_number_complement = header[2];

    uint16_t packet_size = (packet->start_byte == SOH) ? PACKET_SIZE_128 : PACKET_SIZE_1024;

    // Receive packet data
    if (SIGMA_Uart_Receive(packet->data, packet_size) != UART_OK) {
        return XMODEM_TIMEOUT_ERROR;  // Timeout or UART error
    }

    // Receive CRC
    uint8_t crc_buf[2];
    if (SIGMA_Uart_Receive(crc_buf, 2) != UART_OK) {
        return XMODEM_TIMEOUT_ERROR;  // Timeout or UART error
    }
    packet->crc = (crc_buf[0] << 8) | crc_buf[1];

    // Verify CRC
    uint16_t calculated_crc = xmodem_calculate_crc16(packet->data, packet_size);
    if (calculated_crc != packet->crc) {
        return XMODEM_CRC_ERROR;  // CRC mismatch
    }

    return XMODEM_OK;  // Packet received successfully
}

// Example CRC-16 calculation function
uint16_t xmodem_calculate_crc16(const uint8_t *data, uint16_t length) {
    uint16_t crc = 0x0000;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}


// Function to send ACK
void xmodem_send_ack(void) {
    SIGMA_Uart_Transmit_ch(ACK);
}

// Function to send NAK
void xmodem_send_nak(void) {
    SIGMA_Uart_Transmit_ch(NAK);
}

