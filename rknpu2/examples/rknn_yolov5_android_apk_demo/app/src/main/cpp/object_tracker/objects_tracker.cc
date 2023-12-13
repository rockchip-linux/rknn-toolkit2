#include "objects_tracker.h"
#include "track_link.h"

#define max(a,b)  (((a) > (b)) ? (a) : (b))
#define min(a,b)  (((a) < (b)) ? (a) : (b))

ObjectsTracker::ObjectsTracker() :
    parameters(),
    numTrackedSteps(0)
{
   
}

ObjectsTracker::~ObjectsTracker()
{

}

ObjectsTracker::Parameters::Parameters()
{
    maxTrackLifetime=6;
    numLastPositionsToTrack=4;
    numDetectedToWaitBeforeFirstShow=0;
    numStepsToWaitBeforeFirstShow=6;
    numStepsToTrackWithoutDetectingIfObjectHasNotBeenShown=4;
    numStepsToShowWithoutDetecting=5;
}

float CalculateIOU(Rect_T r1, Rect_T r2) {
	int xmin0 = r1.x;
	int ymin0 = r1.y;	
	int xmax0 = r1.x +r1.width;
	int ymax0 = r1.y +r1.height;
	int xmin1 = r2.x;
	int ymin1 = r2.y;	
	int xmax1 = r2.x +r2.width;
	int ymax1 = r2.y +r2.height;
    float w = max(0.f, min(xmax0, xmax1) - max(xmin0, xmin1));
    float h = max(0.f, min(ymax0, ymax1) - max(ymin0, ymin1));
    float i = w * h;
    float u = (xmax0 - xmin0) * (ymax0 - ymin0) + (xmax1 - xmin1) * (ymax1 - ymin1) - i;
    return u <= 0.f ? 0.f : (i / u);
}

float CalculateArea(Rect_T r) {
    float i = r.width * r.height;
    return i;
}

void ObjectsTracker::predict_loctation(TrackedObject& curObject, int image_width,int image_height, float& pre_x, float& pre_y, float& v_x, float& v_y){
    int numpositions = (int)curObject.lastPositions.size();
    Rect_T prevRect = curObject.lastPositions[numpositions-1];
    Rect_T tmpRect=curObject.lastPositions[numpositions-2];
    float vx_1 = (prevRect.x -tmpRect.x)/(1.0f +curObject.preNumFramesNotDetected);
    float vx_2 = (prevRect.x +prevRect.width -tmpRect.x -tmpRect.width)/(1.0f +curObject.preNumFramesNotDetected);
    float vx_ = min(vx_1, vx_2);
    float vy_1 = (prevRect.y -tmpRect.y)/(1.0f +curObject.preNumFramesNotDetected);
    float vy_2 = (prevRect.y +prevRect.height -tmpRect.y -tmpRect.height)/(1.0f +curObject.preNumFramesNotDetected);
    float vy_ = min(vy_1, vy_2);
    v_x = 0.5f*vx_ + 0.5f*curObject.vx;
    v_y = 0.5f*vy_ + 0.5f*curObject.vy;
    int x = (int)(prevRect.x  + v_x*(1 ) *0.7f +0.5f);
    int y = (int)(prevRect.y  + v_y*(1 ) *0.7f +0.5f);
    x = x >= 0 ? x : 0;
    y = y >= 0 ? y : 0;
    if(x + prevRect.width >= image_width)
        x -= (x + prevRect.width -image_width +1);
    if(y + prevRect.height >= image_height)
        y -= (y + prevRect.height -image_height +1);
    x = x >= 0 ? x : 0;
    y = y >= 0 ? y : 0;
    pre_x = x;
    pre_y = y;

}

void ObjectsTracker::getObjects(std::vector<ExtObject>& result)
{
    result.clear();
    for(size_t i=0; i < trackedObjects.size(); i++) {
        ObjectStatus status;
        Rect_T r=calcTrackedObjectPositionToShow((int)i, status);

		if (CalculateArea(r)==0.f || trackedObjects[i].numFramesDetected < 2){
            continue;
        }
		
        result.push_back(ExtObject(trackedObjects[i].id, r, trackedObjects[i].predict_loc_when_miss, trackedObjects[i].smooth_Positionn, status,
			trackedObjects[i].numFramesDetected, trackedObjects[i].numFramesNotDetected, trackedObjects[i].obj_class, trackedObjects[i].score, trackedObjects[i].miss));

	}
}

void ObjectsTracker::updateTrackedObjects(const std::vector<Rect_T>& detectedObjects, const std::vector<int> objects_class
        , const std::vector<float> objects_score, int maxTrackLifetime,  int image_width,int image_height)
{
    enum {
        NEW_RECTANGLE=-1,
        INTERSECTED_RECTANGLE=-2
    };

    int N1=(int)trackedObjects.size();
    int N2=(int)detectedObjects.size();

    for(int i=0; i < N1; i++) {
        trackedObjects[i].numDetectedFrames++;
    }
	
    std::vector<int> correspondence(detectedObjects.size(), NEW_RECTANGLE);
	std::vector<float> correspondenceScore(detectedObjects.size(), 0);
	
    for(int i=0; i < N1; i++) {
        TrackedObject& curObject=trackedObjects[i];
        int bestIndex=-1;
        float bestArea=-1;
        int numpositions=(int)curObject.lastPositions.size();
        Rect_T prevRect=curObject.lastPositions[numpositions-1];

		//save predict loctation
		if(numpositions>1)
		{
            float pre_x, pre_y, vx, vy;
            predict_loctation(curObject, image_width, image_height, pre_x, pre_y, vx, vy);
            curObject.vx = vx;
            curObject.vy = vy;
            prevRect.x = (int)pre_x;
            prevRect.y = (int)pre_y;
            curObject.predict_loc_when_miss = prevRect;
		}
		
		//search track loaction
        for(int j=0; j < N2; j++) {
            
			float percentage_IOU = CalculateIOU(prevRect, detectedObjects[j]);
            if ( percentage_IOU > 0.1f ) {//&& objects_class[j] ==  curObject.obj_class
				
				float trackScore = percentage_IOU *1.f / (curObject.numFramesNotDetected + 1);
                if ( percentage_IOU > bestArea && correspondenceScore[j] < trackScore) {
                    bestIndex = j;
                    bestArea = percentage_IOU;
					correspondenceScore[j] = trackScore;
                }
            }
			
        }
		if (bestIndex >= 0) {
			correspondence[bestIndex] = i;
		}	
    }

	//select track loaction
	for (int i = 0; i < N1; i++) {
		TrackedObject& curObject = trackedObjects[i];
		int bestIndex = -1;
		for (int j = 0; j < N2; j++) {
			if (correspondence[j] == i){
				bestIndex = j;
				break;
			}
		}
		if (bestIndex >= 0) {
			correspondence[bestIndex] = i;
			for (int j = 0; j < N2; j++) {
				if (correspondence[j] >= 0)
					continue;

				float percentage_IOU = CalculateIOU(detectedObjects[j], detectedObjects[bestIndex]);			
				if (percentage_IOU > 0.45f  ){//&& objects_class[j] == curObject.obj_class
					correspondence[j] = INTERSECTED_RECTANGLE;
				}
			}
			curObject.numFramesDetected++;
		}
		else {

			curObject.numFramesNotDetected++;
			curObject.miss = 1;
		}

	}

	//allocate new detected location
    for(int j=0; j < N2; j++) {
        int i = correspondence[j];
        if (i >= 0) {//add position
            trackedObjects[i].lastPositions.push_back(detectedObjects[j]);
            while ((int)trackedObjects[i].lastPositions.size() > (int) parameters.numLastPositionsToTrack) {
                trackedObjects[i].lastPositions.erase(trackedObjects[i].lastPositions.begin());
            }
			trackedObjects[i].preNumFramesNotDetected = trackedObjects[i].numFramesNotDetected;
            trackedObjects[i].numFramesNotDetected = 0;
            trackedObjects[i].score = objects_score[j];
            trackedObjects[i].obj_class = objects_class[j];
			trackedObjects[i].miss = 0;
            //smooth rect
            trackedObjects[i].smooth_Positionn.width = (trackedObjects[i].smooth_Positionn.width *1 +detectedObjects[j].width )/(2);
            trackedObjects[i].smooth_Positionn.height = (trackedObjects[i].smooth_Positionn.height *1 +detectedObjects[j].height)/(2);
            Rect_T r_smooth = trackedObjects[i].smooth_Positionn;
            float weight_p = 0.5f, weight_n = 1.f;
            trackedObjects[i].smooth_Positionn.x = (int)(((r_smooth.x +r_smooth.width*0.5f) *weight_p +(detectedObjects[j].x +detectedObjects[j].width*0.5f)*weight_n)/(weight_p +weight_n)
                                                         +trackedObjects[i].vx *weight_p /(weight_p +weight_n) -r_smooth.width *0.5f);

            trackedObjects[i].smooth_Positionn.y = (int)(((r_smooth.y +r_smooth.height*0.5f) *weight_p +(detectedObjects[j].y +detectedObjects[j].height*0.5f)*weight_n)/(weight_p +weight_n)
                                                         +trackedObjects[i].vy *weight_p /(weight_p +weight_n)-r_smooth.height *0.5f);
		} else if (i==NEW_RECTANGLE){ //new object
			
            trackedObjects.push_back(detectedObjects[j]);		
			int _N2 = (int)trackedObjects.size();
			trackedObjects[_N2-1].obj_class = objects_class[j];
            trackedObjects[_N2-1].score = objects_score[j];
			correspondence[j] = _N2 -1;
            trackedObjects[_N2-1].smooth_Positionn.width = detectedObjects[j].width;
            trackedObjects[_N2-1].smooth_Positionn.height = detectedObjects[j].height;
            trackedObjects[_N2-1].smooth_Positionn.x = (detectedObjects[j].x);
            trackedObjects[_N2-1].smooth_Positionn.y = (detectedObjects[j].y);
        }
    }

    std::vector<TrackedObject>::iterator it=trackedObjects.begin();
    while( it != trackedObjects.end() ) {
        if ( (it->numFramesNotDetected > maxTrackLifetime)
#if 0
                ||
                (
                 (it->numDetectedFrames <= parameters.numStepsToWaitBeforeFirstShow)
                 &&
                 (it->numFramesNotDetected > parameters.numStepsToTrackWithoutDetectingIfObjectHasNotBeenShown)
                )
#endif
           )
        {
            it=trackedObjects.erase(it);
			
        } else {
            it++;
        }
		
    }

}

Rect_T ObjectsTracker::calcTrackedObjectPositionToShow(int i, ObjectStatus& status) const
{
   Rect_T r;
   r.x = 0;
   r.y = 0;
   r.width = 0;
   r.height = 0;

    if ( (i < 0) || (i >= (int)trackedObjects.size()) ) {
        status = WRONG_OBJECT;
       
        return r;
    }

#if 0
    if (trackedObjects[i].numDetectedFrames <= parameters.numStepsToWaitBeforeFirstShow) {
        status = DETECTED_NOT_SHOWN_YET;
        return r;
    }


    if (trackedObjects[i].numFramesDetected <= parameters.numDetectedToWaitBeforeFirstShow) {
        status = DETECTED_NOT_SHOWN_YET;
        return r;
    }


    if (trackedObjects[i].numFramesNotDetected > parameters.numStepsToShowWithoutDetecting) {
        status = DETECTED_TEMPORARY_LOST;
		
        return r;
    }
#endif

    const TrackedObject::PositionsVector& lastPositions=trackedObjects[i].lastPositions;

    int N=(int)lastPositions.size();
    if (N<=0) {
        status = WRONG_OBJECT;
        return r;
    }

    return lastPositions[N-1];

}


