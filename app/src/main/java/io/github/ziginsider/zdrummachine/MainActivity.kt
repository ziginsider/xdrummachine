package io.github.ziginsider.zdrummachine

import android.content.res.AssetManager
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.util.Log
import androidx.core.view.isVisible
import io.github.ziginsider.zdrummachine.databinding.ActivityMainBinding
import android.widget.SeekBar
import android.widget.SeekBar.OnSeekBarChangeListener


class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    // TODO to VM
    // 0 - stopped (idle), 1 - playing, 2 - paused
    private var playingStatus = 0

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        if (savedInstanceState == null) {
            binding.seekBar.progress = 60
            binding.seekBarTextView.text = binding.seekBar.progress.toString()
        }

        // Example of a call to a native method
        binding.sampleText.text = stringFromJNI()

        binding.playButton.setOnClickListener {
            // TODO to VM
            when (playingStatus) {
                0, 2 -> {
                    Log.d(TAG, "start playing")
                    native_onStart(this.assets, binding.seekBar.progress)
                    playingStatus = 1 // TODO get status from zDrumMachine instead
                    binding.playButton.setImageDrawable(this.getDrawable(R.drawable.ic_baseline_pause_24))
                    binding.stopButton.isVisible = true
                }
                1 -> {
                    Log.d(TAG, "pause playing")
                    native_onPause()
                    playingStatus = 2 // TODO get status from zDrumMachine instead
                    binding.playButton.setImageDrawable(this.getDrawable(R.drawable.ic_baseline_play_arrow_24))
                }
            }
        }

        binding.stopButton.setOnClickListener {
            Log.d(TAG, "stop playing")
            binding.seekBar.progress = 60
            onStopPlaying()
        }

        setSeekBar()
    }

    private fun setSeekBar() {

        binding.seekBar.setOnSeekBarChangeListener(object : OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar, i: Int, b: Boolean) {
                binding.seekBarTextView.text = i.toString()
            }

            override fun onStartTrackingTouch(seekBar: SeekBar) {}
            override fun onStopTrackingTouch(seekBar: SeekBar) {
                onStopPlaying()
            }
        })

        binding.minusBpmButton.setOnClickListener {
            onStopPlaying()
            binding.seekBar.progress--
        }

        binding.plusBpmButton.setOnClickListener {
            onStopPlaying()
            binding.seekBar.progress++
        }
    }

    private fun onStopPlaying() {
        if (playingStatus == 0) return
        native_onStop()
        playingStatus = 0
        binding.stopButton.isVisible = false
        binding.playButton.setImageDrawable(this@MainActivity.getDrawable(R.drawable.ic_baseline_play_arrow_24))
    }

    /**
     * A native method that is implemented by the 'zdrummachine' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    private external fun native_onStart(assetManager: AssetManager, bpm: Int)

    private external fun native_onPause()

    private external fun native_onStop()

//    private external fun native_stop(bmp: Int)

    companion object {
        // Used to load the 'zdrummachine' library on application startup.
        init {
            System.loadLibrary("zdrummachine")
        }

        private const val TAG = "MainActivity"
    }
}