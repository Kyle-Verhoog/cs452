#include <writeback_interface.h>

void WritebackInterface(){
	int in_tid = WhoIs(1);
	int out_tid = WhoIs(1);

	//Keep track of Cursor - all other tasks should move cursor back here after use
	int cur_row = 1;
	int cur_col = 1;

	//Move cursor to starting location

	while(true){
		char c = Getc(in_tid, COM2);

		switch(c){
			case BACKSPACE:
				//Move cursor back twice
				Putc(out_tid, COM2, ' '); //Write Empty space
				//Move cursor back once
				break;
			case CARRIGE_RETURN:
				//Clear the line
				cur_col = 1;
				break;
			default:
				Putc(out_tid, COM2, c);
				break;
		}
	}

	Exit();
}