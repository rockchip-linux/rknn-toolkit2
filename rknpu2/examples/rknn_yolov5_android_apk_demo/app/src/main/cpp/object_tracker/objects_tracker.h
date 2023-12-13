#ifndef __OBJECTS_TRACKER_H_
#define __OBJECTS_TRACKER_H_

#include "stdio.h"
#include <vector>
#include "math.h"
#include "track_link.h"

class ObjectsTracker {

public:

    struct Parameters
    {
        int maxTrackLifetime;

        int numLastPositionsToTrack;
        int numDetectedToWaitBeforeFirstShow;
        int numStepsToWaitBeforeFirstShow;
        int numStepsToTrackWithoutDetectingIfObjectHasNotBeenShown;
        int numStepsToShowWithoutDetecting;

        Parameters();
    };

    enum ObjectStatus
    {
        DETECTED_NOT_SHOWN_YET,
        DETECTED,
        DETECTED_TEMPORARY_LOST,
        WRONG_OBJECT
    };

    struct ExtObject
    {		
        int id;
        Rect_T location;
		Rect_T predict_loc_when_miss;
		Rect_T smooth_rect;
        ObjectStatus status;
		int detectedNum;
		int noDetectedNum;
		int obj_class;
		float score;
		int miss;

        ExtObject() {}
        ExtObject(int _id, Rect_T _location, Rect_T _predict_loc_when_miss, Rect_T _smooth_rect, ObjectStatus _status,
						int _detectedNum, int _noDetectedNum, int _obj_class, float _score, int _miss)
            :id(_id), location(_location), predict_loc_when_miss(_predict_loc_when_miss), smooth_rect(_smooth_rect), status(_status),
            		detectedNum(_detectedNum), noDetectedNum(_noDetectedNum), obj_class(_obj_class), score(_score), miss(_miss)
        {
        	
        }
		
    };

    ObjectsTracker();
    virtual ~ObjectsTracker();

  /*  bool setParameters(const Parameters& params);
    const Parameters& getParameters() const;*/

    virtual void getObjects(std::vector<ExtObject>& result);

    //virtual int addObject(const Rect_T& location); //returns id of the new object

    void updateTrackedObjects(const std::vector<Rect_T>& detectedObjects, const std::vector<int> objects_class, const std::vector<float> objects_score, int maxTrackLifetime, int image_width, int image_height);
  	Rect_T calcTrackedObjectPositionToShow(int i, ObjectStatus& status) const;
    Parameters parameters;

    struct TrackedObject
    {
        typedef std::vector<Rect_T> PositionsVector;
        PositionsVector lastPositions;
		Rect_T predict_loc_when_miss;
        Rect_T smooth_Positionn;
		float vx;
		float vy;
        int numDetectedFrames;
        int numFramesDetected;
        int numFramesNotDetected;
		int preNumFramesNotDetected;
        int id;
		int obj_class;
		float score;
		int miss;
		//int flag_unshift;*/
        TrackedObject(const Rect_T& Rect_T):vx(0), vy(0), numDetectedFrames(1), numFramesDetected(1), numFramesNotDetected(0)
        							, preNumFramesNotDetected(0), score(0), miss(0)
			
        {
            lastPositions.push_back(Rect_T);
            id=getNextId();
        };

        static int getNextId()
        {
            static int _id=0;
            return _id++;
        }
    };

    int numTrackedSteps;
    std::vector<TrackedObject> trackedObjects;
    void predict_loctation(TrackedObject& curObject, int image_width,int image_height, float& pre_x, float& pre_y, float& v_x, float& v_y);

};

#endif /* end of __OBJECTS_TRACKER_H_ */
