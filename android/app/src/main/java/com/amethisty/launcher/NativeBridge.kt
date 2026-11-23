package com.amethisty.launcher

/**
 * JNI bridge for Amethisty's own native library.
 * Loading this in the launcher process is not game-process injection.
 */
object NativeBridge {
    init {
        System.loadLibrary("amethisty")
    }

    @JvmStatic
    external fun initializeNative(modsDirectory: String, privateDirectory: String): Boolean
}
