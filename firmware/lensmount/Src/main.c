
#include "main.h"
#include "lens.h"
#include "tusb.h"

void System_Config(void);
static void GPIO_Init(void);
void cdc_task(void);

extern struct lensinfo_t lens;
extern uint8_t r_buf[40];
int8_t _tmp;

int main(void)
{

  HAL_Init();
  System_Config();
  GPIO_Init();

	tud_init(BOARD_TUD_RHPORT);

/*
  lens_init();


  HAL_Delay(100);
  lens_get_info();

  HAL_Delay(1000);
  lens_focus_max();
  HAL_Delay(1000);
  lens_focus_min();
  HAL_Delay(1000);
*/

  while (1)
  {
    tud_task(); // tinyusb device task
    cdc_task();
  }
}

void System_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  RCC_CRSInitTypeDef RCC_CRSInitStruct = {0};

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

    /**Enable the SYSCFG APB clock 
    */
  __HAL_RCC_CRS_CLK_ENABLE();

    /**Configures CRS 
    */
  RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;
  RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_USB;
  RCC_CRSInitStruct.Polarity = RCC_CRS_SYNC_POLARITY_RISING;
  RCC_CRSInitStruct.ReloadValue = __HAL_RCC_CRS_RELOADVALUE_CALCULATE(48000000,1000);
  RCC_CRSInitStruct.ErrorLimitValue = 34;
  RCC_CRSInitStruct.HSI48CalibrationValue = 32;

  HAL_RCCEx_CRSConfig(&RCC_CRSInitStruct);

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_USB_CLK_ENABLE();
}

static void GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // data communication pins
  GPIO_InitStruct.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // voltage enable pins
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0 | GPIO_PIN_1, 0);

  // usb pins
  __HAL_REMAP_PIN_ENABLE(HAL_REMAP_PA11_PA12);
}

void USB_IRQHandler(void)
{
	tud_int_handler(0);
}

void cdc_task()
{
  if (tud_cdc_connected())
  {
    // connected and there are data available
    if (tud_cdc_available())
    {
      uint8_t buf[64];
      uint32_t count = tud_cdc_read(buf, sizeof(buf));

      uint32_t ptr = 0;
      while (ptr < count)
      {
        char cmd = buf[ptr++];
        if (cmd == 'r') {
          uint8_t response = efs_receive();
          tud_cdc_write_char(response);
        } else if (cmd == 's') {
          efs_send(buf[ptr++]);
        } else if (cmd == 'd') {
          efs_delay(buf[ptr++]);
        } else {
          tud_cdc_write_char('\n');
        }
      }

      tud_cdc_write_flush();
    }
  }
}

void Error_Handler(void)
{
  __disable_irq();
  while (1) {}
}
