/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_ll_adc.h"
#include "stm32f4xx_ll_dac.h"
#include "stm32f4xx_ll_dma.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_system.h"
#include "stm32f4xx_ll_exti.h"
#include "stm32f4xx_ll_cortex.h"
#include "stm32f4xx_ll_utils.h"
#include "stm32f4xx_ll_pwr.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_tim.h"
#include "stm32f4xx_ll_usart.h"
#include "stm32f4xx.h"
#include "stm32f4xx_ll_gpio.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define BTN_R1_Pin LL_GPIO_PIN_2
#define BTN_R1_GPIO_Port GPIOE
#define BTN_D1_Pin LL_GPIO_PIN_3
#define BTN_D1_GPIO_Port GPIOE
#define BTN_L1_Pin LL_GPIO_PIN_4
#define BTN_L1_GPIO_Port GPIOE
#define BTN_M1_Pin LL_GPIO_PIN_5
#define BTN_M1_GPIO_Port GPIOE
#define BTN_X1_Pin LL_GPIO_PIN_6
#define BTN_X1_GPIO_Port GPIOE
#define Joy1Y_Pin LL_GPIO_PIN_0
#define Joy1Y_GPIO_Port GPIOA
#define Joy1X_Pin LL_GPIO_PIN_1
#define Joy1X_GPIO_Port GPIOA
#define Joy2X_Pin LL_GPIO_PIN_2
#define Joy2X_GPIO_Port GPIOA
#define Joy2Y_Pin LL_GPIO_PIN_3
#define Joy2Y_GPIO_Port GPIOA
#define TFT_SCK_Pin LL_GPIO_PIN_5
#define TFT_SCK_GPIO_Port GPIOA
#define TFT_LED_Pin LL_GPIO_PIN_6
#define TFT_LED_GPIO_Port GPIOA
#define TFT_SDA_Pin LL_GPIO_PIN_7
#define TFT_SDA_GPIO_Port GPIOA
#define TFT_RST_Pin LL_GPIO_PIN_4
#define TFT_RST_GPIO_Port GPIOC
#define TFT_DC_Pin LL_GPIO_PIN_5
#define TFT_DC_GPIO_Port GPIOC
#define BTN_X2_Pin LL_GPIO_PIN_7
#define BTN_X2_GPIO_Port GPIOE
#define BTN_X3_Pin LL_GPIO_PIN_8
#define BTN_X3_GPIO_Port GPIOE
#define BTN_X4_Pin LL_GPIO_PIN_9
#define BTN_X4_GPIO_Port GPIOE
#define BTN_JS2_Pin LL_GPIO_PIN_10
#define BTN_JS2_GPIO_Port GPIOE
#define BTN_D2_Pin LL_GPIO_PIN_11
#define BTN_D2_GPIO_Port GPIOE
#define BTN_R2_Pin LL_GPIO_PIN_12
#define BTN_R2_GPIO_Port GPIOE
#define BTN_M2_Pin LL_GPIO_PIN_13
#define BTN_M2_GPIO_Port GPIOE
#define BTN_U2_Pin LL_GPIO_PIN_14
#define BTN_U2_GPIO_Port GPIOE
#define BTN_L2_Pin LL_GPIO_PIN_15
#define BTN_L2_GPIO_Port GPIOE
#define APU_NSS_Pin LL_GPIO_PIN_8
#define APU_NSS_GPIO_Port GPIOD
#define APU_IRQ_Pin LL_GPIO_PIN_9
#define APU_IRQ_GPIO_Port GPIOD
#define APU_RST_Pin LL_GPIO_PIN_10
#define APU_RST_GPIO_Port GPIOD
#define APU_IO1_Pin LL_GPIO_PIN_11
#define APU_IO1_GPIO_Port GPIOD
#define APU_IO2_Pin LL_GPIO_PIN_12
#define APU_IO2_GPIO_Port GPIOD
#define LED1_Pin LL_GPIO_PIN_13
#define LED1_GPIO_Port GPIOD
#define LED2_Pin LL_GPIO_PIN_14
#define LED2_GPIO_Port GPIOD
#define LED3_Pin LL_GPIO_PIN_15
#define LED3_GPIO_Port GPIOD
#define BTN_JS1_Pin LL_GPIO_PIN_0
#define BTN_JS1_GPIO_Port GPIOE
#define BTN_U1_Pin LL_GPIO_PIN_1
#define BTN_U1_GPIO_Port GPIOE
#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif
/* USER CODE BEGIN Private defines */
#define gpio_port(X) X ## _GPIO_Port, X ## _Pin
#define gpio_set(X) LL_GPIO_SetOutputPin(X ## _GPIO_Port, X ## _Pin)
#define gpio_reset(X) LL_GPIO_ResetOutputPin(X ## _GPIO_Port, X ## _Pin)
#define gpio_write(X, W) MODIFY_REG((X ## _GPIO_Port), X ## _Pin, (X ## _Pin) & (-(W)))
#define gpio_toggle(X) LL_GPIO_TogglePin(X ## _GPIO_Port, X ## _Pin)
#define gpio_read(X) (READ_BIT((X ## _GPIO_Port)->IDR, (X ## _Pin)) == (X ## _Pin))
#define gpio_readn(X) (READ_BIT((X ## _GPIO_Port)->IDR, (X ## _Pin)) == 0)

extern volatile uint32_t __ticks, __delay;
extern volatile uint8_t __tickf;

#ifndef always_inlined
#define always_inlined __attribute__( ( always_inline ) ) __STATIC_INLINE
#endif

always_inlined uint32_t get_ticks(void) {
  return __ticks;
}
always_inlined void wait_tick_tock(void) {
  while (__tickf);
  __tickf = 1;
}
always_inlined uint8_t tick_tock(void) {
  if (__tickf) return 0;
  else {
    __tickf = 1;
    return 1;
  }
}
always_inlined void delay(uint32_t t) {
	__delay = t;
	while (__delay);
}
always_inlined void ticks_init(void) {
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
