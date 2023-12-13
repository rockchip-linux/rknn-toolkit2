#ifndef __OBJECTS_UPDATE_H_
#define __OBJECTS_UPDATE_H_

#include "objects_tracker.h"

class OdtDetector
{
public:

    OdtDetector();
    virtual ~OdtDetector();
	int update(int maxTrackLifetime, int track_num_input, object_T* object_input,
					int* track_num_output, object_T* object_output, int width, int height);
    int getWidth() { return m_width; }
    int getHeight() { return m_height; }

private:

    int m_width;
    int m_height;
	
    ObjectsTracker m_objects_tracker;

};

#endif /* end of __ODT_DETECTOR_H_ */
