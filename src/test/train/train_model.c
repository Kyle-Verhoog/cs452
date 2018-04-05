#include <test/train/train_model.h>

void train_model_velo_tests(){
	TrainModel tm;
	int train_num;

	train_num = 77;
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
	// PRINTF("%d\n", interpolatePartial(&tm, 75, 8, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 70, 4, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 65, 4, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 60, 4, 15));
	// PRINTF("%d\n", interpolatePartial(&tm, 55, 4, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 50, 4, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 45, 4, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 40, 4, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 35, 4, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 30, 4, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 25, 4, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 20, 4, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 15, 4, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 10, 4, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 5, 4, 10));
	// PRINTF("%d\n", interpolatePartial(&tm, 0, 4, 10));

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

	assert(interpolatePartial(&tm, 80, 8, 15) > interpolatePartial(&tm, 75, 4, 10) &&
			interpolatePartial(&tm, 75, 4, 10) > interpolatePartial(&tm, 70, 4, 10) );

	assert(interpolatePartial(&tm, 70, 4, 10) > interpolatePartial(&tm, 65, 4, 10) &&
			interpolatePartial(&tm, 65, 4, 10) > interpolatePartial(&tm, 60, 4, 10) );

	assert(interpolatePartial(&tm, 60, 4, 10) > interpolatePartial(&tm, 55, 4, 10) &&
			interpolatePartial(&tm, 55, 4, 10) > interpolatePartial(&tm, 50, 4, 10) );
}

void train_model_stpdist_tests(){
	TrainModel tm;
	int train_num;

	train_num = 77;
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
	// PRINTF("%d\n", interpolatePartial(&tm, 75, 8, 15));
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
			interpolatePartial(&tm, 75, 4, 10) > interpolatePartial(&tm, 70, 4, 10) );

	assert(interpolatePartial(&tm, 70, 4, 10) > interpolatePartial(&tm, 65, 4, 10) &&
			interpolatePartial(&tm, 65, 4, 10) > interpolatePartial(&tm, 60, 4, 10) );	

	assert(interpolatePartial(&tm, 70, 4, 10) > interpolatePartial(&tm, 65, 4, 10) &&
			interpolatePartial(&tm, 65, 4, 10) > interpolatePartial(&tm, 60, 4, 10) );	

	assert(interpolatePartial(&tm, 60, 4, 10) > interpolatePartial(&tm, 55, 4, 10) &&
			interpolatePartial(&tm, 55, 4, 10) > interpolatePartial(&tm, 50, 4, 10) );	

	assert(interpolatePartial(&tm, 50, 4, 10) > interpolatePartial(&tm, 45, 4, 10) &&
			interpolatePartial(&tm, 45, 4, 10) > interpolatePartial(&tm, 40, 4, 10) );	
}

void train_model_acceldist_tests(){
	TrainModel tm;
	int train_num;

	train_num = 77;
	getAccelerationDistanceModel(&tm, train_num);

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
	//  PRINTF("%d\n", interpolatePartial(&tm, 80, 8, 15));
	//  PRINTF("%d\n", interpolatePartial(&tm, 75, 4, 10));
	//  PRINTF("%d\n", interpolatePartial(&tm, 70, 4, 10));
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

	assert(interpolatePartial(&tm, 80, 8, 15) > interpolatePartial(&tm, 75, 4, 10) &&
			interpolatePartial(&tm, 75, 4, 10) > interpolatePartial(&tm, 70, 4, 10) );

	assert(interpolatePartial(&tm, 70, 4, 10) > interpolatePartial(&tm, 65, 4, 10) &&
			interpolatePartial(&tm, 65, 4, 10) > interpolatePartial(&tm, 60, 4, 10) );	

	assert(interpolatePartial(&tm, 70, 4, 10) > interpolatePartial(&tm, 65, 4, 10) &&
			interpolatePartial(&tm, 65, 4, 10) > interpolatePartial(&tm, 60, 4, 10) );	

	assert(interpolatePartial(&tm, 60, 4, 10) > interpolatePartial(&tm, 55, 4, 10) &&
			interpolatePartial(&tm, 55, 4, 10) > interpolatePartial(&tm, 50, 4, 10) );	

	assert(interpolatePartial(&tm, 50, 4, 10) > interpolatePartial(&tm, 45, 4, 10) &&
			interpolatePartial(&tm, 45, 4, 10) > interpolatePartial(&tm, 40, 4, 10) );	
}


void train_model_ttoa_tests(){
	TrainModel tm;
	int train_num, gear, gear2;

	train_num = 79;
	getAccelerationDistanceModel(&tm, train_num);

	gear = estimateGear(tm.x, tm.t, 500);
	assert(estimateGear(tm.x, tm.t, 500) >= estimateGear(tm.x, tm.t, 450));
	//PRINTF("%d\n", gear);
	gear = estimateGear(tm.x, tm.t, 450);
	assert(estimateGear(tm.x, tm.t, 450) >= estimateGear(tm.x, tm.t, 440));
	//PRINTF("%d\n", gear);
	gear = estimateGear(tm.x, tm.t, 440);
	assert(estimateGear(tm.x, tm.t, 440) >= estimateGear(tm.x, tm.t, 430));
	//PRINTF("%d\n", gear);
	gear = estimateGear(tm.x, tm.t, 430);
	assert(estimateGear(tm.x, tm.t, 430) >= estimateGear(tm.x, tm.t, 420));
	//PRINTF("%d\n", gear);
	gear = estimateGear(tm.x, tm.t, 420);
	assert(estimateGear(tm.x, tm.t, 420) >= estimateGear(tm.x, tm.t, 410));
	//PRINTF("%d\n", gear);
	gear = estimateGear(tm.x, tm.t, 410);
	assert(estimateGear(tm.x, tm.t, 410) >= estimateGear(tm.x, tm.t, 400));
	//PRINTF("%d\n", gear);
	gear = estimateGear(tm.x, tm.t, 292);
	assert(estimateGear(tm.x, tm.t, 292) >= estimateGear(tm.x, tm.t, 291));
	//PRINTF("%d\n", gear);
	gear = estimateGear(tm.x, tm.t, 259);
	assert(estimateGear(tm.x, tm.t, 259) >= estimateGear(tm.x, tm.t, 254));
	//PRINTF("%d\n", gear);
	gear = estimateGear(tm.x, tm.t, 115);
	assert(estimateGear(tm.x, tm.t, 115) >= estimateGear(tm.x, tm.t, 100));
	//PRINTF("%d\n", gear);
	gear = estimateGear(tm.x, tm.t, 100);
	assert(estimateGear(tm.x, tm.t, 100) >= estimateGear(tm.x, tm.t, 99));
	//PRINTF("%d\n", gear);
	gear = estimateGear(tm.x, tm.t, 50);
	assert(estimateGear(tm.x, tm.t, 50) >= estimateGear(tm.x, tm.t, 0));
	//PRINTF("%d\n", gear);
	//gear = estimateGear(tm.x, tm.t, 0);
	//PRINTF("%d\n", gear);
}
