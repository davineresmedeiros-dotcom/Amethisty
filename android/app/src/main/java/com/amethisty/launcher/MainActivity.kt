package com.amethisty.launcher

import android.app.Activity
import android.content.pm.PackageInfo
import android.os.Build
import android.os.Bundle
import android.view.Gravity
import android.view.ViewGroup
import android.widget.Button
import android.widget.LinearLayout
import android.widget.TextView

private const val GD_PACKAGE = "com.robtopx.geometryjump"
private const val TARGET_VERSION = "2.2.145"

class MainActivity : Activity() {
    private lateinit var status: TextView
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
        refreshGameInfo()
    }

    override fun onResume() {
        super.onResume()
        if (::status.isInitialized) refreshGameInfo()
    }

    private fun buildUi() {
        val root = LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
            gravity = Gravity.CENTER
            setPadding(32, 36, 32, 36)
            layoutParams = ViewGroup.LayoutParams(
                ViewGroup.LayoutParams.MATCH_PARENT,
                ViewGroup.LayoutParams.MATCH_PARENT
            )
        }
        val title = TextView(this).apply {
            text = "Amethisty"
            textSize = 32f
            gravity = Gravity.CENTER
        }
        status = TextView(this).apply {
            textSize = 16f
            gravity = Gravity.CENTER
            setPadding(0, 24, 0, 24)
        }
        launchButton = Button(this).apply {
            text = "Abrir Geometry Dash"
            setOnClickListener { launchGame() }
        }
        root.addView(title)
        root.addView(status)
        root.addView(launchButton)
        setContentView(root)
    }

    private fun installedGameInfo(): PackageInfo? = try {
        if (Build.VERSION.SDK_INT >= 33) {
            packageManager.getPackageInfo(GD_PACKAGE, android.content.pm.PackageManager.PackageInfoFlags.of(0))
        } else {
            @Suppress("DEPRECATION")
            packageManager.getPackageInfo(GD_PACKAGE, 0)
        }
    } catch (_: android.content.pm.PackageManager.NameNotFoundException) {
        null
    }

    private fun refreshGameInfo() {
        val info = installedGameInfo()
        if (info == null) {
            status.text = "Geometry Dash não encontrado.\\nInstale o jogo oficial para continuar.\\n" +
                "Biblioteca nativa Amethisty: " +
                (if (nativeAvailable) "carregada no launcher" else "indisponível")
            launchButton.isEnabled = false
            return
        }
        val code = if (Build.VERSION.SDK_INT >= 28) info.longVersionCode else {
            @Suppress("DEPRECATION") info.versionCode.toLong()
        }
        val version = info.versionName ?: "desconhecida"
        val correct = version == TARGET_VERSION
        status.text = "Geometry Dash detectado\\nVersão: $version\\nVersion code real: $code\\n" +
            (if (correct) "Compatibilidade do pacote: versão-alvo encontrada" else
                "Versão diferente da alvo ($TARGET_VERSION). Não foi bloqueada por um código inventado.") +
            "\\nBiblioteca nativa Amethisty: " +
            (if (nativeAvailable) "carregada no launcher" else "indisponível")
        launchButton.isEnabled = packageManager.getLaunchIntentForPackage(GD_PACKAGE) != null
    }

    private fun launchGame() {
        val intent = packageManager.getLaunchIntentForPackage(GD_PACKAGE)
        if (intent != null) startActivity(intent) else refreshGameInfo()
    }
}
