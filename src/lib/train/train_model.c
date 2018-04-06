#include <lib/train/train_model.h>

// TRAIN DATA
static TrainModel TR_77_VELO = {
	77,
	{0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ,100, 110, 120, 130, 140},
	{0, 100, 410, 870, 1340, 1360, 1670, 2220, 2712, 3381, 4063, 4785, 5337, 5936, 6728}
};

static TrainModel TR_77_STP_DIST = {
	77,
	{0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ,100, 110, 120, 130, 140},
	{0, 0, 0, 0, 0, 13564, 34176, 94045, 168608, 277890, 402523, 570764, 730057, 954537, 1215350},
	{0, 0, 0, 0, 0, 20, 54, 84, 128, 156, 188, 224, 271, 294, 336}
};

static TrainModel TR_77_ACCEL_DIST = {
	77,
	{0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ,100, 110, 120, 130, 140},
	{0, 0, 0, 0, 0, 25580, 88240, 184186, 296202, 396204, 528630, 650541, 831305, 1014943, 1257763},
	{0, 0, 0, 0, 0, 50, 115, 185, 259, 292, 331, 357, 381, 420, 440}
};

//////////////////////////////////

// TRAIN DATA
static TrainModel TR_78_VELO = {
	78,
	{0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ,100, 110, 120, 130, 140},
	{0, 0, 0, 0, 246, 856, 1076, 1586, 1961, 2434, 2967, 3621, 4174, 4658, 4908}
};

static TrainModel TR_78_STP_DIST = {
	78,
	{0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ,100, 110, 120, 130, 140},
	{0, 0, 0, 0, 0, 34620, 75504, 114471, 178925, 242086, 326093, 439092, 607723, 755534, 955263},
	{0, 0, 0, 0, 0, 30, 60, 97, 124, 162, 189, 228, 274, 304, 327}
};

static TrainModel TR_78_ACCEL_DIST = {
	78,
	{0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ,100, 110, 120, 130, 140},
	{0, 0, 0, 0, 0, 73062, 139978, 170076, 264766, 335648, 496600, 592868, 686840, 783246, 898044},
	{0, 0, 0, 0, 0, 240, 280, 318, 356, 392, 430, 468, 500, 540, 595}
};

////////////////////////////////

// TRAIN DATA
static TrainModel TR_79_VELO = {
	79,
	{0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ,100, 110, 120, 130, 140},
	{0, 20, 100, 400, 662, 1071, 1680, 2255, 2785, 3389, 4083, 4684, 5392, 6128, 6707}
};

static TrainModel TR_79_STP_DIST = {
	79,
	{0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ,100, 110, 120, 130, 140},
	{0, 0, 0, 0, 0, 10984, 48299, 120840, 188456, 288503, 415125, 568893, 773016, 976814, 1242868},
	{0, 0, 0, 0, 0, 54, 84, 104, 134, 171, 198, 229, 255, 284, 337}
};

static TrainModel TR_79_ACCEL_DIST = {
	79,
	{0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ,100, 110, 120, 130, 140},
	{0, 0, 0, 0, 0, 25136, 92205, 197452, 333512, 433081, 588099, 718499, 864020, 1065962, 1259344},
	{0, 0, 0, 0, 0, 100, 155, 214, 261, 299, 332, 356, 387, 410, 440}
};

////////////////////////////////

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
		return interpolatePartial(tm, setting, 0, 10);	
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
		int time[] = {0, 0, 0, 0, 0, 20, 54, 84, 128, 156, 188, 224, 271, 294, 336};
    tm->train_num = 1;
		memcpy(tm->x, gear, sizeof(gear));
		memcpy(tm->y, stp_dist, sizeof(stp_dist));
		memcpy(tm->t, time, sizeof(time));
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
	//Trains for test
	if(train_num == 1 || train_num == 2 || train_num == 3){
		memcpy(tm, &TR_77_ACCEL_DIST, sizeof(TR_77_ACCEL_DIST));
	}
	else if(train_num == 77){
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

/////////////////////////////////////////////////////////

int trainUpdateDist(TrainModelSnapshot *tms, int train_num){
  int dist, new_gear;
  TrainModel acl_model;
  TrainModel stp_model;

  //Update the duration
  tms->duration += tms->elapsed;

  //Case Constant Velocity
  if(tms->start_gear == tms->end_gear){
    dist = tms->model.y[tms->start_gear/10] * tms->elapsed;
    tms->duration = 0;
  }
  //Case Acceleration
  else if(tms->start_gear < tms->end_gear){
    getAccelerationDistanceModel(&acl_model, train_num);
    //Check if still accelerating
    if(tms->duration < (acl_model.t[tms->end_gear/10] - acl_model.t[tms->start_gear/10])){
      new_gear = estimateGear(acl_model.x, acl_model.t, tms->duration);
      new_gear = new_gear > tms->end_gear ? tms->end_gear : new_gear; //Ensure we don't exceed bounds
      new_gear = new_gear < tms->cur_gear ? tms->cur_gear : new_gear;  //Ensure that we are always going up
    }
    else{
      new_gear = tms->end_gear;
      tms->start_gear = tms->end_gear;
    }

    dist = zero_limit(easyInterpolation(&acl_model, new_gear) - easyInterpolation(&acl_model, tms->cur_gear));
#ifndef X86
      assert(new_gear >= tms->cur_gear);
      assert(dist >= 0);
#endif    
    tms->cur_gear = new_gear;
  }
  //Case De-aceleration
  else{
    getStoppingDistanceModel(&stp_model, train_num);
    //Check if still stopping
    if(tms->duration < (stp_model.t[tms->start_gear/10] - stp_model.t[tms->end_gear/10])){
      new_gear = estimateGear(stp_model.x, stp_model.t, stp_model.t[tms->start_gear/10] - tms->duration);
      new_gear = new_gear < tms->end_gear ? tms->end_gear : new_gear; //Ensure that we are always going down
      new_gear = new_gear > tms->cur_gear ? tms->cur_gear : new_gear;
    }
    else{
	  tms->start_gear = tms->end_gear;
      new_gear = tms->end_gear;
    }
    dist = zero_limit(easyInterpolation(&stp_model, tms->cur_gear) - easyInterpolation(&stp_model, new_gear));
#ifndef X86
      assert(new_gear <= tms->cur_gear);
      assert(dist >= 0);
#endif    
    tms->cur_gear = new_gear;
  }

  return dist;  
}
