/*
 * Declarations for the Camera2 NDK surface used by FFmpeg's android_camera indev.
 * Types and constants match the NDK headers, without availability attributes, so this
 * header can be compiled into an API 23 binary. The symbols are defined by
 * camera_ndk_loader.c and resolved from the system libraries at runtime.
 */

#ifndef FFMPEGKIT_CAMERA_NDK_H
#define FFMPEGKIT_CAMERA_NDK_H

#include <android/native_window.h>
#include <stdint.h>

typedef enum {
    ACAMERA_OK = 0,
    ACAMERA_ERROR_BASE = -10000,
    ACAMERA_ERROR_UNKNOWN = ACAMERA_ERROR_BASE,
    ACAMERA_ERROR_INVALID_PARAMETER = ACAMERA_ERROR_BASE - 1,
    ACAMERA_ERROR_CAMERA_DISCONNECTED = ACAMERA_ERROR_BASE - 2,
    ACAMERA_ERROR_NOT_ENOUGH_MEMORY = ACAMERA_ERROR_BASE - 3,
    ACAMERA_ERROR_METADATA_NOT_FOUND = ACAMERA_ERROR_BASE - 4,
    ACAMERA_ERROR_CAMERA_DEVICE = ACAMERA_ERROR_BASE - 5,
    ACAMERA_ERROR_CAMERA_SERVICE = ACAMERA_ERROR_BASE - 6,
    ACAMERA_ERROR_SESSION_CLOSED = ACAMERA_ERROR_BASE - 7,
    ACAMERA_ERROR_INVALID_OPERATION = ACAMERA_ERROR_BASE - 8,
    ACAMERA_ERROR_STREAM_CONFIGURE_FAIL = ACAMERA_ERROR_BASE - 9,
    ACAMERA_ERROR_CAMERA_IN_USE = ACAMERA_ERROR_BASE - 10,
    ACAMERA_ERROR_MAX_CAMERA_IN_USE = ACAMERA_ERROR_BASE - 11,
    ACAMERA_ERROR_CAMERA_DISABLED = ACAMERA_ERROR_BASE - 12,
    ACAMERA_ERROR_PERMISSION_DENIED = ACAMERA_ERROR_BASE - 13
} camera_status_t;

typedef enum {
    AMEDIA_OK = 0,
    AMEDIA_ERROR_BASE = -10000,
    AMEDIA_ERROR_UNKNOWN = AMEDIA_ERROR_BASE,
    AMEDIA_ERROR_MALFORMED = AMEDIA_ERROR_BASE - 1,
    AMEDIA_ERROR_UNSUPPORTED = AMEDIA_ERROR_BASE - 2,
    AMEDIA_ERROR_INVALID_OBJECT = AMEDIA_ERROR_BASE - 3,
    AMEDIA_ERROR_INVALID_PARAMETER = AMEDIA_ERROR_BASE - 4,
    AMEDIA_ERROR_INVALID_OPERATION = AMEDIA_ERROR_BASE - 5,
    AMEDIA_DRM_ERROR_BASE = -20000,
    AMEDIA_DRM_NOT_PROVISIONED = AMEDIA_DRM_ERROR_BASE - 1,
    AMEDIA_DRM_RESOURCE_BUSY = AMEDIA_DRM_ERROR_BASE - 2,
    AMEDIA_DRM_DEVICE_REVOKED = AMEDIA_DRM_ERROR_BASE - 3,
    AMEDIA_DRM_SHORT_BUFFER = AMEDIA_DRM_ERROR_BASE - 4,
    AMEDIA_DRM_SESSION_NOT_OPENED = AMEDIA_DRM_ERROR_BASE - 5,
    AMEDIA_DRM_TAMPER_DETECTED = AMEDIA_DRM_ERROR_BASE - 6,
    AMEDIA_DRM_VERIFY_FAILED = AMEDIA_DRM_ERROR_BASE - 7,
    AMEDIA_DRM_NEED_KEY = AMEDIA_DRM_ERROR_BASE - 8,
    AMEDIA_DRM_LICENSE_EXPIRED = AMEDIA_DRM_ERROR_BASE - 9,
    AMEDIA_IMGREADER_ERROR_BASE = -30000,
    AMEDIA_IMGREADER_NO_BUFFER_AVAILABLE = AMEDIA_IMGREADER_ERROR_BASE - 1,
    AMEDIA_IMGREADER_MAX_IMAGES_ACQUIRED = AMEDIA_IMGREADER_ERROR_BASE - 2,
    AMEDIA_IMGREADER_CANNOT_LOCK_IMAGE = AMEDIA_IMGREADER_ERROR_BASE - 3,
    AMEDIA_IMGREADER_CANNOT_UNLOCK_IMAGE = AMEDIA_IMGREADER_ERROR_BASE - 4,
    AMEDIA_IMGREADER_IMAGE_NOT_LOCKED = AMEDIA_IMGREADER_ERROR_BASE - 5
} media_status_t;

enum {
    ERROR_CAMERA_IN_USE = 1,
    ERROR_MAX_CAMERAS_IN_USE = 2,
    ERROR_CAMERA_DISABLED = 3,
    ERROR_CAMERA_DEVICE = 4,
    ERROR_CAMERA_SERVICE = 5
};

typedef enum {
    TEMPLATE_RECORD = 3
} ACameraDevice_request_template;

#define AIMAGE_FORMAT_YUV_420_888 0x23

/* Section ids follow acamera_metadata_section in NdkCameraMetadataTags.h. */
#define ACAMERA_CONTROL_START (1 << 16)
#define ACAMERA_LENS_START (8 << 16)
#define ACAMERA_SCALER_START (13 << 16)
#define ACAMERA_SENSOR_START (14 << 16)
#define ACAMERA_CONTROL_AE_TARGET_FPS_RANGE (ACAMERA_CONTROL_START + 5)
#define ACAMERA_CONTROL_AE_AVAILABLE_TARGET_FPS_RANGES (ACAMERA_CONTROL_START + 20)
#define ACAMERA_LENS_FACING (ACAMERA_LENS_START + 5)
#define ACAMERA_LENS_FACING_FRONT 0
#define ACAMERA_SCALER_AVAILABLE_STREAM_CONFIGURATIONS (ACAMERA_SCALER_START + 10)
#define ACAMERA_SENSOR_ORIENTATION (ACAMERA_SENSOR_START + 14)

typedef struct ACameraManager ACameraManager;
typedef struct ACameraDevice ACameraDevice;
typedef struct ACameraMetadata ACameraMetadata;
typedef struct ACaptureSessionOutputContainer ACaptureSessionOutputContainer;
typedef struct ACaptureSessionOutput ACaptureSessionOutput;
typedef struct ACameraOutputTarget ACameraOutputTarget;
typedef struct ACaptureRequest ACaptureRequest;
typedef struct ACameraCaptureSession ACameraCaptureSession;
typedef struct AImageReader AImageReader;
typedef struct AImage AImage;

typedef struct ACameraMetadata_rational {
    int32_t numerator;
    int32_t denominator;
} ACameraMetadata_rational;

typedef struct ACameraMetadata_const_entry {
    uint32_t tag;
    uint8_t type;
    uint32_t count;
    union {
        const uint8_t *u8;
        const int32_t *i32;
        const float *f;
        const int64_t *i64;
        const double *d;
        const ACameraMetadata_rational *r;
    } data;
} ACameraMetadata_const_entry;

typedef struct ACameraIdList {
    int numCameras;
    const char **cameraIds;
} ACameraIdList;

typedef void (*ACameraDevice_StateCallback)(void *context, ACameraDevice *device);
typedef void (*ACameraDevice_ErrorStateCallback)(void *context, ACameraDevice *device, int error);

typedef struct ACameraDevice_StateCallbacks {
    void *context;
    ACameraDevice_StateCallback onDisconnected;
    ACameraDevice_ErrorStateCallback onError;
} ACameraDevice_StateCallbacks;

typedef void (*ACameraCaptureSession_stateCallback)(void *context, ACameraCaptureSession *session);

typedef struct ACameraCaptureSession_stateCallbacks {
    void *context;
    ACameraCaptureSession_stateCallback onClosed;
    ACameraCaptureSession_stateCallback onReady;
    ACameraCaptureSession_stateCallback onActive;
} ACameraCaptureSession_stateCallbacks;

typedef void (*AImageReader_ImageCallback)(void *context, AImageReader *reader);

typedef struct AImageReader_ImageListener {
    void *context;
    AImageReader_ImageCallback onImageAvailable;
} AImageReader_ImageListener;

ACameraManager *ACameraManager_create(void);
void ACameraManager_delete(ACameraManager *manager);
camera_status_t ACameraManager_getCameraIdList(ACameraManager *manager, ACameraIdList **cameraIdList);
void ACameraManager_deleteCameraIdList(ACameraIdList *cameraIdList);
camera_status_t ACameraManager_getCameraCharacteristics(ACameraManager *manager, const char *cameraId, ACameraMetadata **characteristics);
camera_status_t ACameraManager_openCamera(ACameraManager *manager, const char *cameraId, ACameraDevice_StateCallbacks *callback, ACameraDevice **device);
const char *ACameraDevice_getId(const ACameraDevice *device);
camera_status_t ACameraDevice_close(ACameraDevice *device);
camera_status_t ACameraDevice_createCaptureRequest(const ACameraDevice *device, ACameraDevice_request_template templateId, ACaptureRequest **request);
camera_status_t ACameraDevice_createCaptureSession(ACameraDevice *device, const ACaptureSessionOutputContainer *outputs, const ACameraCaptureSession_stateCallbacks *callbacks, ACameraCaptureSession **session);
camera_status_t ACameraMetadata_getConstEntry(const ACameraMetadata *metadata, uint32_t tag, ACameraMetadata_const_entry *entry);
void ACameraMetadata_free(ACameraMetadata *metadata);
camera_status_t ACaptureSessionOutputContainer_create(ACaptureSessionOutputContainer **container);
void ACaptureSessionOutputContainer_free(ACaptureSessionOutputContainer *container);
camera_status_t ACaptureSessionOutputContainer_add(ACaptureSessionOutputContainer *container, const ACaptureSessionOutput *output);
camera_status_t ACaptureSessionOutputContainer_remove(ACaptureSessionOutputContainer *container, const ACaptureSessionOutput *output);
camera_status_t ACaptureSessionOutput_create(ANativeWindow *anw, ACaptureSessionOutput **output);
void ACaptureSessionOutput_free(ACaptureSessionOutput *output);
camera_status_t ACameraOutputTarget_create(ANativeWindow *window, ACameraOutputTarget **output);
void ACameraOutputTarget_free(ACameraOutputTarget *output);
camera_status_t ACaptureRequest_addTarget(ACaptureRequest *request, const ACameraOutputTarget *output);
camera_status_t ACaptureRequest_removeTarget(ACaptureRequest *request, const ACameraOutputTarget *output);
camera_status_t ACaptureRequest_setEntry_i32(ACaptureRequest *request, uint32_t tag, uint32_t count, const int32_t *data);
void ACaptureRequest_free(ACaptureRequest *request);
camera_status_t ACameraCaptureSession_setRepeatingRequest(ACameraCaptureSession *session, void *callbacks, int numRequests, ACaptureRequest **requests, int *captureSequenceId);
camera_status_t ACameraCaptureSession_stopRepeating(ACameraCaptureSession *session);
void ACameraCaptureSession_close(ACameraCaptureSession *session);
media_status_t AImageReader_new(int32_t width, int32_t height, int32_t format, int32_t maxImages, AImageReader **reader);
void AImageReader_delete(AImageReader *reader);
media_status_t AImageReader_getWindow(AImageReader *reader, ANativeWindow **window);
media_status_t AImageReader_setImageListener(AImageReader *reader, AImageReader_ImageListener *listener);
media_status_t AImageReader_acquireLatestImage(AImageReader *reader, AImage **image);
void AImage_delete(AImage *image);
media_status_t AImage_getTimestamp(const AImage *image, int64_t *timestampNs);
media_status_t AImage_getPlaneRowStride(const AImage *image, int planeIdx, int32_t *rowStride);
media_status_t AImage_getPlanePixelStride(const AImage *image, int planeIdx, int32_t *pixelStride);
media_status_t AImage_getPlaneData(const AImage *image, int planeIdx, uint8_t **data, int *dataLength);

#endif
