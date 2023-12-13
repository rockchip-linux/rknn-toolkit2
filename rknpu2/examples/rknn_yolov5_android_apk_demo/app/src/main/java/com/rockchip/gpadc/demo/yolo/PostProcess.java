package com.rockchip.gpadc.demo.yolo;/*
 * Copyright (C) 2022 Rockchip Electronics Co., Ltd.
 * Authors:
 *  raul.rao <raul.rao@rock-chips.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import android.content.res.AssetManager;
import android.util.Log;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.Vector;

public class PostProcess {
    public static final int INPUT_SIZE = 640;
    public static final int INPUT_CHANNEL = 3;

    public static final String TAG = "rkyolo.PostProcess";

    // Pre-allocated buffers.
    private Vector<String> labels = new Vector<String>();

    public void init(AssetManager assetManager) throws IOException {
        loadLabelName(assetManager,
                "file:///android_asset/coco_80_labels_list.txt", labels);
    }

    public String getLabelTitle(int id) {
        return labels.get(id);
    }

    private void loadLabelName(final AssetManager assetManager, final String locationFilename,
                               Vector<String> labels) throws IOException {
        // Try to be intelligent about opening from assets or sdcard depending on prefix.
        final String assetPrefix = "file:///android_asset/";
        InputStream is;
        if (locationFilename.startsWith(assetPrefix)) {
            is = assetManager.open(locationFilename.split(assetPrefix, -1)[1]);
        } else {
            is = new FileInputStream(locationFilename);
        }

        final BufferedReader reader = new BufferedReader(new InputStreamReader(is));
        String line;
        while ((line = reader.readLine()) != null) {
            labels.add(line);
        }
        reader.close();

        Log.d(TAG, "Loaded label!");
    }
}
