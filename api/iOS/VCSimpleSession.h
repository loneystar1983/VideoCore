/*

 Video Core
 Copyright (c) 2014 James G. Hurley

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

 */

/*!
 *  A simple Objective-C Session API that will create an RTMP session using the
 *  device's camera(s) and microphone.
 *
 */

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <AVFoundation/AVFoundation.h>
#import <UIKit/UIKit.h>

#ifdef __APPLE__
#   ifdef TARGET_OS_IPHONE
#       include <videocore/sources/iOS/CameraSource.h>
#       include <videocore/sources/iOS/MicSource.h>
#   else /* OS X */
#   endif
#else
#   include <videocore/mixers/GenericAudioMixer.h>
#endif

@class VCSimpleSession;

typedef NS_ENUM(NSInteger, VCSessionState)
{
    VCSessionStateNone,
    VCSessionStatePreviewStarted,
    VCSessionStateStarting,
    VCSessionStateStarted,
    VCSessionStatePaused,
    VCSessionStateEnded,
    VCSessionStateError,
    VCSessionStateBufferOverflow
};

typedef NS_ENUM(NSInteger, VCCameraState)
{
    VCCameraStateFront,
    VCCameraStateBack
};

typedef NS_ENUM(NSInteger, VCAspectMode)
{
    VCAspectModeFit,
    VCAscpectModeFill
};

//With new filters should add an enum here
typedef NS_ENUM(NSInteger, VCFilter) {
    VCFilterNormal,
    VCFilterGray,
    VCFilterInvertColors,
    VCFilterSepia,
    VCFilterFisheye,
    VCFilterGlow
};

typedef NS_ENUM(NSInteger, VCConnectionQuality) {
    kVCConnectionQualityHigh,
    kVCConnectionQualityMedium,
    kVCConnectionQualityLow
};

#define kMinVideoBitrate  32000
#define kMaxBufferedDuration  1

#define kDefaultAudioChannelCount 2
#define kDefaultAudioGain 0.5f
#define kDefaultAudioSampleRate 44100
#define kDefaultAudioBitRate 96000
#define kDefaultAudioBytesPerChannel 2

@protocol VCSessionDelegate <NSObject>
@required
- (void) connectionStatusChanged: (VCSessionState) sessionState;
@optional
- (void) didChangeConnectionQuality:(VCConnectionQuality)connectionQuality;
- (void) didAddCameraSource:(VCSimpleSession*)session;

//Depreciated, should use method below
- (void) detectedThroughput: (NSInteger) throughputInBytesPerSecond;
- (void) detectedThroughput: (NSInteger) throughputInBytesPerSecond videoRate:(NSInteger) rate;
- (void) detectedThroughput: (NSInteger) throughputInBytesPerSecond videoRate:(NSInteger) rate audioRate:(NSInteger)aRate insBytesPerSecond:(NSInteger)insBytesPerSecond;
@end

@interface VCSimpleSession : NSObject {
@public
    std::shared_ptr<videocore::iOS::CameraSource> m_cameraSource;
    std::shared_ptr<videocore::iOS::MicSource> m_micSource;
    
    std::shared_ptr<videocore::ISource> m_extCameraSource;
    std::shared_ptr<videocore::ISource> m_extMicSource;
}
@property (nonatomic, readonly) VCSessionState rtmpSessionState;
@property (nonatomic, strong, readonly) UIView* previewView;

/*! Setters / Getters for session properties */
@property (nonatomic, assign) CGSize            videoSize;      // Change will not take place until the next RTMP Session
@property (nonatomic, assign) int               bitrate;        // Change will not take place until the next RTMP Session
@property (nonatomic, assign) int               fps;            // Change will not take place until the next RTMP Session
@property (nonatomic, assign, readonly) BOOL    useInterfaceOrientation;
@property (nonatomic, assign) VCCameraState cameraState;
@property (nonatomic, assign) BOOL          orientationLocked;
@property (nonatomic, assign) BOOL          torch;
@property (nonatomic, assign) float         videoZoomFactor;
@property (nonatomic, assign) int           audioChannelCount;
@property (nonatomic, assign) float         audioSampleRate;
@property (nonatomic, assign) int         audioBitRate;
// 麦克风增益
@property (nonatomic, assign) float         micGain;        // [0..1]
@property (nonatomic, assign) CGPoint       focusPointOfInterest;   // (0,0) is top-left, (1,1) is bottom-right
@property (nonatomic, assign) CGPoint       exposurePointOfInterest;
@property (nonatomic, assign) BOOL          continuousAutofocus;
@property (nonatomic, assign) BOOL          continuousExposure;
@property (nonatomic, assign) BOOL          useAdaptiveBitrate;     /* Default is off */
@property (nonatomic, readonly) int         estimatedThroughput;    /* Bytes Per Second. */
@property (nonatomic, assign) VCAspectMode  aspectMode;
@property (nonatomic, copy, readonly) NSString    *filePath;

@property (nonatomic, readonly) AVCaptureSession    *captureSession;

@property (nonatomic, assign) VCFilter      filter; /* Default is VCFilterNormal*/

@property (nonatomic, assign) id<VCSessionDelegate> delegate;

@property (nonatomic, assign) int maxSendBufferSize;

// -----------------------------------------------------------------------------
- (instancetype) initWithVideoSize:(CGSize)videoSize
                         frameRate:(int)fps
                           bitrate:(int)bps;

// -----------------------------------------------------------------------------
- (instancetype) initWithVideoSize:(CGSize)videoSize
                         frameRate:(int)fps
                           bitrate:(int)bps
           useInterfaceOrientation:(BOOL)useInterfaceOrientation;

// -----------------------------------------------------------------------------
- (instancetype) initWithVideoSize:(CGSize)videoSize
                         frameRate:(int)fps
                           bitrate:(int)bps
           useInterfaceOrientation:(BOOL)useInterfaceOrientation
                       cameraState:(VCCameraState) cameraState;

// -----------------------------------------------------------------------------
- (instancetype) initWithVideoSize:(CGSize)videoSize
                         frameRate:(int)fps
                           bitrate:(int)bps
           useInterfaceOrientation:(BOOL)useInterfaceOrientation
                       cameraState:(VCCameraState) cameraState
                        aspectMode:(VCAspectMode) aspectMode;

// -----------------------------------------------------------------------------
- (instancetype) initWithVideoSize:(CGSize)videoSize
                         frameRate:(int)fps
                           bitrate:(int)bps
           useInterfaceOrientation:(BOOL)useInterfaceOrientation
                       cameraState:(VCCameraState) cameraState
                        aspectMode:(VCAspectMode)aspectMode
                         extCamera:(std::shared_ptr<videocore::ISource>)extCamera
                            extMic:(std::shared_ptr<videocore::ISource>)extMic;

// -----------------------------------------------------------------------------

- (void) startRtmpSessionWithURL:(NSString *)rtmpUrl
                    andStreamKey:(NSString *)streamKey;

- (void) startRtmpSessionWithURL:(NSString *)rtmpUrl
                    andStreamKey:(NSString *)streamKey
                        filePath:(NSString *)path;

- (void) pauseRtmpSession;

- (void) continueRtmpSessionWithURL:(NSString *)rtmpUrl
                       andStreamKey:(NSString *)streamKey;

- (void) endRtmpSession;
- (void) endRtmpSessionWithCompletionHandler:(void(^)(void))handler;
/*!
 *  Note that the rect you provide should be based on your video dimensions.  The origin
 *  of the image will be the center of the image (so if you put 0,0 as its position, it will
 *  basically end up with the bottom-right quadrant of the image hanging out at the top-left corner of
 *  your video)
 */

- (void) addPixelBufferSource: (UIImage*) image
                     withRect: (CGRect) rect;

@end
