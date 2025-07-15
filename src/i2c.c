#include "i2c.h"

uint8_t i2c_number(i2c_t *I2Cx)
{
    if(I2Cx == I2C1)
        return 1;
    if(I2Cx == I2C2)
        return 2;
    if(I2Cx == I2C3)
        return 3;
    return 0;
}

/**
 * @brief Helper to find the GPIO pins and AF for a given I2C peripheral.
 * @param[in] I2Cx The I2C peripheral.
 * @param[out] scl_conf Pointer to store the SCL pin GPIO config.
 * @param[out] sda_conf Pointer to store the SDA pin GPIO config.
 * @return 1 on success, 0 on failure.
 */
static int get_i2c_pin_configs(i2c_t *I2Cx, gpio_config_t *scl_conf, gpio_config_t *sda_conf)
{
    // Common settings for all I2C pins: Open-Drain, high-speed, pull-up.
    // Open-Drain is required by the I2C protocol.
    scl_conf->mode   = GPIO_MODE_ALTERNATE;
    scl_conf->otype  = GPIO_OTYPE_OPENDRAIN;
    scl_conf->ospeed = GPIO_OSPEED_HIGH;
    scl_conf->pupd   = GPIO_PUPD_PULLUP;
    scl_conf->alt_func = 4;
    *sda_conf = *scl_conf; // Copy common settings

    // --- Mappings for STM32L476RG ---
    if(I2Cx == I2C1) {      // Options: SCL = PB6, PB8, PG14; SDA = PB7, PB9, PG13
        scl_conf->port = GPIOB; scl_conf->pin = 6;
        sda_conf->port = GPIOB; sda_conf->pin = 7;
        return 1;
    }
    if(I2Cx == I2C2) {      // Options: SCL = PB10, PB13, PF1; SDA = PB11, PB14, PF0
        scl_conf->port = GPIOB; scl_conf->pin = 10;
        sda_conf->port = GPIOB; sda_conf->pin = 11;
        return 1;
    }
    if(I2Cx == I2C3) {      // Options: SCL = PC0, PG7; SDA = PC1, PG8 
        scl_conf->port = GPIOC; scl_conf->pin = 0;
        sda_conf->port = GPIOC; sda_conf->pin = 1;
        return 1;
    }
    
    return 0; // No mapping found
}

void i2c_init(i2c_t *I2Cx, uint32_t timing)
{
    // 1. Enable peripheral clock for the I2C port
    rcc_i2c_clock_enable(i2c_number(I2Cx));

    // 2. Configure GPIO pins for SCL and SDA
    gpio_config_t scl_pin_config, sda_pin_config;
    if (!get_i2c_pin_configs(I2Cx, &scl_pin_config, &sda_pin_config)) {
        return; // Failed to find pin mapping
    }
    gpio_init(&scl_pin_config);
    gpio_init(&sda_pin_config);

    // 3. Configure the I2C peripheral
    // Ensure the peripheral is disabled to allow configuration
    I2Cx->CR1 &= ~I2C_CR1_PE;

    // Set the timing register for the desired I2C speed (e.g., 100kHz or 400kHz)
    I2Cx->TIMINGR = timing;
    
    // Disable analog filter (optional, can improve robustness)
    // I2Cx->CR1 |= (1 << 12); // ANFOFF

    // Enable the peripheral
    I2Cx->CR1 |= I2C_CR1_PE;
}

/**
 * @brief Helper function to wait for a flag with a timeout.
 * @return 0 on success (flag set), -1 on timeout.
 */
static int i2c_wait_for_flag(i2c_t *i2c_port, uint32_t flag, uint32_t timeout)
{
    while (!(i2c_port->ISR & flag)) {
        if (--timeout == 0) return -1;
    }
    return 0;
}

/**
 * @brief Writes a block of data to an I2C slave device.
 */
int i2c_master_write(i2c_t *i2c_port, uint8_t slave_addr, const uint8_t *data, uint32_t size)
{
    // 1. Wait until the bus is not busy
    if (i2c_wait_for_flag(i2c_port, ~I2C_ISR_BUSY, 10000) != 0) return -1;

    // 2. Configure the transfer
    uint32_t cr2_val = 0;
    cr2_val |= (slave_addr << 1) & 0xFE; // Set slave address
    cr2_val &= ~I2C_CR2_RD_WRN_Pos;      // Set direction to Write
    cr2_val |= (size << I2C_CR2_NBYTES_Pos); // Set number of bytes
    cr2_val |= (1U << I2C_CR2_AUTOEND_Pos);  // Enable automatic STOP
    
    i2c_port->CR2 = cr2_val;

    // 3. Generate START condition
    i2c_port->CR2 |= (1U << I2C_CR2_START_Pos);

    // 4. Loop to write data bytes
    for (uint32_t i = 0; i < size; i++) {
        // Wait for TXIS (Transmit Interrupt Status) flag
        if (i2c_wait_for_flag(i2c_port, I2C_ISR_TXIS, 10000) != 0) return -1;
        
        // Write data to the transmit data register
        i2c_port->TXDR = data[i];
    }
    
    // 5. Wait for NACK or transfer complete
    // The AUTOEND feature will handle the STOP condition.
    if (i2c_port->ISR & I2C_ISR_NACKF) {
        return -2; // Slave sent NACK
    }

    return 0; // Success
}

/**
 * @brief Reads a block of data from an I2C slave device.
 */
int i2c_master_read(i2c_t *i2c_port, uint8_t slave_addr, uint8_t *data, uint32_t size)
{
    // 1. Wait until the bus is not busy
    if (i2c_wait_for_flag(i2c_port, ~I2C_ISR_BUSY, 10000) != 0) return -1;

    // 2. Configure the transfer
    uint32_t cr2_val = 0;
    cr2_val |= (slave_addr << 1) & 0xFE; // Set slave address
    cr2_val |= (1U << I2C_CR2_RD_WRN_Pos); // Set direction to Read
    cr2_val |= (size << I2C_CR2_NBYTES_Pos); // Set number of bytes
    cr2_val |= (1U << I2C_CR2_AUTOEND_Pos); // Enable automatic STOP
    
    i2c_port->CR2 = cr2_val;

    // 3. Generate START condition
    i2c_port->CR2 |= (1U << I2C_CR2_START_Pos);

    // 4. Loop to read data bytes
    for (uint32_t i = 0; i < size; i++) {
        // Wait for RXNE (Receive buffer Not Empty) flag
        if (i2c_wait_for_flag(i2c_port, I2C_ISR_RXNE, 10000) != 0) return -1;

        // Read data from the receive data register
        data[i] = i2c_port->RXDR;
    }
    
    if (i2c_port->ISR & I2C_ISR_NACKF) {
        return -2; // Should not happen in a read, but check anyway
    }

    return 0; // Success
}
