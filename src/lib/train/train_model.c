#include <lib/train/train_model.h>

unsigned int usqrt(int x)
{
	unsigned int a = 0L;
	unsigned int r = 0L;
	unsigned int e = 0L;

	int i;
	for (i = 0; i < BITSPERINT; i++)
	{
	    r = (r << 2) + TOP2BITS(x); x <<= 2;
	    a <<= 1;
	    e = (a << 1) + 1;
	    if (r >= e)
	    {
	          r -= e;
	          a++;
	    }
	}

	return a/65535;
}

int mean(int * list, int size){
	int sum = 0;
	int i;
	for(i = 0; i < size; i++){
		sum += list[i];
	}

	return (sum/size);
}

int standard_deviation(int *list, int size){
	int diff;
	int inter_sum = 0;
	int avg = mean(list, size);

	int i;
	for(i = 0; i < size; i++){
		diff = list[i] - avg;
		inter_sum += diff * diff;
	}

	inter_sum = inter_sum / (size - 1);

	return usqrt(inter_sum);
}

// ============= FULL INTERPOLATION ================//
int get_lagrange_basis(TrainModel *tm, int point, int x){
	int frac = tm->y[point];
	int i;
	for(i = 0; i < TRAIN_MODEL_SIZE; i++){
		if(i != point){
			frac *= (x - tm->x[i]);
			frac /= (tm->x[point] - tm->x[i]);
		}
	}
	
	return frac;
}


int interpolate(TrainModel *tm, int setting){
	int val = 0;
	int i;
	for(i = 0; i < TRAIN_MODEL_SIZE; i++){
		val += get_lagrange_basis(tm, i, setting);
	}
	return val;
}

// ============= PARTIAL INTERPOLATION ================//

int get_lagrange_basis_partial(TrainModel *tm, int point, int x, int head, int tail){
	int frac = tm->y[point];
	int i;
	for(i = head; i < tail; i++){
		if(i != point){
			frac *= (x - tm->x[i]);
			frac /= (tm->x[point] - tm->x[i]);
		}
	}
	
	return frac;
}

//Non-inclusive tail
int interpolatePartial(TrainModel *tm, int setting, int head, int tail){
	int val = 0;
	int i;

#ifdef X86
	assert(head >= 0 && head < tail);
	assert(tail > head && tail <= TRAIN_MODEL_SIZE);
#endif

	for(i = head; i < tail; i++){
		val += get_lagrange_basis_partial(tm, i, setting, head, tail);
	}
	return val;	
}

// TRAIN MEASURES //////////////////////////////////////////
void getStoppingDistanceModel(TrainModel *tm, int train_num){
	if(train_num == 24){
		int gear[] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ,100, 110, 120, 130, 140};
		int stp_dist[] = {0, 0, 0, 0, 0, 13564, 34176, 94045, 168608, 277890, 402523, 570764, 730057, 954537, 1215350};
		memcpy(tm->x, gear, sizeof(gear));
		memcpy(tm->y, stp_dist, sizeof(stp_dist));
	}
	else{
#ifndef X86
		assert(0 && "Attempting to use uncalibrated train!");
#endif
	}
}

void getVelocityModel(TrainModel *tm, int train_num){
	if(train_num == 24){
		int gear[] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ,100, 110, 120, 130, 140};
		int velo[] = {0, 100, 410, 870, 1340, 1360, 1670, 2220, 2712, 3381, 4063, 4785, 5337, 5936, 6728};
		memcpy(tm->x, gear, sizeof(gear));
		memcpy(tm->y, velo, sizeof(velo));
	}
	else{
#ifndef X86
		assert(0 && "Attempting to use uncalibrated train!");
#endif
	}
}

