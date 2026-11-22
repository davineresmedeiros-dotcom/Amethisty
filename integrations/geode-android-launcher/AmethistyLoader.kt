package com.geode.launcher

import android.util.Log

/**
 * Small adapter for a launcher that loads Geometry Dash's native libraries.
 *
 * Package libamethisty.so in the launcher's matching jniLibs ABI directory.
 */
internal object AmethistyLoader {
    private const val TAG = "Amethisty"

    fun load(): Boolean {
        return try {
            System.loadLibrary("amethisty")
            Log.i(TAG, "Native loader entry loaded.")
            true
        } catch (error: UnsatisfiedLinkError) {
            Log.e(TAG, "Could not load libamethisty.so; continuing without Amethisty.", error)
            false
        }
    }
}
