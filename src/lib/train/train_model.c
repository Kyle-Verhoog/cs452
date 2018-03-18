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
