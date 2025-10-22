package com.racinggame3d

import android.app.Activity
import android.content.Context
import android.opengl.GLSurfaceView
import android.os.Bundle
import android.util.AttributeSet
import android.util.Log
import android.view.*
import android.view.WindowManager
import androidx.core.view.WindowCompat
import androidx.core.view.WindowInsetsCompat
import androidx.core.view.WindowInsetsControllerCompat
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class GameActivity : Activity() {
    
    companion object {
        private const val TAG = "GameActivity"
        
        // Load native library
        init {
            System.loadLibrary("RacingGame3D")
        }
    }
    
    private lateinit var glSurfaceView: GameSurfaceView
    private var isGameInitialized = false
    private var isPaused = false
    
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        
        Log.i(TAG, "GameActivity onCreate")
        
        // Setup fullscreen immersive mode
        setupFullscreen()
        
        // Keep screen on
        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)
        
        // Create OpenGL surface view
        glSurfaceView = GameSurfaceView(this)
        setContentView(glSurfaceView)
        
        // Set up touch handling
        setupTouchHandling()
    }
    
    override fun onResume() {
        super.onResume()
        Log.i(TAG, "GameActivity onResume")
        
        glSurfaceView.onResume()
        
        if (isGameInitialized && isPaused) {
            nativeResume()
            isPaused = false
        }
    }
    
    override fun onPause() {
        super.onPause()
        Log.i(TAG, "GameActivity onPause")
        
        if (isGameInitialized) {
            nativePause()
            isPaused = true
        }
        
        glSurfaceView.onPause()
    }
    
    override fun onDestroy() {
        super.onDestroy()
        Log.i(TAG, "GameActivity onDestroy")
        
        if (isGameInitialized) {
            nativeShutdown()
            isGameInitialized = false
        }
    }
    
    override fun onLowMemory() {
        super.onLowMemory()
        Log.w(TAG, "Low memory warning")
        
        if (isGameInitialized) {
            nativeLowMemory()
        }
    }
    
    override fun onBackPressed() {
        // Handle back button - pause game or show menu
        if (isGameInitialized) {
            nativePause()
        }
        // Don't call super.onBackPressed() to prevent activity from closing
    }
    
    private fun setupFullscreen() {
        WindowCompat.setDecorFitsSystemWindows(window, false)
        val controller = WindowInsetsControllerCompat(window, window.decorView)
        controller.hide(WindowInsetsCompat.Type.systemBars())
        controller.systemBarsBehavior = WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE
        
        // Set flags for immersive mode
        window.decorView.systemUiVisibility = (
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                        or View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                        or View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                        or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                        or View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                        or View.SYSTEM_UI_FLAG_FULLSCREEN
                )
    }
    
    private fun setupTouchHandling() {
        glSurfaceView.setOnTouchListener { _, event ->
            handleTouchEvent(event)
            true
        }
    }
    
    private fun handleTouchEvent(event: MotionEvent): Boolean {
        if (!isGameInitialized) return false
        
        val action = event.actionMasked
        val pointerIndex = event.actionIndex
        val pointerId = event.getPointerId(pointerIndex)
        
        when (action) {
            MotionEvent.ACTION_DOWN, MotionEvent.ACTION_POINTER_DOWN -> {
                val x = event.getX(pointerIndex)
                val y = event.getY(pointerIndex)
                val pressure = event.getPressure(pointerIndex)
                nativeTouchDown(pointerId, x, y, pressure)
            }
            
            MotionEvent.ACTION_MOVE -> {
                // Handle all active pointers
                for (i in 0 until event.pointerCount) {
                    val id = event.getPointerId(i)
                    val x = event.getX(i)
                    val y = event.getY(i)
                    val pressure = event.getPressure(i)
                    nativeTouchMove(id, x, y, pressure)
                }
            }
            
            MotionEvent.ACTION_UP, MotionEvent.ACTION_POINTER_UP -> {
                val x = event.getX(pointerIndex)
                val y = event.getY(pointerIndex)
                nativeTouchUp(pointerId, x, y)
            }
            
            MotionEvent.ACTION_CANCEL -> {
                // Cancel all active touches
                for (i in 0 until event.pointerCount) {
                    val id = event.getPointerId(i)
                    val x = event.getX(i)
                    val y = event.getY(i)
                    nativeTouchUp(id, x, y)
                }
            }
        }
        
        return true
    }
    
    // Custom GLSurfaceView for the game
    inner class GameSurfaceView @JvmOverloads constructor(
        context: Context,
        attrs: AttributeSet? = null
    ) : GLSurfaceView(context, attrs) {
        
        private val renderer = GameRenderer()
        
        init {
            // Set OpenGL ES 3.0 context
            setEGLContextClientVersion(3)
            
            // Set renderer
            setRenderer(renderer)
            
            // Render continuously
            renderMode = RENDERMODE_CONTINUOUSLY
            
            Log.i(TAG, "GameSurfaceView initialized")
        }
        
        override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {
            super.surfaceChanged(holder, format, width, height)
            Log.i(TAG, "Surface changed: ${width}x${height}")
            
            if (isGameInitialized) {
                nativeSurfaceChanged(width, height)
            }
        }
    }
    
    // OpenGL renderer
    inner class GameRenderer : GLSurfaceView.Renderer {
        
        override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
            Log.i(TAG, "Surface created")
            
            // Initialize native game
            if (!isGameInitialized) {
                isGameInitialized = nativeInitialize(glSurfaceView.holder.surface)
                if (!isGameInitialized) {
                    Log.e(TAG, "Failed to initialize native game")
                }
            }
        }
        
        override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
            Log.i(TAG, "Surface changed in renderer: ${width}x${height}")
            
            if (isGameInitialized) {
                nativeSurfaceChanged(width, height)
            }
        }
        
        override fun onDrawFrame(gl: GL10?) {
            if (isGameInitialized && !isPaused) {
                nativeRender()
            }
        }
    }
    
    // Native method declarations
    private external fun nativeInitialize(surface: Surface): Boolean
    private external fun nativeShutdown()
    private external fun nativeRender()
    private external fun nativePause()
    private external fun nativeResume()
    private external fun nativeSurfaceChanged(width: Int, height: Int)
    private external fun nativeTouchDown(touchId: Int, x: Float, y: Float, pressure: Float)
    private external fun nativeTouchMove(touchId: Int, x: Float, y: Float, pressure: Float)
    private external fun nativeTouchUp(touchId: Int, x: Float, y: Float)
    private external fun nativeSetPerformanceLevel(level: Int)
    private external fun nativeLowMemory()
}