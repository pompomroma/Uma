#ifdef __APPLE__
#import <UIKit/UIKit.h>
#import "AppDelegate.h"

int main(int argc, char * argv[]) {
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}
#else
// Desktop version for testing
#include "Game.h"
#include <iostream>

int main() {
    std::cout << "Uma Racing - Desktop version not implemented" << std::endl;
    std::cout << "Please build for iOS target" << std::endl;
    return 0;
}
#endif