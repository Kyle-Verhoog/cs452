#include <lib/train/train_model.h>

int abs(int x){
	if(x < 0){
		x = x * -1;
	}
	return x;
}

int zero_limit(int x){
	if (x < 0){
		x = 0;
	}
	return x;
}

int cap(int x, int cap){
	if(x > cap){
		x = cap;
	}
	return x;
}

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

int linear_interpolate(TrainModel *tm, int setting){
	int floor, lower, upper, frac, result;
	floor = setting/10 * 10;
	frac = setting - floor; //Ensure to divide by 10

	lower = tm->y[floor/10];
	upper = tm->y[floor/10 + 1];

	result = lower + (upper - lower)*frac/10;
	assert(result <= upper);
	return result;
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
	int i, floor;

#ifndef X86
	assert(head >= 0 && head < tail);
	assert(tail > head && tail <= TRAIN_MODEL_SIZE);
#endif

	for(i = head; i < tail; i++){
		val += get_lagrange_basis_partial(tm, i, setting, head, tail);
	}

	//Ensure setting lies between the two points
	floor = setting/10 * 10;
	if(floor != setting){
		if(val < tm->y[floor/10] || val > tm->y[floor/10 + 1]){
			//Fallback to linear interpolation
			val = linear_interpolate(tm, setting);
		}
	}

	val = zero_limit(val);	//The zero'd values stay zero

	return val;	
}

int easyInterpolation(TrainModel *tm, int setting){
	if(setting > TRAIN_MODEL_SIZE*(10/2)){ //Greater than half way
		return interpolatePartial(tm, setting, 8, TRAIN_MODEL_SIZE);
	}
	else{
		return interpolatePartial(tm, setting, 4, 10);	
	}
}

//====================================================================//

//Estimate Gear: ONLY USE ON LINEAR-ish PROGRESSIONS (non-linear)
int estimateGear(int *gear, int *model, int point){
	int slope, num_data, offset, res, i, lower, upper;

	num_data = 0;
	slope = 0;

	//Determine if its upper half or lower half of graph
	if(point > model[TRAIN_MODEL_SIZE/2]){
		lower = TRAIN_MODEL_SIZE/2 + 1;
		upper = TRAIN_MODEL_SIZE;
	}
	else{
		lower = 0;
		upper = TRAIN_MODEL_SIZE/2;	
	}

	//Calculate Slope
	for(i = 0; i < TRAIN_MODEL_SIZE - 1; i++){
		int local = (model[i+1] - model[i])/(gear[i+1] - gear[i]);
#ifndef X86
	assert(local >= 0);
#endif	
		//Drop data is slope is 0
		if(local > 0){
			num_data++;
			slope += local;
		}
		else{
			i++; //Ignore both model values
		}
	}

	slope = slope/num_data;	//Calculate averaged slope

	num_data = 0;
	offset = 0;

	//Find best offset
	for(i = 0; i < TRAIN_MODEL_SIZE; i++){
		int y = model[i];
		//Only consider the non-zero points
		if(y > 0){
			offset += y - (slope * gear[i]);
			num_data++;
		}else{
			i++; //Ignore both model values
		}
	}

	offset = offset/num_data; //caluclate averaged offset

	res = cap((point - offset)/slope, gear[TRAIN_MODEL_SIZE - 1]);
#ifndef X86
	assert(res >= 0);
#endif

	return res;
}


// TRAIN MEASURES //////////////////////////////////////////
void getStoppingDistanceModel(TrainModel *tm, int train_num){
  if (train_num == 1) {
		int gear[] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ,100, 110, 120, 130, 140};
		int stp_dist[] = {0, 0, 0, 0, 0, 13564, 34176, 94045, 168608, 277890, 402523, 570764, 730057, 954537, 1215350};
    tm->train_num = 1;
		memcpy(tm->x, gear, sizeof(gear));
		memcpy(tm->y, stp_dist, sizeof(stp_dist));
  }
  else if(train_num == 77){
		memcpy(tm, &TR_77_STP_DIST, sizeof(TR_77_STP_DIST));
	}
	else if(train_num == 78){
		memcpy(tm, &TR_78_STP_DIST, sizeof(TR_78_STP_DIST));	
	}
	else if(train_num == 79){
		memcpy(tm, &TR_79_STP_DIST, sizeof(TR_79_STP_DIST));	
	}
	else{
#ifndef X86
		assert(0 && "Attempting to use uncalibrated train!");
#endif
	}
}

void getVelocityModel(TrainModel *tm, int train_num){
  // trains for testing
  if (train_num == 1 || train_num == 2 || train_num == 3) {
    int gear[] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ,100, 110, 120, 130, 140};
    int velo[] = {0, 100, 400, 900, 1300, 1400, 1700, 2200, 2700, 3400, 4000, 4800, 5300, 6000, 6700};
    memcpy(tm->x, gear, sizeof(gear));
    memcpy(tm->y, velo, sizeof(velo));
  }
  else if(train_num == 77){
		memcpy(tm, &TR_77_VELO, sizeof(TR_77_VELO));
	}
	else if(train_num == 78){
		memcpy(tm, &TR_78_VELO, sizeof(TR_78_VELO));
	}
	else if(train_num == 79){
		memcpy(tm, &TR_79_VELO, sizeof(TR_79_VELO));	
	}
	else{
#ifndef X86
		assert(0 && "Attempting to use uncalibrated train!");
#endif
	}
}

void getAccelerationDistanceModel(TrainModel *tm, int train_num){
	if(train_num == 77){
		memcpy(tm, &TR_77_ACCEL_DIST, sizeof(TR_77_ACCEL_DIST));
	}
	else if(train_num == 78){
		memcpy(tm, &TR_78_ACCEL_DIST, sizeof(TR_78_ACCEL_DIST));
	}
	else if(train_num == 79){
		memcpy(tm, &TR_79_ACCEL_DIST, sizeof(TR_79_ACCEL_DIST));
	}
	else{
#ifndef X86
		assert(0 && "Attempting to use uncalibrated train!");
#endif
	}
}

int alphaUpdate(TrainModel *tm, int setting, int velocity){
	int uVelocity;
	int mVelocity = easyInterpolation(tm, setting);
	
	uVelocity = (velocity * ALPHA + mVelocity * (100 - ALPHA)) / 100;

	if(abs(uVelocity - mVelocity) > uVelocity / 4){	//TODO: Crude way to check within 25%
		return REJECTED;		
	}
	
	tm->y[setting] = uVelocity;
	return ACCEPTED;
}
