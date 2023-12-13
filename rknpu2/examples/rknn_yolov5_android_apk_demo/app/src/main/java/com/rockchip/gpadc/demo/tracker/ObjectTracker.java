package com.rockchip.gpadc.demo.tracker;

import android.graphics.RectF;
import android.util.Log;

import com.rockchip.gpadc.demo.InferenceResult;

import java.util.ArrayList;

public class ObjectTracker {

    private final String TAG = "rkyolo.ObjectTracker";

    private long mHandle;

    private int mMaxTrackLifetime = 3;

    private int mWidth;

    private int mHeight;

    private static int MAX_TRACKED_NUM = 64;

    private static float[] track_input_location = new float[MAX_TRACKED_NUM *4];
    private static int[] track_input_class = new int[MAX_TRACKED_NUM];
    private static float[] track_input_score = new float[MAX_TRACKED_NUM];
    private static float[] track_output_location = new float[MAX_TRACKED_NUM *4];
    private static int[] track_output_class = new int[MAX_TRACKED_NUM];
    private static int[] track_output_id = new int[MAX_TRACKED_NUM];
    private static float[] track_output_score = new float[MAX_TRACKED_NUM];

//    public int track_count = 0;
//    public long track_time = 0;

    public ObjectTracker(int width, int height, int maxTrackLifetime) {
        mWidth = width;
        mHeight = height;
        mMaxTrackLifetime = maxTrackLifetime;
        mHandle = native_create();
    }

    protected void finalize() {
        native_destroy(mHandle);
    }

    public ArrayList<InferenceResult.Recognition> tracker(ArrayList<InferenceResult.Recognition> recognitions) {
//        long startTime = System.currentTimeMillis();
//        long endTime;
        int track_input_num = 0;
        ArrayList<InferenceResult.Recognition> tracked_recognitions = new ArrayList<>();

        for (int i = 0; i < recognitions.size(); ++i) {

            track_input_location[4*track_input_num +0] = recognitions.get(i).getLocation().left;
            track_input_location[4*track_input_num +1] = recognitions.get(i).getLocation().top;
            track_input_location[4*track_input_num +2] = recognitions.get(i).getLocation().right;
            track_input_location[4*track_input_num +3] = recognitions.get(i).getLocation().bottom;
            track_input_class[track_input_num] = recognitions.get(i).getId();
            track_input_score[track_input_num] = recognitions.get(i).getConfidence();
            //Log.i(TAG, track_input_num +" javain class:" +topClassScoreIndex +" P:" +track_input_score[track_input_num] +" score:" +expit(track_input_score[track_input_num]));
            track_input_num++;
            if (track_input_num >= MAX_TRACKED_NUM){
                break;
            }
        }

        int[] track_output_num = new int[1];

        native_track(mHandle, mMaxTrackLifetime,
                track_input_num, track_input_location, track_input_class, track_input_score,
                track_output_num, track_output_location, track_output_class, track_output_score,
                track_output_id, mWidth, mHeight);

        for (int i = 0; i < track_output_num[0]; ++i) {

            RectF detection = new RectF(
                            track_output_location[i * 4 + 0]/mWidth,
                            track_output_location[i * 4 + 1]/mHeight,
                            track_output_location[i * 4 + 2]/mWidth,
                            track_output_location[i * 4 + 3]/mHeight);
            float exp_score =  track_output_score[i];
            if (track_output_score[i] == -10000){
                exp_score = 0;
            }
            InferenceResult.Recognition recog = new InferenceResult.Recognition(
                    track_output_class[i],
                    exp_score,
                    detection);
            recog.setTrackId(track_output_id[i]);
            //Log.i(TAG, "javaout"+i +" class:" +topClassScoreIndex +" P:" +track_output_score[i] +" score:" +exp_score);
            tracked_recognitions.add(recog);
        }
//        endTime = System.currentTimeMillis();
//        this.track_count += 1;
//        this.track_time += (endTime - startTime);
//        if (this.track_count >= 100) {
//            float track_avg = this.track_time * 1.0f / this.track_count;
//            Log.i(TAG, String.format("track cost time avg: %.5f", track_avg));
//            this.track_count = 0;
//            this.track_time = 0;
//        }
        return tracked_recognitions;
    }

    private native long native_create();
    private native void native_destroy(long handle);
    private native void native_track(long hanle, int maxTrackLifetime,
                                     int track_input_num, float[] track_input_locations, int[] track_input_class, float[] track_input_score,
                                    int[] track_output_num, float[] track_output_locations, int[] track_output_class, float[] track_output_score,
                                     int[] track_output_id, int width, int height);

    static {
        System.loadLibrary("rknn4j");
    }
}
