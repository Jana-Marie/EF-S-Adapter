#include "efs_comm.h"

void efs_delay(uint32_t delay) {
  while(delay--){
    asm("NOP");
  }
}

void efs_wait() {
  efs_delay(60);
  while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) == 0){ // Wait until lens is not busy anymore
    asm("NOP");
  }
}

uint8_t reverse(uint8_t data) {
   data = (data & 0xF0) >> 4 | (data & 0x0F) << 4;
   data = (data & 0xCC) >> 2 | (data & 0x33) << 2;
   data = (data & 0xAA) >> 1 | (data & 0x55) << 1;
   return data;
}

void efs_send(uint8_t data) {
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, 0);
  efs_sr(data);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, 1);
  efs_wait();
  efs_delay(100);
}

uint8_t efs_receive() {
  uint8_t rec = efs_sr(0x00);
  efs_wait();
  efs_delay(100);
  return rec;
}


uint8_t efs_sr(uint8_t data) {
  data = reverse(data);
  uint8_t rec = 0;
  uint8_t i;
  for(i = 0; i < 16; i++){
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, i & 1);
    if(i % 2 == 0){
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, (data >> (i/ 2)) & 1);
    } else {
      rec |= (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_7) << (((16-i))/ 2));
    }
    efs_delay(33);
  }
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, 0);
  return rec;
}
