#include <user/ui/sensor_interface.h>

int min(int i, int j) { return i > j ? j : i; }

void SensorInterface() {
  tid_t recv_tid;
  Sensor *sensors;
  int i, reply, offset;
  char buf[256];
  tid_t tm_tid;
  rec_buffer rb;

  rec_buffer_init(&rb);

  tm_tid = WhoIs(TERMINAL_MANAGER_ID);
  assert(tm_tid >= 0);

  TMRegister(tm_tid, SENSOR_OFFSET_X, SENSOR_OFFSET_Y, SENSOR_WIDTH, SENSOR_HEIGHT);
  TMLogRegister(tm_tid);

  while (true) {
    Receive(&recv_tid, &sensors, sizeof(sensors));
    offset = 0;
    offset += buf_pack_c(buf+offset, TERM_RESET);

    for(i = 0; i < SENSOR_SIZE; i++){
      if(sensors[i].state){
        rec_buffer_add(&rb, i);
      }
    }

    //Print from Latest Sensor
    for(i = 0; i < min(rb.num, 9); i++){
      int val = rec_buffer_get(&rb, i);
      offset += buf_pack_c(buf+offset, val/16 + 'A');
      offset += buf_pack_i32(buf+offset, val%16 + 1);
      if ((val%16+1) < 10)
        offset += buf_pack_c(buf+offset, ' ');
      offset += buf_pack_c(buf+offset, ' ');
    }

    TMPutStr(tm_tid, buf, offset);
    Reply(recv_tid, &reply, sizeof(reply));
  }
}
