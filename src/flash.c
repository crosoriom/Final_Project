#include "flash.h"

void flash_configure_for_high_speed(void)
{
	// PASO 1: Limpiar los bits de latencia actuales.
	// Se usa una operación de lectura-modificación-escritura segura.
	FLASH->ACR &= ~FLASH_ACR_LATENCY_MASK;

	// PASO 2: Establecer la nueva latencia en 4 Wait States (4WS).
	// Esto es obligatorio para que el CPU pueda leer la memoria FLASH a 80 MHz.
	FLASH->ACR |= FLASH_ACR_LATENCY_4WS;

	// PASO 3 (Opcional pero recomendado): Habilitar cachés y prefetch.
	// Esto mejora significativamente el rendimiento al reducir la necesidad
	// de que el CPU espere a la memoria FLASH.
	FLASH->ACR |= FLASH_ACR_PRFTEN | FLASH_ACR_ICEN | FLASH_ACR_DCEN;
}
