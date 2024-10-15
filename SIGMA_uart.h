/**
 * @file    SIGMA_Uart.h
 * @author  Sigma Embedded 
 * @date    16/10/2024
 * @brief   This module is a layer between the HAL UART functions and the Xmodem protocol.
 *
 *          Copyright (c) 2024 SIGMA Embedded
 */

#ifndef SIGMA_UART_H_
#define SIGMA_UART_H_

//#include "stm32l5xx_hal.h"

//extern UART_HandleTypeDef hlpuart1;
#include <stdint.h>

/* Timeout for HAL. */
#define UART_TIMEOUT ((uint16_t)1000u)

/* Status report for the functions. */
typedef enum {
  UART_OK     = 0x00u, /**< The action was successful. */
  UART_ERROR  = 0xFFu  /**< Generic error. */
} uart_status;

uart_status SIGMA_Uart_Receive(uint8_t *data, uint16_t length);
uart_status SIGMA_Uart_Transmit_str(uint8_t *data);
uart_status SIGMA_Uart_Transmit_ch(uint8_t data);
void HAL_Delay(uint32_t delay);

#endif /* SIGMA_UART_H_ */
