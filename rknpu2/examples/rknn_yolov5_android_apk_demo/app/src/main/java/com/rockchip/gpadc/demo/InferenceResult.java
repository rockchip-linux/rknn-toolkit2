package com.rockchip.gpadc.demo;

import android.content.res.AssetManager;
import android.graphics.RectF;

import com.rockchip.gpadc.demo.yolo.InferenceWrapper;
import com.rockchip.gpadc.demo.yolo.PostProcess;
import com.rockchip.gpadc.demo.tracker.ObjectTracker;

import java.io.IOException;
import java.util.ArrayList;

import static com.rockchip.gpadc.demo.yolo.PostProcess.INPUT_SIZE;
import static com.rockchip.gpadc.demo.rga.HALDefine.CAMERA_PREVIEW_WIDTH;
import static com.rockchip.gpadc.demo.rga.HALDefine.CAMERA_PREVIEW_HEIGHT;
import static java.lang.System.arraycopy;

public class InferenceResult {

    OutputBuffer mOutputBuffer;
    ArrayList<Recognition> recognitions = null;
    private boolean mIsVaild = false;   //是否需要重新计算
    PostProcess mPostProcess = new PostProcess();
    private ObjectTracker mSSDObjectTracker;

    public void init(AssetManager assetManager) throws IOException {
        mOutputBuffer = new OutputBuffer();

        mPostProcess.init(assetManager);

//        mSSDObjectTracker = new ObjectTracker(CAMERA_PREVIEW_WIDTH, CAMERA_PREVIEW_HEIGHT, 3);
    }

    public void reset() {
        if (recognitions != null) {
            recognitions.clear();
            mIsVaild = true;
        }
        mSSDObjectTracker = new ObjectTracker(CAMERA_PREVIEW_WIDTH, CAMERA_PREVIEW_HEIGHT, 3);
    }
    public synchronized void setResult(OutputBuffer outputs) {

        if (mOutputBuffer.mGrid0Out == null) {
            mOutputBuffer.mGrid0Out = outputs.mGrid0Out.clone();
            mOutputBuffer.mGrid1Out = outputs.mGrid1Out.clone();
            mOutputBuffer.mGrid2Out = outputs.mGrid2Out.clone();
        } else {
            arraycopy(outputs.mGrid0Out, 0, mOutputBuffer.mGrid0Out, 0,
                    outputs.mGrid0Out.length);
            arraycopy(outputs.mGrid1Out, 0, mOutputBuffer.mGrid1Out, 0,
                    outputs.mGrid1Out.length);
            arraycopy(outputs.mGrid2Out, 0, mOutputBuffer.mGrid2Out, 0,
                    outputs.mGrid2Out.length);
        }
        mIsVaild = false;
    }

    public synchronized ArrayList<Recognition> getResult(InferenceWrapper mInferenceWrapper) {
        if (!mIsVaild) {
            mIsVaild = true;

            recognitions = mInferenceWrapper.postProcess(mOutputBuffer);

            recognitions = mSSDObjectTracker.tracker(recognitions);
        }

        return recognitions;
    }

    public static class OutputBuffer {
        public byte[] mGrid0Out;
        public byte[] mGrid1Out;
        public byte[] mGrid2Out;
    }

    /**
     * An immutable result returned by a Classifier describing what was recognized.
     */
    public static class Recognition {

        private int trackId = 0;

        /**
         * A unique identifier for what has been recognized. Specific to the class, not the instance of
         * the object.
         */
        private final int id;

        /**
         * A sortable score for how good the recognition is relative to others. Higher should be better.
         */
        private final Float confidence;

        /** Optional location within the source image for the location of the recognized object. */
        private RectF location;

        public Recognition(
                final int id, final Float confidence, final RectF location) {
            this.id = id;
            this.confidence = confidence;
            this.location = location;
            // TODO -- add name field, and show it.
        }

        public int getId() {
            return id;
        }

        public Float getConfidence() {
            return confidence;
        }

        public RectF getLocation() {
            return new RectF(location);
        }

        public void setLocation(RectF location) {
            this.location = location;
        }

        public void setTrackId(int trackId) {
            this.trackId = trackId;
        }

        public int getTrackId() {
            return this.trackId;
        }

        @Override
        public String toString() {
            String resultString = "";

            resultString += "[" + id + "] ";

            if (confidence != null) {
                resultString += String.format("(%.1f%%) ", confidence * 100.0f);
            }

            if (location != null) {
                resultString += location + " ";
            }

            return resultString.trim();
        }
    }

    /**
     * Detected objects, returned from native yolo_post_process
     */
    public static class DetectResultGroup {
        /**
         * detected objects count.
         */
        public int count = 0;

        /**
         * id for each detected object.
         */
        public int[] ids;

        /**
         * score for each detected object.
         */
        public float[] scores;

        /**
         * box for each detected object.
         */
        public float[] boxes;

//        public DetectResultGroup(
//                int count, int[] ids, float[] scores, float[] boxes
//        ) {
//            this.count = count;
//            this.ids = ids;
//            this.scores = scores;
//            this.boxes = boxes;
//        }
//
//        public int getCount() {
//            return count;
//        }
//
//        public void setCount(int count) {
//            this.count = count;
//        }
//
//        public int[] getIds() {
//            return ids;
//        }
//
//        public void setIds(int[] ids) {
//            this.ids = ids;
//        }
//
//        public float[] getScores() {
//            return scores;
//        }
//
//        public void setScores(float[] scores) {
//            this.scores = scores;
//        }
//
//        public float[] getBoxes() {
//            return boxes;
//        }
//
//        public void setBoxes(float[] boxes) {
//            this.boxes = boxes;
//        }
    }
}
