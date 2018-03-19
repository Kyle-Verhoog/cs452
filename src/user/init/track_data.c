#include <user/init/track_data.h>

void TrackDataInit() {
  init_tracka(track_a);
  init_trackb(track_b);

  set_track_a();

#ifdef TRACK_A
  set_track_a();
#endif

#ifdef TRACK_B
  set_track_b();
#endif
  Exit();
}
