package com.amethisty.launcher

import android.app.Activity
import android.content.Intent
import android.content.pm.PackageInfo
import android.net.Uri
import android.os.Build
import android.os.Bundle
import android.provider.OpenableColumns
import android.view.Gravity
import android.view.ViewGroup
import android.widget.Button
import android.widget.LinearLayout
import android.widget.TextView
import java.io.File
import java.io.IOException

private const val GD_PACKAGE = "com.robtopx.geometryjump"
private const val TARGET_VERSION = "2.2.145"
private const val REQUEST_PICK_MOD = 4102

class MainActivity : Activity() {
    private lateinit var status: TextView
    private lateinit var launchButton: Button
    private lateinit var modsButton: Button
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
        modsButton = Button(this).apply {
            text = "Adicionar mod (.amethyst)"
            setOnClickListener { chooseModPackage() }
        }
        launchButton = Button(this).apply {
            text = "Abrir Geometry Dash"
            setOnClickListener { launchGame() }
        }
        root.addView(title)
        root.addView(status)
        root.addView(modsButton)
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

    private fun modsDirectory(): File = File(filesDir, "amethisty/mods").apply { mkdirs() }

    private fun modCount(): Int = modsDirectory().listFiles()?.count {
        it.isFile && it.extension.equals("amethyst", ignoreCase = true)
    } ?: 0

    private fun refreshGameInfo() {
        val info = installedGameInfo()
        val mods = "Pacotes .amethyst guardados: ${modCount()}"
        if (info == null) {
            status.text = "Geometry Dash não encontrado.\nInstale o jogo oficial para continuar.\n$mods\n" +
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
        status.text = "Geometry Dash detectado\nVersão: $version\nVersion code real: $code\n" +
            (if (correct) "Versão-alvo encontrada" else
                "Versão diferente da alvo ($TARGET_VERSION); não bloqueada por código inventado.") +
            "\n$mods\nBiblioteca nativa Amethisty: " +
            (if (nativeAvailable) "carregada no launcher" else "indisponível")
        launchButton.isEnabled = packageManager.getLaunchIntentForPackage(GD_PACKAGE) != null
    }

    private fun chooseModPackage() {
        val intent = Intent(Intent.ACTION_OPEN_DOCUMENT).apply {
            addCategory(Intent.CATEGORY_OPENABLE)
            type = "application/octet-stream"
            putExtra(Intent.EXTRA_MIME_TYPES, arrayOf("application/octet-stream", "application/zip"))
        }
        startActivityForResult(intent, REQUEST_PICK_MOD)
    }

    @Deprecated("Use Activity Result APIs when the AndroidX Activity dependency is added.")
    override fun onActivityResult(requestCode: Int, resultCode: Int, data: Intent?) {
        super.onActivityResult(requestCode, resultCode, data)
        if (requestCode != REQUEST_PICK_MOD || resultCode != RESULT_OK) return
        val uri = data?.data ?: return
        val result = importModPackage(uri)
        status.text = result
        refreshGameInfo()
        status.text = result + "\n\n" + status.text
    }

    private fun importModPackage(uri: Uri): String {
        val resolver = contentResolver
        var displayName = "mod.amethyst"
        resolver.query(uri, arrayOf(OpenableColumns.DISPLAY_NAME), null, null, null)?.use { cursor ->
            if (cursor.moveToFirst()) {
                val index = cursor.getColumnIndex(OpenableColumns.DISPLAY_NAME)
                if (index >= 0) displayName = cursor.getString(index) ?: displayName
            }
        }
        if (!displayName.endsWith(".amethyst", ignoreCase = true)) {
            return "Arquivo recusado: selecione um pacote .amethyst."
        }
        val safeName = displayName.substringAfterLast('/').substringAfterLast('\\')
            .replace(Regex("[^A-Za-z0-9._-]"), "_")
            .take(120)
        if (safeName.isBlank() || safeName == "." || safeName == "..") {
            return "Nome de arquivo inválido."
        }
        val destination = File(modsDirectory(), safeName)
        val temporary = File(modsDirectory(), "$safeName.tmp")
        return try {
            resolver.openInputStream(uri)?.use { input ->
                temporary.outputStream().use { output -> input.copyTo(output) }
            } ?: return "Não foi possível abrir o arquivo escolhido."
            if (temporary.length() <= 0L) {
                temporary.delete()
                return "Pacote vazio; importação cancelada."
            }
            if (!temporary.renameTo(destination)) {
                temporary.delete()
                return "Não foi possível salvar o pacote na pasta de mods."
            }
            "Pacote copiado para a pasta privada do Amethisty: $safeName"
        } catch (_: IOException) {
            temporary.delete()
            "Falha ao copiar o pacote .amethyst."
        }
    }

    private fun launchGame() {
        val intent = packageManager.getLaunchIntentForPackage(GD_PACKAGE)
        if (intent != null) startActivity(intent) else refreshGameInfo()
    }
}
