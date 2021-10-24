package io.github.ziginsider.zdrummachine

import android.content.res.AssetManager
import android.os.Bundle
import android.util.Log
import android.widget.SeekBar
import android.widget.SeekBar.OnSeekBarChangeListener
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.isVisible
import io.github.ziginsider.zdrummachine.databinding.ActivityMainBinding


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
            native_onInit(this.assets, binding.seekBar.progress)
        }

        binding.playButton.setOnClickListener {
            // TODO to VM
            when (playingStatus) {
                0, 2 -> {
                    Log.d(TAG, "start playing")
                    setSoundPatterns()
                    native_onStart(binding.seekBar.progress)
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

    private fun setSoundPatterns() {
        val metronomeArray = mutableListOf<Int>()
        val midTomArray = mutableListOf<Int>()
        val snareArray = mutableListOf<Int>()

        with(binding) {
            if (midTom1.isChecked) midTomArray.add(0)
            if (midTom2.isChecked) midTomArray.add(500)
            if (midTom3.isChecked) midTomArray.add(1000)
            if (midTom4.isChecked) midTomArray.add(1500)
            if (midTom5.isChecked) midTomArray.add(2000)
            if (midTom6.isChecked) midTomArray.add(2500)
            if (midTom7.isChecked) midTomArray.add(3000)
            if (midTom8.isChecked) midTomArray.add(3500)

            if (snare1.isChecked) snareArray.add(0)
            if (snare2.isChecked) snareArray.add(500)
            if (snare3.isChecked) snareArray.add(1000)
            if (snare4.isChecked) snareArray.add(1500)
            if (snare5.isChecked) snareArray.add(2000)
            if (snare6.isChecked) snareArray.add(2500)
            if (snare7.isChecked) snareArray.add(3000)
            if (snare8.isChecked) snareArray.add(3500)
        }

        // set patterns
        if (binding.metronomeCheckbox.isChecked) {
            metronomeArray.add(0)
            metronomeArray.add(1000)
            metronomeArray.add(2000)
            metronomeArray.add(3000)
        }
        native_setPattern(0, metronomeArray.toIntArray())
        native_setPattern(1, midTomArray.toIntArray())
        native_setPattern(2, snareArray.toIntArray())
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

    private external fun native_onInit(assetManager: AssetManager, bpm: Int)
    private external fun native_onStart(bpm: Int)
    private external fun native_onPause()
    private external fun native_onStop()
    private external fun native_setPattern(id: Int, pattern: IntArray)

    companion object {
        // Used to load the 'zdrummachine' library on application startup.
        init {
            System.loadLibrary("zdrummachine")
        }

        private const val TAG = "MainActivity"
    }
}