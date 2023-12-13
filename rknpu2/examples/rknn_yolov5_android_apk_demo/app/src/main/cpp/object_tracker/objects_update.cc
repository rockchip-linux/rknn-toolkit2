#include "stdio.h"
#include "objects_update.h"
#include "track_link.h"

OdtDetector::OdtDetector()
{  
	printf("success build\n");
}

OdtDetector::~OdtDetector()
{
}

int OdtDetector::update(int maxTrackLifetime, int track_num_input, object_T* object_input,
					int* track_num_output, object_T* object_output, int width, int height)
{

  	m_width = width;
	m_height = height;

    std::vector<Rect_T> objects_Rect_T;
    std::vector<int> objects_class;
    std::vector<float> object_score;
	for (int i = 0; i < track_num_input; i++) {
		objects_Rect_T.push_back(object_input[i].r);
        objects_class.push_back(object_input[i].obj_class);
        object_score.push_back(object_input[i].score);
    }

    m_objects_tracker.updateTrackedObjects(objects_Rect_T, objects_class, object_score, maxTrackLifetime, m_width, m_height);
    objects_Rect_T.clear();
	objects_class.clear();
    object_score.clear();

    std::vector<ObjectsTracker::ExtObject> extObjects;
    m_objects_tracker.getObjects(extObjects);
    
	int nobjects = (int)extObjects.size();
	track_num_output[0] = nobjects;
	
    int i = 0;
    for (; i < nobjects && i < 100; i++)
    {
		if(extObjects[i].miss == 0){
	        //object_output[i].r = extObjects[i].location;
            object_output[i].r = extObjects[i].smooth_rect;

		}
		else{
			object_output[i].r = extObjects[i].predict_loc_when_miss;
            //object_output[i].score = -10000;
		}
		object_output[i].obj_class = extObjects[i].obj_class;
        object_output[i].score = extObjects[i].score;
	    object_output[i].id = extObjects[i].id;

    }
    return nobjects;
}
