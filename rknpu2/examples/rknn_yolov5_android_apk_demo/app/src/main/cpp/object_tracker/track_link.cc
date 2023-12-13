#include "track_link.h"
#include "objects_update.h"
#include "objects_tracker.h"
#define MAX_OUTPUT 100

object_T object_input[MAX_OUTPUT];
object_T object_output[MAX_OUTPUT];

long create_tracker() {
    OdtDetector *object = new OdtDetector();
    return (long)object;
}

void destroy_tracker(long handle) {
    delete((OdtDetector*)handle);
}

void track(long handle, int maxTrackLifetime,
           int track_input_num, float * c_track_input_locations, int * c_track_input_class, float * c_track_input_score,
		   int * c_track_output_num, float * c_track_output_locations, int * c_track_output_class, float * c_track_output_score,
		   int * c_track_output_id, int width, int height){

    OdtDetector *object = (OdtDetector*)handle;

	for (int i = 0; i < track_input_num; i++) {
		object_input[i].r.x = (int)c_track_input_locations[i*4 +0];
		object_input[i].r.y = (int)c_track_input_locations[i*4 +1];
		object_input[i].r.width = (int)(c_track_input_locations[i*4 +2] -c_track_input_locations[i*4 +0]);
		object_input[i].r.height = (int)(c_track_input_locations[i*4 +3] -c_track_input_locations[i*4 +1]);
		object_input[i].score = c_track_input_score[i];
		object_input[i].obj_class = c_track_input_class[i];
		//LOGI("%d input P: %f\n", i, object_input[i].score);
	}

	object->update(maxTrackLifetime, track_input_num, object_input, c_track_output_num, object_output, width, height);

	for (int i = 0; i < *c_track_output_num; i++) {
		c_track_output_locations[i*4 +0] = (float)object_output[i].r.x;
        c_track_output_locations[i*4 +1] = (float)object_output[i].r.y;
        c_track_output_locations[i*4 +2] = (float)(object_output[i].r.x +object_output[i].r.width);
        c_track_output_locations[i*4 +3] = (float)(object_output[i].r.y +object_output[i].r.height);
		c_track_output_class[i] = object_output[i].obj_class;
		c_track_output_score[i] = object_output[i].score;
//        LOGI("output P: %f\n", c_track_output_score[i]);
		c_track_output_id[i] = object_output[i].id;
	}

	//LOGI("handle=%ld tracker input_num=%d output_num=%d maxTrackLifetime=%d width=%d height=%d\n",
	//        handle, track_input_num, *c_track_output_num, maxTrackLifetime, width, height);
}


































