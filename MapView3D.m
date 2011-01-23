//
//  MapView3D.m
//  TrenchBroom
//
//  Created by Kristian Duske on 15.03.10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#import "MapView3D.h"
#import <OpenGl/gl.h>
#import <OpenGL/glu.h>
#import "RenderMap.h"
#import "Camera.h"
#import "RenderContext.h"
#import "RenderContext3D.h"
#import "TextureManager.h"

NSString* const MapView3DDefaults = @"3D View";
NSString* const MapView3DDefaultsBackgroundColor = @"BackgroundColor";

@implementation MapView3D

- (void)awakeFromNib {
    [super awakeFromNib];
    
    [[NSNotificationCenter defaultCenter] addObserver:self 
                                             selector:@selector(userDefaultsChanged:) 
                                                 name:NSUserDefaultsDidChangeNotification 
                                               object:[NSUserDefaults standardUserDefaults]];
    
    [self userDefaultsChanged:nil];
}

- (void) drawRect:(NSRect)dirtyRect {
    NSRect bounds = [self frame];
    glViewport(bounds.origin.x, bounds.origin.y, bounds.size.width, bounds.size.height);
    
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(backgroundColor[0], backgroundColor[1], backgroundColor[2], 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    RenderContext3D* renderContext = [[RenderContext3D alloc] initWithTextureManager:textureManager];
    [renderContext updateView:bounds withCamera:camera];
    
    glPolygonMode(GL_FRONT, GL_FILL);
    
    [renderMap renderWithContext:renderContext];
    [renderContext release];
    
    [[self openGLContext] flushBuffer];
}

- (void)userDefaultsChanged:(NSNotification *)notification {
    NSDictionary* viewDefaults = [[NSUserDefaults standardUserDefaults] dictionaryForKey:MapView3DDefaults];
    if (viewDefaults == nil)
        return;
    
    NSArray* backgroundColorArray = [viewDefaults objectForKey:MapView3DDefaultsBackgroundColor];
    if (backgroundColorArray != nil && [backgroundColorArray count] == 3)
        for (int i = 0; i < 3; i++)
            backgroundColor[i] = [[backgroundColorArray objectAtIndex:i] floatValue];

    [self setNeedsDisplay:YES];
}

- (void)setCamera:(Camera *)aCamera {
    if (aCamera == nil)
        [NSException raise:NSInvalidArgumentException format:@"camera must not be nil"];
    
    camera = [aCamera retain];
}

- (void)setRenderMap:(RenderMap *)aRenderMap {
    if (aRenderMap == nil)
        [NSException raise:NSInvalidArgumentException format:@"rendermap must not be nil"];
    
    renderMap = [aRenderMap retain];
}

- (void)setTextureManager:(TextureManager *)theTextureManager {
    if (theTextureManager == nil)
        [NSException raise:NSInvalidArgumentException format:@"texture manager must not be nil"];
    
    textureManager = [theTextureManager retain];
}

- (void)dealloc {
    [[NSNotificationCenter defaultCenter] removeObserver:self];
    [renderMap release];
    [camera release];
    [textureManager release];
    [super dealloc];
}

@end
