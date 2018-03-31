#include <test/train/train_model.h>

void train_model_velo_tests(){
	TrainModel tm;
	int train_num;

	train_num = 24;
	getVelocityModel(&tm, train_num);

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
	// PRINTF("%d\n", interpolatePartial(&tm, 95, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 90, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 85, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 80, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 75, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 70, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 65, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 60, 6, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 55, 0, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 50, 0, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 45, 0, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 40, 0, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 35, 0, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 30, 0, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 25, 0, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 20, 0, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 15, 0, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 10, 0, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 5, 0, 2));
	// PRINTF("%d\n", interpolatePartial(&tm, 0, 0, 2));

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

	assert(interpolatePartial(&tm, 80, 6, 15) > interpolatePartial(&tm, 75, 6, 15) &&
			interpolatePartial(&tm, 75, 6, 15) > interpolatePartial(&tm, 70, 6, 15) );

	assert(interpolatePartial(&tm, 70, 6, 15) > interpolatePartial(&tm, 65, 6, 15) &&
			interpolatePartial(&tm, 65, 6, 15) > interpolatePartial(&tm, 60, 6, 15) );

	assert(interpolatePartial(&tm, 60, 6, 15) > interpolatePartial(&tm, 55, 0, 10) &&
			interpolatePartial(&tm, 55, 0, 10) > interpolatePartial(&tm, 50, 0, 10) );
}

void train_model_stpdist_tests(){
	TrainModel tm;
	int train_num;

	train_num = 24;
	getStoppingDistanceModel(&tm, train_num);

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

	// PRINTF("%d\n", interpolatePartial(&tm, 140, 8, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 139, 8, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 138, 8, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 137, 8, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 136, 8, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 135, 8, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 134, 8, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 133, 8, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 132, 8, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 131, 8, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 130, 8, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 125, 8, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 120, 8, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 115, 8, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 110, 8, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 105, 8, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 100, 8, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 95, 8, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 90, 8, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 85, 8, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 80, 8, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 75, 4, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 70, 4, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 65, 4, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 60, 4, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 55, 4, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 50, 4, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 45, 4, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 40, 4, 10));

	assert(interpolatePartial(&tm, 140, 8, 15) > interpolatePartial(&tm, 139, 8, 15) &&
			interpolatePartial(&tm, 139, 8, 15) > interpolatePartial(&tm, 138, 8, 15) );
	assert(interpolatePartial(&tm, 139, 8, 15) > interpolatePartial(&tm, 138, 8, 15) &&
			interpolatePartial(&tm, 138, 8, 15) > interpolatePartial(&tm, 137, 8, 15) );
	assert(interpolatePartial(&tm, 138, 8, 15) > interpolatePartial(&tm, 137, 8, 15) &&
			interpolatePartial(&tm, 137, 8, 15) > interpolatePartial(&tm, 136, 8, 15) );
	assert(interpolatePartial(&tm, 137, 8, 15) > interpolatePartial(&tm, 136, 8, 15) &&
			interpolatePartial(&tm, 136, 8, 15) > interpolatePartial(&tm, 135, 8, 15) );
	assert(interpolatePartial(&tm, 136, 8, 15) > interpolatePartial(&tm, 135, 8, 15) &&
			interpolatePartial(&tm, 135, 8, 15) > interpolatePartial(&tm, 134, 8, 15) );

	assert(interpolatePartial(&tm, 130, 8, 15) > interpolatePartial(&tm, 129, 8, 15) &&
			interpolatePartial(&tm, 129, 8, 15) > interpolatePartial(&tm, 128, 8, 15) );
	assert(interpolatePartial(&tm, 129, 8, 15) > interpolatePartial(&tm, 128, 8, 15) &&
			interpolatePartial(&tm, 128, 8, 15) > interpolatePartial(&tm, 127, 8, 15) );
	assert(interpolatePartial(&tm, 128, 8, 15) > interpolatePartial(&tm, 127, 8, 15) &&
			interpolatePartial(&tm, 127, 8, 15) > interpolatePartial(&tm, 126, 8, 15) );
	assert(interpolatePartial(&tm, 127, 8, 15) > interpolatePartial(&tm, 126, 8, 15) &&
			interpolatePartial(&tm, 126, 8, 15) > interpolatePartial(&tm, 125, 8, 15) );
	assert(interpolatePartial(&tm, 126, 8, 15) > interpolatePartial(&tm, 125, 8, 15) &&
			interpolatePartial(&tm, 125, 8, 15) > interpolatePartial(&tm, 124, 8, 15) );

	assert(interpolatePartial(&tm, 120, 8, 15) > interpolatePartial(&tm, 115, 8, 15) &&
			interpolatePartial(&tm, 115, 8, 15) > interpolatePartial(&tm, 110, 8, 15) );

	assert(interpolatePartial(&tm, 110, 8, 15) > interpolatePartial(&tm, 105, 8, 15) &&
			interpolatePartial(&tm, 105, 8, 15) > interpolatePartial(&tm, 100, 8, 15) );

	assert(interpolatePartial(&tm, 100, 8, 15) > interpolatePartial(&tm, 95, 8, 15) &&
			interpolatePartial(&tm, 95, 8, 15) > interpolatePartial(&tm, 90, 8, 15) );

	assert(interpolatePartial(&tm, 90, 8, 15) > interpolatePartial(&tm, 85, 8, 15) &&
			interpolatePartial(&tm, 85, 8, 15) > interpolatePartial(&tm, 80, 8, 15) );

	assert(interpolatePartial(&tm, 80, 8, 15) > interpolatePartial(&tm, 75, 8, 15) &&
			interpolatePartial(&tm, 75, 8, 15) > interpolatePartial(&tm, 70, 8, 15) );

	assert(interpolatePartial(&tm, 70, 6, 10) > interpolatePartial(&tm, 65, 6, 10) &&
			interpolatePartial(&tm, 65, 6, 10) > interpolatePartial(&tm, 60, 6, 10) );	

	assert(interpolatePartial(&tm, 70, 6, 10) > interpolatePartial(&tm, 65, 6, 10) &&
			interpolatePartial(&tm, 65, 6, 10) > interpolatePartial(&tm, 60, 6, 10) );	

	assert(interpolatePartial(&tm, 60, 6, 10) > interpolatePartial(&tm, 55, 6, 10) &&
			interpolatePartial(&tm, 55, 6, 10) > interpolatePartial(&tm, 50, 6, 10) );	

	assert(interpolatePartial(&tm, 50, 6, 10) > interpolatePartial(&tm, 45, 6, 10) &&
			interpolatePartial(&tm, 45, 6, 10) > interpolatePartial(&tm, 40, 6, 10) );	
}	
