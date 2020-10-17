#ifndef __LENS_H
#define __LENS_H
#include "efs_comm.h"

struct lensinfo_t {
  uint8_t id;
  uint8_t type;
  uint16_t min_focal_length;
  uint16_t max_focal_length;
  uint16_t cur_focal_length;
  uint16_t cur_focus;
  uint8_t min_aperture;
  uint8_t max_aperture;
  uint8_t cur_aperture;
  uint8_t c1;
  uint8_t c2;
  uint8_t af;
} lens;

uint8_t lens_busy_poll();
void lens_init();
void lens_focus_max();
void lens_focus_min();
void lens_open_aperture();
void lens_set_aperture(int8_t step);
void lens_get_info();
void lens_move_focus(int16_t step);
void lens_inc_focus();
void lens_dec_focus();
void lens_get_aperture();
void lens_dec_aperture();
void lens_inc_aperture();

#endif
