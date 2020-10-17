#include "lens.h"

uint8_t r_buf[40]; //tmp

float aperture_to_float(uint8_t aperture){
  return (aperture / 8) - 1;
}

void lens_get_info(){
  if(lens_busy_poll() == 0xAA){

    efs_send(0xA0);
    lens.cur_focal_length = (efs_receive() << 8); // handle
    lens.cur_focal_length |= efs_receive(); //  focal max

    efs_send(0xB0);
    lens.max_aperture = efs_receive(); //  max aperture
    lens.cur_aperture = efs_receive(); //  curr aperture (av = n/8-1)
    lens.min_aperture = efs_receive(); //  min aperture

    efs_send(0xB2);
    r_buf[5] = efs_receive(); //  some correction values?
    r_buf[6] = efs_receive(); //
    r_buf[7] = efs_receive(); // <- changes

    efs_send(0x90);
    r_buf[9] = efs_receive(); // 0 by now
    lens.af = (efs_receive() >> 7); //  7th bit 0 = AF, 7th bit 1 = MF

    efs_send(0xF0);
    r_buf[10] = efs_receive(); // <- changes, some correction values?

    lens_busy_poll();

    efs_send(0xC0);
    lens.cur_focus = (efs_receive() << 8);
    lens.cur_focus |= efs_receive();

    efs_send(0xC2); // does nothing
    r_buf[20] = efs_receive(); //
    r_buf[21] = efs_receive(); //
    r_buf[22] = efs_receive(); //
    r_buf[23] = efs_receive(); //

  }
}

void lens_focus_max(){
  if(lens_busy_poll() == 0xAA){
    efs_send(0x06);
    efs_receive(); // handle
  }
}

void lens_focus_min(){
  if(lens_busy_poll() == 0xAA){
    efs_send(0x05);
    efs_receive(); // handle
  }
}

void lens_inc_focus(){
  if(lens_busy_poll() == 0xAA){
    efs_send(0x44);
    efs_send(0xFF);
    efs_send(0xF9); // todo steps
    efs_receive(); // handle
  }
}

void lens_dec_focus(){
  if(lens_busy_poll() == 0xAA){
    efs_send(0x44);
    efs_send(0x00);
    efs_send(0x05); // todo steps
    efs_receive(); // handle
  }
}

void lens_move_focus(int16_t step){
  step = ~(step);
  if(lens_busy_poll() == 0xAA){
    efs_send(0x44);
    efs_send((step >> 8));
    efs_send((step & 0xFF)); // todo steps
    efs_receive(); // handle
  }
}

void lens_get_aperture(){
  if(lens_busy_poll() == 0xAA){
    efs_send(0xB0);
    lens.max_aperture = efs_receive(); //  max aperture
    lens.cur_aperture = efs_receive(); //  curr aperture (av = n/8-1)
    lens.min_aperture = efs_receive(); //  min aperture

    efs_send(0x91);
    r_buf[29] = efs_receive();
    r_buf[30] = efs_receive();
    r_buf[31] = efs_receive();


  }
}

void lens_open_aperture(){
  if(lens_busy_poll() == 0xAA){
    efs_send(0x13);
    efs_send(0x80);
    efs_receive(); // handle
  }
}


void lens_inc_aperture(){
  if(lens_busy_poll() == 0xAA){
    //efs_send(0x07);
    efs_send(0x13);
    efs_send(0xFF); // todo steps
    efs_receive(); // handle
  }
}

void lens_dec_aperture(){
  if(lens_busy_poll() == 0xAA){
    //efs_send(0x07);
    efs_send(0x13);
    efs_send(0x01); // todo steps
    efs_receive(); // handle
  }
}

void lens_set_aperture(int8_t step){
  step = ~(step);
  if(lens_busy_poll() == 0xAA){
    //efs_send(0x07);
    efs_send(0x13);
    efs_send(step); // todo steps
    efs_receive(); // handle
  }
}

void lens_init(){
  efs_send(0x00);   //sync
  efs_send(0x00);
  efs_send(0x00);
  efs_send(0x00);
  efs_send(0x00);
  efs_delay(1500);
  lens_busy_poll();   //sync
  lens_busy_poll();

  efs_send(0x09); //idc
  efs_send(0x00);
  efs_send(0x0E);
  efs_send(0x0F);

  lens_busy_poll();
  efs_send(0x0B);   // aperture init
  r_buf[24] = efs_receive();
  lens_busy_poll();
  efs_send(0x0C);   // focus init
  r_buf[25] = efs_receive();
  lens_busy_poll();
  efs_send(0x0D);   // focus init
  r_buf[26] = efs_receive();
  lens_busy_poll();
  efs_send(0x0E);   // focus init
  r_buf[27] = efs_receive();
  lens_busy_poll();
  efs_send(0x0F);   // focus init
  r_buf[28] = efs_receive();

  efs_send(0x80);

  lens.type = efs_receive(); //  (fixed, zoom, macro, soft focus ....) bit 5 = 1 for zoom lenses
  lens.id = efs_receive(); //  lens id
  lens.max_focal_length = (efs_receive() << 8); // handle
  lens.max_focal_length |= efs_receive(); //  focal max
  lens.min_focal_length = (efs_receive() << 8); // handle
  lens.min_focal_length |= efs_receive(); //  focal min
  lens.c1 = efs_receive(); // handle lens characteristics always 0?
  lens.c2 = efs_receive(); // handle lens characteristics always 0?

  //lens_open_aperture();
  lens_get_info();
}

uint8_t lens_busy_poll(){
  uint8_t ret;
  efs_send(0x0A);
  efs_delay(500);
  ret = efs_receive();
  efs_delay(1500);
  return ret;
}
