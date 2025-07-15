#ifndef FLASH_H
#define FLASH_H

#include <stdint.h>

// Dirección base del periférico FLASH para STM32L476RG
#define FLASH ((Flash_t *)0x40022000UL)

// --- Macros para el Registro de Control de Acceso (ACR) ---

// Máscara para limpiar los bits de latencia (bits 0-2)
#define FLASH_ACR_LATENCY_MASK (0x7 << 0)
// Valor para establecer 4 Wait States (necesario para 80 MHz)
#define FLASH_ACR_LATENCY_4WS (0x4 << 0)

// Bits para habilitar los buffers y cachés para mejor rendimiento
#define FLASH_ACR_PRFTEN (0x1 << 8)  // Habilitar prefetch buffer
#define FLASH_ACR_ICEN   (0x1 << 9)  // Habilitar instruction cache
#define FLASH_ACR_DCEN   (0x1 << 10) // Habilitar data cache

/**
 * @brief Estructura que mapea los registros del periférico FLASH
 */
typedef struct {
	volatile uint32_t ACR;
	volatile uint32_t PDKEYR;
	volatile uint32_t KEYR;
	volatile uint32_t OPTKEYR;
	volatile uint32_t SR;
	volatile uint32_t CR;
	volatile uint32_t ECCR;
    volatile uint32_t RESERVED0;
    volatile uint32_t OPTR;
    volatile uint32_t PCROP1SR;
    volatile uint32_t PCROP1ER;
    volatile uint32_t WRP1AR;
    volatile uint32_t WRP1BR;
    volatile uint32_t PCROP2SR;
    volatile uint32_t PCROP2ER;
    volatile uint32_t WRP2AR;
    volatile uint32_t WRP2BR;
} Flash_t;


/**
 * @brief Configura la latencia de la memoria FLASH para altas frecuencias.
 *
 * Esta función debe ser llamada ANTES de aumentar la velocidad del reloj
 * del sistema a 80 MHz. Configura los Wait States necesarios y activa
 * las cachés para asegurar un funcionamiento estable y de alto rendimiento.
 */
void flash_configure_for_high_speed(void);


#endif // FLASH_H
