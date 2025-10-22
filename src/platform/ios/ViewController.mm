#import "ViewController.h"
#include "Game.h"
#include <memory>

@interface ViewController () {
    std::unique_ptr<Game> game;
    CADisplayLink *displayLink;
}

@property (strong, nonatomic) EAGLContext *context;

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    // Create OpenGL ES context
    self.context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
    
    if (!self.context) {
        NSLog(@"Failed to create ES context");
        return;
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    view.drawableMultisample = GLKViewDrawableMultisample4X;
    
    [EAGLContext setCurrentContext:self.context];
    
    // Initialize the game
    game = std::make_unique<Game>();
    
    CGRect bounds = self.view.bounds;
    CGFloat scale = [[UIScreen mainScreen] scale];
    int width = bounds.size.width * scale;
    int height = bounds.size.height * scale;
    
    if (!game->initialize(width, height)) {
        NSLog(@"Failed to initialize game");
        return;
    }
    
    // Set up display link for smooth rendering
    displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(gameLoop:)];
    displayLink.preferredFramesPerSecond = 60;
    [displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    
    // Enable touch input
    self.view.multipleTouchEnabled = YES;
}

- (void)dealloc {
    [displayLink invalidate];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
}

- (void)gameLoop:(CADisplayLink *)displayLink {
    if (game) {
        CFTimeInterval currentTime = CACurrentMediaTime();
        game->update(currentTime);
    }
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect {
    if (game) {
        game->render();
    }
}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations {
    return UIInterfaceOrientationMaskLandscape;
}

#pragma mark - Touch Handling

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    if (!game) return;
    
    UITouch *touch = [touches anyObject];
    CGPoint location = [touch locationInView:self.view];
    
    // Convert to normalized coordinates
    CGRect bounds = self.view.bounds;
    float x = location.x / bounds.size.width;
    float y = location.y / bounds.size.height;
    
    game->handleTouch(x, y, true);
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    if (!game) return;
    
    UITouch *touch = [touches anyObject];
    CGPoint location = [touch locationInView:self.view];
    
    // Convert to normalized coordinates
    CGRect bounds = self.view.bounds;
    float x = location.x / bounds.size.width;
    float y = location.y / bounds.size.height;
    
    game->handleTouchMove(x, y);
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    if (!game) return;
    
    game->handleTouchEnd();
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    [self touchesEnded:touches withEvent:event];
}

@end