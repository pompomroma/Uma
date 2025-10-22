#import "AppDelegate.h"
#import "ViewController.h"

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    
    ViewController *viewController = [[ViewController alloc] init];
    self.window.rootViewController = viewController;
    
    [self.window makeKeyAndVisible];
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Pause the game
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Save game state
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Resume the game
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Resume the game
}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Save game state
}

@end