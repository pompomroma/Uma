#import <UIKit/UIKit.h>
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#include "../src/Game.h"
#include <memory>
#include <iostream>

@interface GameViewController : UIViewController <MTKViewDelegate>

@property (nonatomic, strong) MTKView *mtkView;
@property (nonatomic, strong) id<MTLDevice> device;
@property (nonatomic, strong) id<MTLCommandQueue> commandQueue;

@end

@implementation GameViewController {
    std::unique_ptr<Game> _game;
    CFTimeInterval _lastUpdateTime;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    NSLog(@"GameViewController viewDidLoad started");
    
    // Setup Metal
    self.device = MTLCreateSystemDefaultDevice();
    if (!self.device) {
        NSLog(@"Metal is not supported on this device");
        [self showErrorAlert:@"Metal Not Supported" message:@"This device does not support Metal rendering."];
        return;
    }
    
    NSLog(@"Metal device created successfully: %@", self.device.name);
    
    self.mtkView = [[MTKView alloc] initWithFrame:self.view.bounds device:self.device];
    self.mtkView.delegate = self;
    self.mtkView.preferredFramesPerSecond = 60;
    self.mtkView.enableSetNeedsDisplay = NO;
    self.mtkView.multipleTouchEnabled = YES;
    self.mtkView.colorPixelFormat = MTLPixelFormatBGRA8Unorm;
    self.mtkView.depthStencilPixelFormat = MTLPixelFormatDepth32Float;
    
    // Add Metal view
    [self.view addSubview:self.mtkView];
    
    // Create command queue
    self.commandQueue = [self.device newCommandQueue];
    if (!self.commandQueue) {
        NSLog(@"Failed to create Metal command queue");
        [self showErrorAlert:@"Metal Error" message:@"Failed to create Metal command queue."];
        return;
    }
    
    // Initialize game
    _game = std::make_unique<Game>();
    CGSize size = self.view.bounds.size;
    NSLog(@"Initializing game with size: %.0fx%.0f", size.width, size.height);
    
    if (!_game->initialize((int)size.width, (int)size.height, "Racing Game 3D")) {
        NSLog(@"Failed to initialize game");
        [self showErrorAlert:@"Game Error" message:@"Failed to initialize the game engine."];
        return;
    }
    
    // Set up Metal for the renderer (if the game has a renderer)
    // This would require adding a method to get the renderer from the game
    // For now, we'll assume the game handles Metal setup internally
    
    _lastUpdateTime = CACurrentMediaTime();
    
    NSLog(@"Game initialized successfully for iOS");
}

- (void)viewWillLayoutSubviews {
    [super viewWillLayoutSubviews];
    self.mtkView.frame = self.view.bounds;
    
    // Handle orientation change
    if (_game) {
        CGSize size = self.view.bounds.size;
        _game->handleDeviceOrientation(size.width, size.height);
    }
}

- (BOOL)prefersStatusBarHidden {
    return YES;
}

- (UIInterfaceOrientationMask)supportedInterfaceOrientations {
    return UIInterfaceOrientationMaskLandscape;
}

#pragma mark - MTKViewDelegate

- (void)mtkView:(MTKView *)view drawableSizeWillChange:(CGSize)size {
    // Handle resize
    if (_game) {
        _game->handleDeviceOrientation(size.width, size.height);
    }
}

- (void)drawInMTKView:(MTKView *)view {
    if (!_game || !_game->getIsRunning()) {
        return;
    }
    
    // Calculate delta time
    CFTimeInterval currentTime = CACurrentMediaTime();
    float deltaTime = (float)(currentTime - _lastUpdateTime);
    _lastUpdateTime = currentTime;
    
    // Cap delta time
    deltaTime = std::min(deltaTime, 0.033f);
    
    // Update and render
    _game->update(deltaTime);
    _game->render();
    
    // Present drawable
    id<MTLCommandBuffer> commandBuffer = [self.commandQueue commandBuffer];
    [commandBuffer presentDrawable:view.currentDrawable];
    [commandBuffer commit];
}

#pragma mark - Touch Handling

- (void)touchesBegan:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    if (!_game) return;
    
    for (UITouch *touch in touches) {
        CGPoint location = [touch locationInView:self.mtkView];
        // touchIdentifierHash provides a unique ID for the touch
        uintptr_t touchId = (uintptr_t)touch;
        _game->handleTouchInput((int)touchId, location.x, location.y, 0, touch.force);
    }
}

- (void)touchesMoved:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    if (!_game) return;
    
    for (UITouch *touch in touches) {
        CGPoint location = [touch locationInView:self.mtkView];
        uintptr_t touchId = (uintptr_t)touch;
        _game->handleTouchInput((int)touchId, location.x, location.y, 1, touch.force);
    }
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    if (!_game) return;
    
    for (UITouch *touch in touches) {
        CGPoint location = [touch locationInView:self.mtkView];
        uintptr_t touchId = (uintptr_t)touch;
        _game->handleTouchInput((int)touchId, location.x, location.y, 3, touch.force);
    }
}

- (void)touchesCancelled:(NSSet<UITouch *> *)touches withEvent:(UIEvent *)event {
    if (!_game) return;
    
    for (UITouch *touch in touches) {
        CGPoint location = [touch locationInView:self.mtkView];
        uintptr_t touchId = (uintptr_t)touch;
        _game->handleTouchInput((int)touchId, location.x, location.y, 4, touch.force);
    }
}

#pragma mark - Memory Warning

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    if (_game) {
        _game->setLowPowerMode(true);
    }
}

- (void)dealloc {
    NSLog(@"GameViewController dealloc");
    if (_game) {
        _game->shutdown();
        _game.reset();
    }
}

- (void)showErrorAlert:(NSString*)title message:(NSString*)message {
    UIAlertController* alert = [UIAlertController alertControllerWithTitle:title
                                                                   message:message
                                                            preferredStyle:UIAlertControllerStyleAlert];
    
    UIAlertAction* okAction = [UIAlertAction actionWithTitle:@"OK"
                                                       style:UIAlertActionStyleDefault
                                                     handler:nil];
    [alert addAction:okAction];
    
    [self presentViewController:alert animated:YES completion:nil];
}

@end
