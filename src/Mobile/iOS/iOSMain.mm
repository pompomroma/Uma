#ifdef IOS_PLATFORM

#import <UIKit/UIKit.h>
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#import <GLKit/GLKit.h>
#import <CoreMotion/CoreMotion.h>
#import <GameController/GameController.h>

#include "../MobileMain.cpp"
#include <iostream>
#include <chrono>

// iOS-specific game state
@interface iOSGameState : NSObject
@property (nonatomic, strong) EAGLContext *context;
@property (nonatomic, assign) BOOL isInitialized;
@property (nonatomic, assign) BOOL isRunning;
@property (nonatomic, assign) CGSize screenSize;
@property (nonatomic, assign) CGFloat screenScale;
@property (nonatomic, assign) std::chrono::steady_clock::time_point lastFrameTime;
@end

@implementation iOSGameState
@end

static iOSGameState *g_iOSState;

// iOS Game View Controller
@interface GameViewController : GLKViewController
@property (nonatomic, strong) CMMotionManager *motionManager;
@property (nonatomic, strong) NSMutableDictionary *activeTouches;
@end

@implementation GameViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    NSLog(@"GameViewController viewDidLoad");
    
    // Initialize state
    g_iOSState = [[iOSGameState alloc] init];
    self.activeTouches = [[NSMutableDictionary alloc] init];
    
    // Setup OpenGL ES context
    [self setupOpenGLES];
    
    // Setup motion manager
    [self setupMotionManager];
    
    // Setup touch handling
    [self setupTouchHandling];
    
    // Initialize game
    [self initializeGame];
}

- (void)setupOpenGLES {
    NSLog(@"Setting up OpenGL ES");
    
    // Create OpenGL ES 3.0 context
    g_iOSState.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    
    if (!g_iOSState.context) {
        NSLog(@"Failed to create OpenGL ES 3.0 context, trying 2.0");
        g_iOSState.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
    }
    
    if (!g_iOSState.context) {
        NSLog(@"Failed to create OpenGL ES context");
        return;
    }
    
    // Setup GLKView
    GLKView *view = (GLKView *)self.view;
    view.context = g_iOSState.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    view.drawableMultisample = GLKViewDrawableMultisample4X;
    
    // Get screen properties
    g_iOSState.screenSize = view.bounds.size;
    g_iOSState.screenScale = [[UIScreen mainScreen] scale];
    
    // Set preferred frame rate
    self.preferredFramesPerSecond = 60;
    
    NSLog(@"OpenGL ES setup complete: %.0fx%.0f @%.1fx", 
          g_iOSState.screenSize.width, g_iOSState.screenSize.height, g_iOSState.screenScale);
}

- (void)setupMotionManager {
    NSLog(@"Setting up motion manager");
    
    self.motionManager = [[CMMotionManager alloc] init];
    
    if (self.motionManager.deviceMotionAvailable) {
        self.motionManager.deviceMotionUpdateInterval = 1.0 / 60.0; // 60 Hz
        
        [self.motionManager startDeviceMotionUpdatesUsingReferenceFrame:CMAttitudeReferenceFrameXArbitraryZVertical
                                                                toQueue:[NSOperationQueue mainQueue]
                                                            withHandler:^(CMDeviceMotion *motion, NSError *error) {
            if (error) {
                NSLog(@"Motion update error: %@", error);
                return;
            }
            
            // Handle device motion for tilt controls (optional)
            [self handleDeviceMotion:motion];
        }];
        
        NSLog(@"Motion manager started");
    } else {
        NSLog(@"Device motion not available");
    }
}

- (void)setupTouchHandling {
    // Enable multiple touch
    self.view.multipleTouchEnabled = YES;
    self.view.userInteractionEnabled = YES;
}

- (void)initializeGame {
    NSLog(@"Initializing iOS game");
    
    // Make context current
    [EAGLContext setCurrentContext:g_iOSState.context];
    
    // Calculate screen dimensions in pixels
    CGFloat width = g_iOSState.screenSize.width * g_iOSState.screenScale;
    CGFloat height = g_iOSState.screenSize.height * g_iOSState.screenScale;
    CGFloat density = g_iOSState.screenScale * 160.0f; // Convert to Android-style DPI
    
    // Initialize mobile game
    if (mobile_game_initialize((int)width, (int)height, density)) {
        g_iOSState.isInitialized = YES;
        g_iOSState.isRunning = YES;
        g_iOSState.lastFrameTime = std::chrono::steady_clock::now();
        NSLog(@"iOS game initialized successfully");
    } else {
        NSLog(@"Failed to initialize iOS game");
    }
}

- (void)viewWillAppear:(BOOL)animated {
    [super viewWillAppear:animated];
    
    // Hide status bar
    [[UIApplication sharedApplication] setStatusBarHidden:YES];
    
    // Resume game if needed
    if (g_iOSState.isInitialized && !g_iOSState.isRunning) {
        [self resumeGame];
    }
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    
    // Pause game
    if (g_iOSState.isInitialized && g_iOSState.isRunning) {
        [self pauseGame];
    }
}

- (void)dealloc {
    NSLog(@"GameViewController dealloc");
    
    [self shutdownGame];
    
    if (self.motionManager.deviceMotionActive) {
        [self.motionManager stopDeviceMotionUpdates];
    }
}

- (void)pauseGame {
    NSLog(@"Pausing iOS game");
    g_iOSState.isRunning = NO;
    mobile_game_pause();
}

- (void)resumeGame {
    NSLog(@"Resuming iOS game");
    g_iOSState.isRunning = YES;
    g_iOSState.lastFrameTime = std::chrono::steady_clock::now();
    mobile_game_resume();
}

- (void)shutdownGame {
    NSLog(@"Shutting down iOS game");
    
    if (g_iOSState.isInitialized) {
        mobile_game_shutdown();
        g_iOSState.isInitialized = NO;
    }
    
    g_iOSState.isRunning = NO;
}

// GLKViewController delegate methods
- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
    if (!g_iOSState.isInitialized || !g_iOSState.isRunning) {
        return;
    }
    
    // Calculate delta time
    auto currentTime = std::chrono::steady_clock::now();
    float deltaTime = 0.016f; // Default to 60 FPS
    
    if (g_iOSState.lastFrameTime.time_since_epoch().count() > 0) {
        deltaTime = std::chrono::duration<float>(currentTime - g_iOSState.lastFrameTime).count();
    }
    g_iOSState.lastFrameTime = currentTime;
    
    // Clamp delta time
    deltaTime = std::min(deltaTime, 0.033f); // Max 30 FPS
    
    // Update and render game
    mobile_game_update(deltaTime);
    mobile_game_render();
}

- (void)viewDidLayoutSubviews {
    [super viewDidLayoutSubviews];
    
    // Handle screen size changes
    CGSize newSize = self.view.bounds.size;
    if (!CGSizeEqualToSize(newSize, g_iOSState.screenSize)) {
        NSLog(@"Screen size changed: %.0fx%.0f", newSize.width, newSize.height);
        
        g_iOSState.screenSize = newSize;
        
        if (g_iOSState.isInitialized) {
            CGFloat width = newSize.width * g_iOSState.screenScale;
            CGFloat height = newSize.height * g_iOSState.screenScale;
            mobile_game_screen_size_changed((int)width, (int)height);
        }
    }
}

// Touch handling
- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    if (!g_iOSState.isInitialized || !g_iOSState.isRunning) {
        return;
    }
    
    for (UITouch *touch in touches) {
        NSValue *touchKey = [NSValue valueWithPointer:(__bridge const void *)touch];
        NSNumber *touchId = @([self.activeTouches count]);
        
        self.activeTouches[touchKey] = touchId;
        
        CGPoint location = [touch locationInView:self.view];
        CGFloat pressure = touch.force / touch.maximumPossibleForce;
        if (isnan(pressure) || pressure == 0) pressure = 1.0;
        
        // Convert to pixel coordinates
        location.x *= g_iOSState.screenScale;
        location.y *= g_iOSState.screenScale;
        
        mobile_game_touch_down([touchId intValue], location.x, location.y, pressure);
    }
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    if (!g_iOSState.isInitialized || !g_iOSState.isRunning) {
        return;
    }
    
    for (UITouch *touch in touches) {
        NSValue *touchKey = [NSValue valueWithPointer:(__bridge const void *)touch];
        NSNumber *touchId = self.activeTouches[touchKey];
        
        if (touchId) {
            CGPoint location = [touch locationInView:self.view];
            CGFloat pressure = touch.force / touch.maximumPossibleForce;
            if (isnan(pressure) || pressure == 0) pressure = 1.0;
            
            // Convert to pixel coordinates
            location.x *= g_iOSState.screenScale;
            location.y *= g_iOSState.screenScale;
            
            mobile_game_touch_move([touchId intValue], location.x, location.y, pressure);
        }
    }
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    if (!g_iOSState.isInitialized || !g_iOSState.isRunning) {
        return;
    }
    
    for (UITouch *touch in touches) {
        NSValue *touchKey = [NSValue valueWithPointer:(__bridge const void *)touch];
        NSNumber *touchId = self.activeTouches[touchKey];
        
        if (touchId) {
            CGPoint location = [touch locationInView:self.view];
            
            // Convert to pixel coordinates
            location.x *= g_iOSState.screenScale;
            location.y *= g_iOSState.screenScale;
            
            mobile_game_touch_up([touchId intValue], location.x, location.y);
            
            [self.activeTouches removeObjectForKey:touchKey];
        }
    }
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [self touchesEnded:touches withEvent:event];
}

// Device motion handling
- (void)handleDeviceMotion:(CMDeviceMotion *)motion {
    // Optional: Use device motion for tilt steering
    // This can be implemented as an alternative control method
    
    // Example: Use gravity vector for steering input
    double gravity = motion.gravity.x;
    // Process gravity value and send to game as steering input
    // mobile_game_set_tilt_steering(gravity);
}

// Memory warning
- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    NSLog(@"Memory warning received");
    
    if (g_iOSState.isInitialized) {
        mobile_game_low_memory();
    }
}

// Status bar
- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (UIStatusBarStyle)preferredStatusBarStyle {
    return UIStatusBarStyleLightContent;
}

// Home indicator (iPhone X and later)
- (BOOL)prefersHomeIndicatorAutoHidden {
    return YES;
}

@end

// App Delegate
@interface AppDelegate : UIResponder <UIApplicationDelegate>
@property (strong, nonatomic) UIWindow *window;
@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    NSLog(@"App launched");
    
    // Create window
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    
    // Create game view controller
    GameViewController *gameVC = [[GameViewController alloc] init];
    
    // Set as root view controller
    self.window.rootViewController = gameVC;
    
    // Make window visible
    [self.window makeKeyAndVisible];
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    NSLog(@"App will resign active");
    // Pause game when app becomes inactive
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    NSLog(@"App entered background");
    // Pause game when app enters background
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    NSLog(@"App will enter foreground");
    // Resume game when app enters foreground
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    NSLog(@"App became active");
    // Resume game when app becomes active
}

- (void)applicationWillTerminate:(UIApplication *)application {
    NSLog(@"App will terminate");
    // Cleanup when app terminates
}

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    NSLog(@"App received memory warning");
    // Handle memory warning
}

@end

// Main function
int main(int argc, char * argv[]) {
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}

#endif // IOS_PLATFORM