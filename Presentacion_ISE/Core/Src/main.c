/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include "adc.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdint.h>
/* USER CODE END Includes */

void SystemClock_Config(void);

/* USER CODE BEGIN PV */

// --- ADC ---
uint16_t adc_raw = 0;

// --- Promedio movil ---
#define N_PROMEDIO 8
uint32_t ventana[N_PROMEDIO] = {0};
uint8_t  idx_ventana = 0;
uint32_t suma_ventana = 0;
uint16_t senal_suavizada = 0;

// --- Deteccion de BPM ---
// Ajustar UMBRAL segun la señal real del KY-038.
#define UMBRAL_LATIDO 2200

uint32_t ultimo_latido_ms = 0;
uint16_t bpm = 0;
uint8_t  en_pico = 0;

// --- UART ---
char msg[32];

/* USER CODE END PV */

int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
  /* USER CODE END 2 */

  while (1)
  {
    /* USER CODE BEGIN WHILE */

    // 1. Leer ADC por polling
    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
    {
        adc_raw = (uint16_t)HAL_ADC_GetValue(&hadc1);

        // 2. Promedio movil de N_PROMEDIO muestras
        suma_ventana -= ventana[idx_ventana];
        ventana[idx_ventana] = adc_raw;
        suma_ventana += adc_raw;
        idx_ventana = (idx_ventana + 1) % N_PROMEDIO;
        senal_suavizada = (uint16_t)(suma_ventana / N_PROMEDIO);

        // 3. Deteccion de latido sobre la señal suavizada
        if (senal_suavizada > UMBRAL_LATIDO && !en_pico)
        {
            en_pico = 1;
            uint32_t ahora = HAL_GetTick();

            if (ultimo_latido_ms > 0)
            {
                uint32_t intervalo_ms = ahora - ultimo_latido_ms;

                // Filtro fisiologico: 30 BPM (2000ms) a 200 BPM (300ms)
                if (intervalo_ms >= 300 && intervalo_ms <= 2000)
                {
                    bpm = (uint16_t)(60000UL / intervalo_ms);
                }
            }
            ultimo_latido_ms = ahora;
        }
        else if (senal_suavizada < UMBRAL_LATIDO)
        {
            en_pico = 0;
        }

        // 4. Enviar señal suavizada y BPM por UART
        int len = sprintf(msg, "SEÑAL:%u BPM:%u\r\n", senal_suavizada, bpm);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, len, 100);
    }

    HAL_Delay(10);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) { Error_Handler(); }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                               | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) { Error_Handler(); }
}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  __disable_irq();
  while (1) {}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* USER CODE END 6 */
}
#endif
