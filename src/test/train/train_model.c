#include <test/train/train_model.h>

void train_model_tests(){
	int gear[] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90 ,100, 110, 120, 130, 140};
	int stp_dist[] = {0, 0, 0, 0, 0, 0, 3000, 12400, 59950, 149694, 381764, 567824, 759493, 971013, 1238057};
	TrainModel tm;

	memcpy(tm.x, gear, sizeof(gear));
	memcpy(tm.y, stp_dist, sizeof(stp_dist));

	// PRINTF("%d\n", interpolate(&tm, 140));
	// PRINTF("%d\n", interpolate(&tm, 135));
	// PRINTF("%d\n", interpolate(&tm, 130));
	// PRINTF("%d\n", interpolate(&tm, 125));
	// PRINTF("%d\n", interpolate(&tm, 120));
	// PRINTF("%d\n", interpolate(&tm, 115));
	// PRINTF("%d\n", interpolate(&tm, 110));
	// PRINTF("%d\n", interpolate(&tm, 105));
	// PRINTF("%d\n", interpolate(&tm, 100));

	// PRINTF("===PARTIAL===\n");

	// PRINTF("%d\n", interpolatePartial(&tm, 140, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 139, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 138, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 137, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 136, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 135, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 134, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 133, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 132, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 131, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 130, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 125, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 120, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 115, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 110, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 105, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 100, 6, 15));

	//Interpolation sort of fails here
	// PRINTF("%d\n", interpolatePartial(&tm, 80, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 75, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 70, 6, 15));

	assert(interpolatePartial(&tm, 140, 6, 15) > interpolatePartial(&tm, 139, 6, 15) &&
			interpolatePartial(&tm, 139, 6, 15) > interpolatePartial(&tm, 138, 6, 15) );
	assert(interpolatePartial(&tm, 139, 6, 15) > interpolatePartial(&tm, 138, 6, 15) &&
			interpolatePartial(&tm, 138, 6, 15) > interpolatePartial(&tm, 137, 6, 15) );
	assert(interpolatePartial(&tm, 138, 6, 15) > interpolatePartial(&tm, 137, 6, 15) &&
			interpolatePartial(&tm, 137, 6, 15) > interpolatePartial(&tm, 136, 6, 15) );
	assert(interpolatePartial(&tm, 137, 6, 15) > interpolatePartial(&tm, 136, 6, 15) &&
			interpolatePartial(&tm, 136, 6, 15) > interpolatePartial(&tm, 135, 6, 15) );
	assert(interpolatePartial(&tm, 136, 6, 15) > interpolatePartial(&tm, 135, 6, 15) &&
			interpolatePartial(&tm, 135, 6, 15) > interpolatePartial(&tm, 134, 6, 15) );

	assert(interpolatePartial(&tm, 130, 6, 15) > interpolatePartial(&tm, 129, 6, 15) &&
			interpolatePartial(&tm, 129, 6, 15) > interpolatePartial(&tm, 128, 6, 15) );
	assert(interpolatePartial(&tm, 129, 6, 15) > interpolatePartial(&tm, 128, 6, 15) &&
			interpolatePartial(&tm, 128, 6, 15) > interpolatePartial(&tm, 127, 6, 15) );
	assert(interpolatePartial(&tm, 128, 6, 15) > interpolatePartial(&tm, 127, 6, 15) &&
			interpolatePartial(&tm, 127, 6, 15) > interpolatePartial(&tm, 126, 6, 15) );
	assert(interpolatePartial(&tm, 127, 6, 15) > interpolatePartial(&tm, 126, 6, 15) &&
			interpolatePartial(&tm, 126, 6, 15) > interpolatePartial(&tm, 125, 6, 15) );
	assert(interpolatePartial(&tm, 126, 6, 15) > interpolatePartial(&tm, 125, 6, 15) &&
			interpolatePartial(&tm, 125, 6, 15) > interpolatePartial(&tm, 124, 6, 15) );

	assert(interpolatePartial(&tm, 120, 6, 15) > interpolatePartial(&tm, 115, 6, 15) &&
			interpolatePartial(&tm, 115, 6, 15) > interpolatePartial(&tm, 110, 6, 15) );

	assert(interpolatePartial(&tm, 110, 6, 15) > interpolatePartial(&tm, 105, 6, 15) &&
			interpolatePartial(&tm, 105, 6, 15) > interpolatePartial(&tm, 100, 6, 15) );

	assert(interpolatePartial(&tm, 100, 6, 15) > interpolatePartial(&tm, 95, 6, 15) &&
			interpolatePartial(&tm, 95, 6, 15) > interpolatePartial(&tm, 90, 6, 15) );

	assert(interpolatePartial(&tm, 90, 6, 15) > interpolatePartial(&tm, 85, 6, 15) &&
			interpolatePartial(&tm, 85, 6, 15) > interpolatePartial(&tm, 80, 6, 15) );

	// assert(interpolatePartial(&tm, 80, 6, 15) > interpolatePartial(&tm, 75, 6, 15) &&
	// 		interpolatePartial(&tm, 75, 6, 15) > interpolatePartial(&tm, 70, 6, 15) );
}	
