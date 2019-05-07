/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "sdio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd_main.h"
#include "cpu.h"
#include "nes.h"
#include "imu.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	ticks_init();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_SPI2_Init();
  MX_USART2_UART_Init();
  MX_TIM13_Init();
  MX_DAC_Init();
  MX_TIM14_Init();
  MX_TIM12_Init();
  MX_TIM11_Init();
  MX_TIM10_Init();
  MX_TIM9_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_SDIO_SD_Init();
  /* USER CODE BEGIN 2 */
  tft_init(PIN_ON_LEFT, BLACK, WHITE, GREEN, RED);
  while(IMU_init() == 0);
  // IMU_enableGyro(50);
  IMU_enableRotation(50);
  // IMU_enableAccel(50);
  joystick_init();
  TIM6->PSC = 83;
  TIM6->ARR = 33333;
  TIM6->CR1 = TIM_CR1_CEN;

  TIM13->PSC = 8399;
  TIM13->ARR = 99;
  TIM13->CCR1 = 10;
  TIM13->CR1 = TIM_CR1_CEN;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
#if 0
    tft_clear();
    
    // tft_printc(0, 0, "Hello WorldHello WorldHello WorldHello WorldHello World");
    // tft_printi(0, 1, get_ticks());
    // tft_printi(0, 2, joystick_conv(joys[0].x));
    // tft_printi(0, 3, joystick_conv(joys[0].y));
    // tft_printi(0, 4, joystick_conv(joys[1].x));
    // tft_printi(0, 5, joystick_conv(joys[1].y));
    // tft_printb(0, 6, BUTTONS, 16);
    tft_printc(0, 0, "NES");
    cpu_debug_print(1);
    
    tft_update_dma();

    static uint16_t btn_state = 0;
    static uint32_t btn_clicked = 0;
    #define btn_pressed(X) (BUTTONS & X)
    #define btn_clicked(X) (BUTTONS & X) && !(btn_state & X)
    if (btn_clicked(BTN_R1)) {
      cpu_exec(1);
      btn_clicked = get_ticks();
    }
    if (btn_clicked(BTN_D1)) {
      nes_init();
    }
    if (btn_pressed(BTN_L1)) {
      cpu_exec(113);
    }
    btn_state = BUTTONS;

		if (tick_tock()) {
			static uint32_t last_ticks = 0;
			if (get_ticks() - last_ticks >= 100) {
				gpio_toggle(LED1);
				last_ticks = get_ticks();
			}

      if (((BUTTONS & BTN_R1)) && (get_ticks() - btn_clicked) > 1000) {
        cpu_exec(10);
      } else if (((BUTTONS & BTN_R1)) && (get_ticks() - btn_clicked) > 200) {
        cpu_exec(1);
      }
		}
#else
    static int GAME_SELECT = 0;
    #define NUM_OF_GAMES 3
    
    static uint16_t btn_state = 0;
    #define btn_pressed(X) (BUTTONS & (X))
    #define btn_clicked(X) (BUTTONS & (X)) && !(btn_state & (X))
    tft_clear();
    tft_printc(4, 0, "ELEC3300(24) - Anshuman & Aaron");
    tft_printc(6, 12, "SELECT");
    tft_prints(13, 12, "%s Super Mario Bros]", (GAME_SELECT == 0) ? ("[>") : ("-"));
    tft_prints(13, 13, "%s Battle City]"     , (GAME_SELECT == 1) ? ("[>") : ("-"));
    tft_prints(13, 14, "%s Mappy]"           , (GAME_SELECT == 2) ? ("[>") : ("-"));
    
    tft_printc(0, 5, " {/\\} \n{<}  {>}\n {\\/} ");
    // tft_printc(0, 5, " /\\ \n/  \\\n\\  /\n \\/ ");
    tft_printc(36, 5, " {A} \n{B} {A}\n {B} ");
    tft_prints(0,  14, "{SEL}");
    tft_prints(36, 14, "{STRT}");

//     tft_printc(5, 1, " __    __  ________   ______   \n\
// /  \\  /  |/        | /      \\ \n\
// [$$]  \\ [$$] |[$$$$$$$$]/ /[$$$$$$]  |\n\
// [$$$]  \\[$$] |[$$] |__    [$$] \\__[$$]/ \n\
// [$$$$]  [$$] |[$$]    |   [$$]      \\ \n\
// [$$] [$$] [$$] |[$$$$$]/     [$$$$$$]  |\n\
// [$$] |[$$$$] |[$$] |_____ /  \\__[$$] |\n\
// [$$] | [$$$] |[$$]       |[$$]    [$$]/ \n\
// [$$]/   [$$]/ [$$$$$$$$]/  [$$$$$$]/  ");
IMU_dataAvailable();
    tft_prints(6,10, "%sIMU Control]",(getIMUControl())?"[":"");
    // tft_printi(6,11,(int16_t)getAccelY()*10);
    if (btn_clicked(BTN_M1)) toggleIMUControl();
    // tft_printi(0, 1, get_ticks()%1000);
    // tft_printi(5,1,(int16_t)(getQuatI()*1000));
    // tft_printi(5,2,(int16_t)(getQuatJ()*1000));
    // tft_printi(5,3,(int16_t)(getQuatK()*1000));
    // tft_printi(5,4,(int16_t)(getQuatReal()*1000));
    tft_printi(10,1,(int16_t)(getRoll()*10));
    tft_printi(10,2,(int16_t)(getPitch()*10));
    tft_printi(10,3,(int16_t)(getYaw()*10));
    // tft_printi(5,2,(int16_t)(getAccelX()*1000));

    // tft_printi(5,1,getAccelLeftRight());
    // static uint8_t asdf = 0;
    // tft_printi(5,4,asdf+= getAccelFlick());

    tft_update();
    
    static uint32_t last_blink = 0;
    if ((get_ticks() - last_blink) > 100) {
      gpio_toggle(LED2);
      last_blink = get_ticks();
    }

    static uint32_t last_bright = 0;
    if ((get_ticks() - last_bright) > 50) {
      if (btn_pressed(BTN_X1)) TIM13->CCR1+= 3;
      TIM13->CCR1%= 100;
      last_bright = get_ticks();
    }


    if (btn_clicked(BTN_X2|BTN_D1|BTN_D2)) {
      GAME_SELECT++;
      GAME_SELECT %= 3;
    }
    if (btn_clicked(BTN_X3|BTN_U1|BTN_U2)) {
      GAME_SELECT--;
      if (GAME_SELECT<0) GAME_SELECT += 3;
    }
    if (btn_clicked(BTN_X4)) {
      nes_init(rom_select(GAME_SELECT));

      while (1) {
        static uint32_t last_bright = 0;
        if ((get_ticks() - last_bright2) > 50) {
          if (btn_pressed(BTN_X1)) TIM13->CCR1+= 3;
          TIM13->CCR1 %= 100;
          last_bright2 = get_ticks();
        }
        if (TIM6->SR & TIM_SR_UIF) {
          IMU_dataAvailable();
          TIM6->SR = 0;
          nes_frame(0);
          gpio_toggle(LED1);
          nes_frame(1);
          gpio_toggle(LED1);
          if (btn_clicked(BTN_X3)) cpu_reset();
          gpio_set(LED2);
        } else {
          gpio_reset(LED2);
        }
      }
    }
    btn_state = BUTTONS;

#endif
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
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_5);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_5)
  {
  Error_Handler();  
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {
    
  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE, LL_RCC_PLLM_DIV_8, 336, LL_RCC_PLLP_DIV_2);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {
    
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_4);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  
  }
  LL_Init1msTick(168000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(168000000);
}

/* USER CODE BEGIN 4 */
volatile uint32_t __ticks = 0;
volatile uint32_t __delay = 0;
volatile uint8_t  __tickf = 1;
void SysTick_Handler(void) {
	__ticks++;
	__tickf = 0;
	if (__delay) __delay--;
}
uint32_t HAL_GetTick(void)
{
  return __ticks;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
