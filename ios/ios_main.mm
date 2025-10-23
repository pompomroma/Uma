#import <UIKit/UIKit.h>
#import "GameViewController.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@end

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Create window
    self.window = [[UIWindow alloc] initWithFrame:[UIScreen mainScreen].bounds];
    
    // Create and set root view controller
    GameViewController *viewController = [[GameViewController alloc] init];
    self.window.rootViewController = viewController;
    
    // Make window visible
    [self.window makeKeyAndVisible];
    
    // Prevent screen dimming during gameplay
    [UIApplication sharedApplication].idleTimerDisabled = YES;
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Pause the game when app goes to background
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Save game state
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Restore game state
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Resume the game
}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Save data and cleanup
}

@end

// Main entry point
int main(int argc, char * argv[]) {
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}
