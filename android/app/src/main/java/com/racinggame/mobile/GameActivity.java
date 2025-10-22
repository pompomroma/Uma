package com.racinggame.mobile;

import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import androidx.appcompat.app.AppCompatActivity;

public class GameActivity extends AppCompatActivity {
    private static final String TAG = "GameActivity";
    
    private GLSurfaceView glSurfaceView;
    private GameRenderer gameRenderer;
    private GameInputHandler inputHandler;
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        
        // Set fullscreen
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        
        // Set landscape orientation
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        
        // Keep screen on
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        
        // Initialize OpenGL ES
        initializeOpenGL();
        
        // Initialize input handler
        inputHandler = new GameInputHandler(this);
        
        // Set content view
        setContentView(glSurfaceView);
    }
    
    private void initializeOpenGL() {
        glSurfaceView = new GLSurfaceView(this);
        glSurfaceView.setEGLContextClientVersion(3);
        glSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        
        gameRenderer = new GameRenderer(this);
        glSurfaceView.setRenderer(gameRenderer);
        glSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY);
    }
    
    @Override
    protected void onResume() {
        super.onResume();
        if (glSurfaceView != null) {
            glSurfaceView.onResume();
        }
    }
    
    @Override
    protected void onPause() {
        super.onPause();
        if (glSurfaceView != null) {
            glSurfaceView.onPause();
        }
    }
    
    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (gameRenderer != null) {
            gameRenderer.cleanup();
        }
    }
    
    @Override
    public boolean onTouchEvent(MotionEvent event) {
        if (inputHandler != null) {
            return inputHandler.handleTouchEvent(event);
        }
        return super.onTouchEvent(event);
    }
    
    @Override
    public void onBackPressed() {
        // Handle back button - show pause menu
        showPauseMenu();
    }
    
    private void showPauseMenu() {
        // Show pause menu
        Toast.makeText(this, "Pause Menu", Toast.LENGTH_SHORT).show();
    }
    
    public void onGameEvent(String event, Object data) {
        // Handle game events
        switch (event) {
            case "game_over":
                handleGameOver();
                break;
            case "level_complete":
                handleLevelComplete();
                break;
            case "error":
                handleError((String) data);
                break;
        }
    }
    
    private void handleGameOver() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(GameActivity.this, "Game Over", Toast.LENGTH_LONG).show();
                finish();
            }
        });
    }
    
    private void handleLevelComplete() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(GameActivity.this, "Level Complete!", Toast.LENGTH_LONG).show();
            }
        });
    }
    
    private void handleError(String error) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(GameActivity.this, "Error: " + error, Toast.LENGTH_LONG).show();
            }
        });
    }
}