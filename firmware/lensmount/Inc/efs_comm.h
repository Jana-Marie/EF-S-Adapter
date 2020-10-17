#ifndef __EFS_COMM_H
#define __EFS_COMM_H

#include "main.h"

void efs_delay(uint32_t delay);
void efs_wait();
uint8_t reverse(uint8_t data);
void efs_send(uint8_t data);
uint8_t efs_receive();
uint8_t efs_sr(uint8_t data);

#endif
