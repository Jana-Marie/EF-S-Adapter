from pathlib import Path
import serial

serialports = list(Path("/dev/").glob("tty.usbmodem*"))
if len(serialports) == 0:
    raise IOError("No suitable serial port found!")
print("opening", serialports[0])
port = serial.Serial(str(serialports[0]))
print("opened :)")

r_buf = [0] * 40  # TODO: get rid

def aperture_to_float(aperture):
  return (aperture / 8) - 1



def efs_send(byte):
    buffer = bytearray()
    buffer.append(ord(b's'))
    buffer.append(byte)
    port.write(buffer)

def efs_receive():
    port.write(b'r')
    return port.read(1)[0]


def efs_delay(time):
    pass
    #buffer = bytearray()
    #buffer.append(ord('d'))
    #buffer.append(time)
    #port.write(buffer)


def lens_busy_poll():
    efs_send(0x0A)
    efs_delay(500)
    ret = efs_receive()
    efs_delay(1500)
    return ret


def lens_init():
    efs_send(0x00)  # sync
    efs_send(0x00)
    efs_send(0x00)
    efs_send(0x00)
    efs_send(0x00)
    efs_delay(1500)
    lens_busy_poll()  # sync
    lens_busy_poll()

    efs_send(0x09)  # idc
    efs_send(0x00)
    efs_send(0x0E)
    efs_send(0x0F)

    lens_busy_poll()
    efs_send(0x0B)  # aperture init
    r_buf[24] = efs_receive()
    lens_busy_poll()
    efs_send(0x0C)  # focus init
    r_buf[25] = efs_receive()
    lens_busy_poll()
    efs_send(0x0D)  # focus init
    r_buf[26] = efs_receive()
    lens_busy_poll()
    efs_send(0x0E)  # focus init
    r_buf[27] = efs_receive()
    lens_busy_poll()
    efs_send(0x0F)  # focus init
    r_buf[28] = efs_receive()

    efs_send(0x80)

    lens_type = (
        efs_receive()
    )  #  (fixed, zoom, macro, soft focus ....) bit 5 = 1 for zoom lenses
    print(f"{lens_type=}")

    lens_id = efs_receive()  #  lens id
    print(f"{lens_id=}")
    lens_max_focal_length = efs_receive() << 8  # handle
    lens_max_focal_length |= efs_receive()  #  focal max
    print(f"{lens_max_focal_length=}")

    lens_min_focal_length = efs_receive() << 8  # handle
    lens_min_focal_length |= efs_receive()  #  focal min
    print(f"{lens_min_focal_length=}")
    lens_c1 = efs_receive()  # handle lens characteristics always 0?
    lens_c2 = efs_receive()  # handle lens characteristics always 0?

    # lens_open_aperture()
    lens_get_info()


def lens_get_info():
    if lens_busy_poll() == 0xAA:
        efs_send(0xA0)
        lens_cur_focal_length = efs_receive() << 8  # handle
        lens_cur_focal_length |= efs_receive()  #  focal max
        print(f"{lens_cur_focal_length=}")

        efs_send(0xB0)
        lens_max_aperture = aperture_to_float(efs_receive())  #  max aperture
        lens_cur_aperture = aperture_to_float(efs_receive())  #  curr aperture (av = n/8-1)
        lens_min_aperture = aperture_to_float(efs_receive())  #  min aperture
        print(f"{lens_max_aperture=} {lens_cur_aperture=} {lens_min_aperture=}")


        efs_send(0xB2)
        r_buf[5] = efs_receive()  #  some correction values?
        r_buf[6] = efs_receive()  #
        r_buf[7] = efs_receive()  # <- changes

        efs_send(0x90)
        r_buf[9] = efs_receive()  # 0 by now
        lens_af = efs_receive() >> 7  #  7th bit 0 = AF, 7th bit 1 = MF

        efs_send(0xF0)
        r_buf[10] = efs_receive()  # <- changes, some correction values?

        lens_busy_poll()

        efs_send(0xC0)
        lens_cur_focus = efs_receive() << 8
        lens_cur_focus |= efs_receive()

        efs_send(0xC2)  # does nothing
        r_buf[20] = efs_receive()  #
        r_buf[21] = efs_receive()  #
        r_buf[22] = efs_receive()  #
        r_buf[23] = efs_receive()  #


def lens_focus_max():
    if lens_busy_poll() == 0xAA:
        efs_send(0x06)
        efs_receive()  # handle


def lens_focus_min():
    if lens_busy_poll() == 0xAA:
        efs_send(0x05)
        efs_receive()  # handle


def lens_inc_focus():
    if lens_busy_poll() == 0xAA:
        efs_send(0x44)
        efs_send(0xFF)
        efs_send(0xF9)  # todo steps
        efs_receive()  # handle


def lens_dec_focus():
    if lens_busy_poll() == 0xAA:
        efs_send(0x44)
        efs_send(0x00)
        efs_send(0x05)  # todo steps
        efs_receive()  # handle


def lens_move_focus(step):
    step = ~(step)
    if lens_busy_poll() == 0xAA:
        efs_send(0x44)
        efs_send((step >> 8))
        efs_send((step & 0xFF))  # todo steps
        efs_receive()  # handle


def lens_get_aperture():
    if lens_busy_poll() == 0xAA:
        efs_send(0xB0)
        lens_max_aperture = efs_receive()  #  max aperture)
        lens_cur_aperture = efs_receive()  #  curr aperture (av = n/8-1)
        lens_min_aperture = efs_receive()  #  min aperture

        efs_send(0x91)
        r_buf[29] = efs_receive()
        r_buf[30] = efs_receive()
        r_buf[31] = efs_receive()


def lens_open_aperture():
    if lens_busy_poll() == 0xAA:
        efs_send(0x13)
        efs_send(0x80)
        efs_receive()  # handle


def lens_inc_aperture():
    if lens_busy_poll() == 0xAA:
        # efs_send(0x07)
        efs_send(0x13)
        efs_send(0xFF)  # todo steps
        efs_receive()  # handle


def lens_dec_aperture():
    if lens_busy_poll() == 0xAA:
        # efs_send(0x07)
        efs_send(0x13)
        efs_send(0x01)  # todo steps
        efs_receive()  # handle


def lens_set_aperture(step):
    step = ~(step)
    if lens_busy_poll() == 0xAA:
        # efs_send(0x07)
        efs_send(0x13)
        efs_send(step)  # todo steps
        efs_receive()  # handle
