package com.geode.launcher

import android.content.Context
import android.util.Log
import java.io.File

internal object AmethistyLoader {
    private const val TAG = "Amethisty"

    private external fun initializeNative(modsDirectory: String, privateDirectory: String): Boolean

    fun load(context: Context): Boolean {
        val modsDirectory = context.getExternalFilesDir("amethisty/mods")
            ?: File(context.filesDir, "mods")
        val privateDirectory = File(context.filesDir, "amethisty")
        if (!modsDirectory.exists() && !modsDirectory.mkdirs()) {
            Log.w(TAG, "Could not create mods directory: \${modsDirectory.absolutePath}")
        }
        if (!privateDirectory.exists() && !privateDirectory.mkdirs()) {
            Log.w(TAG, "Could not create private directory: \${privateDirectory.absolutePath}")
        }
        return try {
            System.loadLibrary("amethisty")
            Log.i(TAG, "Native library loaded.")
            val initialized = initializeNative(modsDirectory.absolutePath, privateDirectory.absolutePath)
            if (!initialized) Log.w(TAG, "Mod initialization did not complete; game continues.")
            initialized
        } catch (error: UnsatisfiedLinkError) {
            Log.e(TAG, "Could not load or initialize Amethisty; continuing without mods.", error)
            false
        }
    }
}
