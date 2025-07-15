#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include "gpio.h"
#include "rcc.h"

/* Base address for all I2C ports */
#define I2C1 ((i2c_t *)0x40005400UL)
#define I2C2 ((i2c_t *)0x40005800UL)
#define I2C3 ((i2c_t *)0x40005C00UL)

// --- I2C Control Register Bits ---
#define I2C_CR1_PE_Pos      (0U)
#define I2C_CR1_PE          (1U << I2C_CR1_PE_Pos) // Peripheral Enable
#define I2C_CR2_SADD_Pos    (0U)  // Slave Address (master mode)
#define I2C_CR2_RD_WRN_Pos  (10U) // Transfer direction (0:Write, 1:Read)
#define I2C_CR2_START_Pos   (13U) // Start generation
#define I2C_CR2_STOP_Pos    (14U) // Stop generation
#define I2C_CR2_NACK_Pos    (15U) // NACK generation
#define I2C_CR2_NBYTES_Pos  (16U) // Number of bytes to transfer
#define I2C_CR2_AUTOEND_Pos (25U) // Automatic END condition

// --- I2C Interrupt and Status Register Bits ---
#define I2C_ISR_TXE_Pos     (0U)
#define I2C_ISR_TXE         (1U << I2C_ISR_TXE_Pos) // Transmit buffer empty
#define I2C_ISR_TXIS_Pos    (1U)
#define I2C_ISR_TXIS        (1U << I2C_ISR_TXIS_Pos) // Transmit interrupt status
#define I2C_ISR_RXNE_Pos    (2U)
#define I2C_ISR_RXNE        (1U << I2C_ISR_RXNE_Pos) // Receive buffer not empty
#define I2C_ISR_NACKF_Pos   (4U)
#define I2C_ISR_NACKF       (1U << I2C_ISR_NACKF_Pos) // Not Acknowledge received flag
#define I2C_ISR_TC_Pos      (6U)
#define I2C_ISR_TC          (1U << I2C_ISR_TC_Pos)  // Transfer Complete flag
#define I2C_ISR_BUSY_Pos    (15U)
#define I2C_ISR_BUSY        (1U << I2C_ISR_BUSY_Pos) // Bus busy flag

// Register map for an I2C peripheral
typedef struct {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t OAR1;
    volatile uint32_t OAR2;
    volatile uint32_t TIMINGR;
    volatile uint32_t TIMEOUTR;
    volatile uint32_t ISR;
    volatile uint32_t ICR;
    volatile uint32_t PECR;
    volatile uint32_t RXDR;
    volatile uint32_t TXDR;
} i2c_t;

/**
 * @brief Initializes an I2C peripheral in master mode.
 * @param[in] config Pointer to the i2c_config_t struct.
 */
void i2c_init(i2c_t *I2Cx, uint32_t timing);

/**
 * @brief Writes a block of data to an I2C slave device.
 * @param[in] i2c_port Pointer to the I2C peripheral.
 * @param[in] slave_addr The 7-bit address of the slave device.
 * @param[in] data Pointer to the data buffer to write.
 * @param[in] size The number of bytes to write.
 * @return 0 on success, non-zero on error (e.g., NACK).
 */
int i2c_master_write(i2c_t *i2c_port, uint8_t slave_addr, const uint8_t *data, uint32_t size);

/**
 * @brief Reads a block of data from an I2C slave device.
 * @param[in] i2c_port Pointer to the I2C peripheral.
 * @param[in] slave_addr The 7-bit address of the slave device.
 * @param[out] data Pointer to the buffer to store the read data.
 * @param[in] size The number of bytes to read.
 * @return 0 on success, non-zero on error.
 */
int i2c_master_read(i2c_t *i2c_port, uint8_t slave_addr, uint8_t *data, uint32_t size);

#endif
