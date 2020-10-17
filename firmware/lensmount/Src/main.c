
#include "main.h"
#include "lens.h"

PCD_HandleTypeDef hpcd_USB_FS;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USB_PCD_Init(void);

extern struct lensinfo_t lens;
extern uint8_t r_buf[40];
int8_t _tmp;

int main(void)
{

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USB_PCD_Init();

  lens_init();

  /*
  clock 80khz
  commands:
  0x9701 or 0x97CA returns 7 bytes (ID,Min/Max Zoom,Version,Brand)
  0xB0 returns 4 bytes (Min/Max Aperture)
  0xA0 returns 2 bytes (Current Zoom Value)
  0x0713xx returns 1 byte (0x11) xx set Aperture to xx stops
  0x1380 returns 1 byte (0x11) reopen Aperture
  0x0A busy poll (returns 0xAA if busy)
  0xC0 returns 2 bytes USM motor position
  0x05 turn focus ring to min
  0x06 turn focus ring to max
  0x44xxxx turn focus ring xxxxsteps relative (1s complement)
  */

  HAL_Delay(100);
  lens_get_info();
  /*
  lens_open_aperture();
  lens_set_aperture(-50);
  HAL_Delay(100);
  lens_focus_min();
  HAL_Delay(100);
  */
  lens_open_aperture();

  while (1)
  {

    for(int i = 0; i <= 20; i++){
      lens_dec_aperture();
      HAL_Delay(100);
    }
    HAL_Delay(1500);

    lens_focus_max();
    HAL_Delay(1500);

    lens_open_aperture();
    HAL_Delay(1500);

    lens_focus_max();
    for(int i = 0; i <= 120; i++){
      lens_move_focus(-30);
      HAL_Delay(30);
    }
    lens_focus_min();
    HAL_Delay(1500);
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;

  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_HSI48;

  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);
}


static void MX_USB_PCD_Init(void)
{
  hpcd_USB_FS.Instance = USB;
  hpcd_USB_FS.Init.dev_endpoints = 8;
  hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_FS.Init.battery_charging_enable = DISABLE;
  HAL_PCD_Init(&hpcd_USB_FS);
}

static void MX_GPIO_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}
