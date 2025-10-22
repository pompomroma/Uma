package com.racinggame3d.mobile;

import android.app.NativeActivity;
import android.os.Bundle;
import android.view.View;
import android.view.WindowManager;
import android.content.pm.ActivityInfo;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.content.Context;
import android.os.Build;
import android.os.VibrationEffect;
import android.os.Vibrator;

public class MainActivity extends NativeActivity implements SensorEventListener {
    
    private SensorManager sensorManager;
    private Sensor accelerometer;
    private Sensor gyroscope;
    private Vibrator vibrator;
    
    static {
        System.loadLibrary("RacingGame3D");
    }
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        // Force landscape orientation
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        
        // Keep screen on while playing
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        
        // Hide system UI for immersive experience
        hideSystemUI();
        
        // Initialize sensors
        sensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        accelerometer = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
        gyroscope = sensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);
        
        // Initialize vibrator
        vibrator = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
    }
    
    @Override
    protected void onResume() {
        super.onResume();
        hideSystemUI();
        
        // Register sensor listeners
        if (accelerometer != null) {
            sensorManager.registerListener(this, accelerometer, SensorManager.SENSOR_DELAY_GAME);
        }
        if (gyroscope != null) {
            sensorManager.registerListener(this, gyroscope, SensorManager.SENSOR_DELAY_GAME);
        }
        
        // Notify native code
        nativeOnResume();
    }
    
    @Override
    protected void onPause() {
        super.onPause();
        
        // Unregister sensor listeners
        sensorManager.unregisterListener(this);
        
        // Notify native code
        nativeOnPause();
    }
    
    @Override
    protected void onDestroy() {
        super.onDestroy();
        nativeOnDestroy();
    }
    
    @Override
    public void onSensorChanged(SensorEvent event) {
        if (event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
            nativeOnAccelerometer(event.values[0], event.values[1], event.values[2]);
        } else if (event.sensor.getType() == Sensor.TYPE_GYROSCOPE) {
            nativeOnGyroscope(event.values[0], event.values[1], event.values[2]);
        }
    }
    
    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {
        // Not used
    }
    
    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        if (hasFocus) {
            hideSystemUI();
        }
    }
    
    private void hideSystemUI() {
        View decorView = getWindow().getDecorView();
        decorView.setSystemUiVisibility(
            View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
            | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
            | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
            | View.SYSTEM_UI_FLAG_FULLSCREEN);
    }
    
    // Called from native code
    public void vibrate(int milliseconds) {
        if (vibrator != null && vibrator.hasVibrator()) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                vibrator.vibrate(VibrationEffect.createOneShot(milliseconds, VibrationEffect.DEFAULT_AMPLITUDE));
            } else {
                vibrator.vibrate(milliseconds);
            }
        }
    }
    
    // Native methods
    private native void nativeOnResume();
    private native void nativeOnPause();
    private native void nativeOnDestroy();
    private native void nativeOnAccelerometer(float x, float y, float z);
    private native void nativeOnGyroscope(float x, float y, float z);
}