#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define GPIO_LED                2     
#define GPIO_BUTTON             4    
#define DEBOUNCE_DELAY_MS       20
#define DEBOUNCE_DELAY_TICKS    pdTICKS_TO_MS(20) 

void app_main(void)
{
    // 1. Configuración de pines individuales
    gpio_set_direction(GPIO_LED, GPIO_MODE_OUTPUT);
    gpio_set_direction(GPIO_BUTTON, GPIO_MODE_INPUT);
    
    // Intentamos activar pull-up 
    gpio_pullup_en(GPIO_BUTTON); 

    gpio_set_level(GPIO_LED, 0);

    // Variables para registrar el estado real del botón (filtrado)
    int ultimo_estado_confirmado = 1; 

    printf("Programa con filtro de ruido (Debounce: %ld ms) iniciado.\n", DEBOUNCE_DELAY_TICKS);

    while (1) {
        // 1. Leer el estado instantáneo del pin
        int lectura_actual = gpio_get_level(GPIO_BUTTON);

        // 2. Si el pin cambió respecto a nuestro último estado confirmado...
        if (lectura_actual != ultimo_estado_confirmado) {
            
            // Esperamos el tiempo de debounce para que el ruido/vibración se apague
            vTaskDelay(DEBOUNCE_DELAY_MS  );
            
            // Volvemos a leer. Si el estado sigue siendo el mismo, la lectura es real
            if (gpio_get_level(GPIO_BUTTON) == lectura_actual) {
                ultimo_estado_confirmado = lectura_actual;
                
                // Aplicamos la acción basándonos en el estado confirmado
                if (ultimo_estado_confirmado == 0) {
                    gpio_set_level(GPIO_LED, 1);  // Enciende el LED
                    printf("Botón presionado (Filtro aprobado)\n");
                } else {
                    gpio_set_level(GPIO_LED, 0);  // Apaga el LED
                    printf("Botón suelto (Filtro aprobado)\n");
                }
            }
        }

        // Un pequeño respiro para el procesador (1 milisegundo)
        vTaskDelay(pdTICKS_TO_MS(1));
    }
}