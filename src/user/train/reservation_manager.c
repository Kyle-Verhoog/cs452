#include <user/train/reservation_manager.h>


void ReservationManager() {
  int r;
  pather reserv;

  pather_init(&reserv, TRACK);

  r = RegisterAs(RESERVATION_MANAGER_ID);
  assert(r == 0);

  while (true) {
  }
}

