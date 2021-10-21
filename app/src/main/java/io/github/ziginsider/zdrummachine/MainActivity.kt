package io.github.ziginsider.zdrummachine

import android.content.res.AssetManager
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import io.github.ziginsider.zdrummachine.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        // Example of a call to a native method
        binding.sampleText.text = stringFromJNI()
    }

    override fun onResume() {
        super.onResume()
        native_onStart(this.assets)
    }

    /**
     * A native method that is implemented by the 'zdrummachine' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    private external fun native_onStart(assetManager: AssetManager)

    companion object {
        // Used to load the 'zdrummachine' library on application startup.
        init {
            System.loadLibrary("zdrummachine")
        }
    }
}