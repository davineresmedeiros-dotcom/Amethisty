package com.amethisty.launcher

import android.app.Activity
import android.app.AlertDialog
import android.content.pm.PackageInfo
import android.graphics.Color
import android.graphics.Typeface
import android.graphics.drawable.GradientDrawable
import android.os.Build
import android.os.Bundle
import android.view.Gravity
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.LinearLayout
import android.widget.TextView

private const val GD_PACKAGE = "com.robtopx.geometryjump"
private const val TARGET_VERSION = "2.2.145"

class MainActivity : Activity() {
    private lateinit var launchButton: Button
    private var nativeAvailable = false

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        nativeAvailable = try {
            NativeBridge.isLoaded()
        } catch (_: UnsatisfiedLinkError) {
            false
        }
        buildUi()
        refreshGameAvailability()
    }

    override fun onResume() {
        super.onResume()
        if (::launchButton.isInitialized) refreshGameAvailability()
    }

    private fun buildUi() {
        val root = LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
            gravity = Gravity.CENTER
            setPadding(dp(28), dp(28), dp(28), dp(28))
            setBackgroundColor(Color.rgb(250, 249, 255))
            layoutParams = ViewGroup.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT
            )
        }

        val title = TextView(this).apply {
            text = "Amethisty"
            textSize = 38f
            typeface = Typeface.create("sans-serif", Typeface.NORMAL)
            setTextColor(Color.rgb(25, 24, 34))
            gravity = Gravity.CENTER
            includeFontPadding = true
        }

        val controls = LinearLayout(this).apply {
            orientation = LinearLayout.HORIZONTAL
            gravity = Gravity.CENTER
            setPadding(0, dp(26), 0, 0)
        }

        launchButton = Button(this).apply {
            text = "▷   Launch"
            textSize = 18f
            isAllCaps = false
            setTextColor(Color.WHITE)
            background = roundedBackground(Color.rgb(65, 91, 145), dp(32))
            minHeight = dp(58)
            setPadding(dp(28), 0, dp(28), 0)
            setOnClickListener { launchGame() }
        }

        val settingsButton = Button(this).apply {
            text = "⚙"
            textSize = 29f
            isAllCaps = false
            setTextColor(Color.rgb(25, 24, 34))
            background = transparentBackground()
            minWidth = dp(58)
            minHeight = dp(58)
            setPadding(dp(8), 0, dp(8), 0)
            contentDescription = "Settings"
            setOnClickListener { showSettings() }
        }

        controls.addView(launchButton)
        controls.addView(settingsButton, LinearLayout.LayoutParams(dp(64), dp(64)).apply {
            leftMargin = dp(12)
        })

        root.addView(title)
        root.addView(controls)
        setContentView(root)
    }

    private fun refreshGameAvailability() {
        launchButton.isEnabled = packageManager.getLaunchIntentForPackage(GD_PACKAGE) != null
        launchButton.alpha = if (launchButton.isEnabled) 1f else 0.55f
    }

    private fun installedGameInfo(): PackageInfo? = try {
        if (Build.VERSION.SDK_INT >= 33) {
            packageManager.getPackageInfo(
                GD_PACKAGE,
                android.content.pm.PackageManager.PackageInfoFlags.of(0)
            )
        } else {
            @Suppress("DEPRECATION")
            packageManager.getPackageInfo(GD_PACKAGE, 0)
        }
    } catch (_: android.content.pm.PackageManager.NameNotFoundException) {
        null
    }

    private fun showSettings() {
        val info = installedGameInfo()
        val version = info?.versionName ?: "Geometry Dash não encontrado"
        val code = if (info != null) {
            if (Build.VERSION.SDK_INT >= 28) info.longVersionCode.toString()
            else {
                @Suppress("DEPRECATION")
                info.versionCode.toString()
            }
        } else "—"

        AlertDialog.Builder(this)
            .setTitle("Settings")
            .setMessage(
                "Amethisty\n\nGeometry Dash: $version\nVersion code: $code\n" +
                    "Versão-alvo: $TARGET_VERSION\nBiblioteca nativa: " +
                    (if (nativeAvailable) "carregada no launcher" else "indisponível")
            )
            .setPositiveButton("OK", null)
            .show()
    }

    private fun launchGame() {
        val intent = packageManager.getLaunchIntentForPackage(GD_PACKAGE)
        if (intent != null) startActivity(intent) else refreshGameAvailability()
    }

    private fun roundedBackground(color: Int, radius: Int) = GradientDrawable().apply {
        setColor(color)
        cornerRadius = radius.toFloat()
    }

    private fun transparentBackground() = GradientDrawable().apply {
        setColor(Color.TRANSPARENT)
    }

    private fun dp(value: Int): Int = (value * resources.displayMetrics.density).toInt()
}
