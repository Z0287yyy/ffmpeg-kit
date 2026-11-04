/*
 * Runtime bindings for libcamera2ndk.so and the API 24 image-reader symbols in
 * libmediandk.so. libavdevice links this object instead of those system libraries,
 * so the FFmpeg libraries can still load on API 23. Camera capture is used only
 * when the system libraries are present (API 24+).
 */

#include "ffmpegkit_camera_ndk.h"

#include <dlfcn.h>
#include <pthread.h>

typedef struct CameraNdk {
    ACameraManager *(*ACameraManager_create)(void);
    void (*ACameraManager_delete)(ACameraManager *);
    camera_status_t (*ACameraManager_getCameraIdList)(ACameraManager *, ACameraIdList **);
    void (*ACameraManager_deleteCameraIdList)(ACameraIdList *);
    camera_status_t (*ACameraManager_getCameraCharacteristics)(ACameraManager *, const char *, ACameraMetadata **);
    camera_status_t (*ACameraManager_openCamera)(ACameraManager *, const char *, ACameraDevice_StateCallbacks *, ACameraDevice **);
    const char *(*ACameraDevice_getId)(const ACameraDevice *);
    camera_status_t (*ACameraDevice_close)(ACameraDevice *);
    camera_status_t (*ACameraDevice_createCaptureRequest)(const ACameraDevice *, ACameraDevice_request_template, ACaptureRequest **);
    camera_status_t (*ACameraDevice_createCaptureSession)(ACameraDevice *, const ACaptureSessionOutputContainer *, const ACameraCaptureSession_stateCallbacks *, ACameraCaptureSession **);
    camera_status_t (*ACameraMetadata_getConstEntry)(const ACameraMetadata *, uint32_t, ACameraMetadata_const_entry *);
    void (*ACameraMetadata_free)(ACameraMetadata *);
    camera_status_t (*ACaptureSessionOutputContainer_create)(ACaptureSessionOutputContainer **);
    void (*ACaptureSessionOutputContainer_free)(ACaptureSessionOutputContainer *);
    camera_status_t (*ACaptureSessionOutputContainer_add)(ACaptureSessionOutputContainer *, const ACaptureSessionOutput *);
    camera_status_t (*ACaptureSessionOutputContainer_remove)(ACaptureSessionOutputContainer *, const ACaptureSessionOutput *);
    camera_status_t (*ACaptureSessionOutput_create)(ANativeWindow *, ACaptureSessionOutput **);
    void (*ACaptureSessionOutput_free)(ACaptureSessionOutput *);
    camera_status_t (*ACameraOutputTarget_create)(ANativeWindow *, ACameraOutputTarget **);
    void (*ACameraOutputTarget_free)(ACameraOutputTarget *);
    camera_status_t (*ACaptureRequest_addTarget)(ACaptureRequest *, const ACameraOutputTarget *);
    camera_status_t (*ACaptureRequest_removeTarget)(ACaptureRequest *, const ACameraOutputTarget *);
    camera_status_t (*ACaptureRequest_setEntry_i32)(ACaptureRequest *, uint32_t, uint32_t, const int32_t *);
    void (*ACaptureRequest_free)(ACaptureRequest *);
    camera_status_t (*ACameraCaptureSession_setRepeatingRequest)(ACameraCaptureSession *, void *, int, ACaptureRequest **, int *);
    camera_status_t (*ACameraCaptureSession_stopRepeating)(ACameraCaptureSession *);
    void (*ACameraCaptureSession_close)(ACameraCaptureSession *);
    media_status_t (*AImageReader_new)(int32_t, int32_t, int32_t, int32_t, AImageReader **);
    void (*AImageReader_delete)(AImageReader *);
    media_status_t (*AImageReader_getWindow)(AImageReader *, ANativeWindow **);
    media_status_t (*AImageReader_setImageListener)(AImageReader *, AImageReader_ImageListener *);
    media_status_t (*AImageReader_acquireLatestImage)(AImageReader *, AImage **);
    void (*AImage_delete)(AImage *);
    media_status_t (*AImage_getTimestamp)(const AImage *, int64_t *);
    media_status_t (*AImage_getPlaneRowStride)(const AImage *, int, int32_t *);
    media_status_t (*AImage_getPlanePixelStride)(const AImage *, int, int32_t *);
    media_status_t (*AImage_getPlaneData)(const AImage *, int, uint8_t **, int *);
} CameraNdk;

static CameraNdk ndk;
static int ndk_ready;
static pthread_once_t ndk_once = PTHREAD_ONCE_INIT;

#define LOAD(handle, name)                     \
    do {                                       \
        ndk.name = dlsym((handle), #name);    \
        if (ndk.name == NULL) {                \
            return;                            \
        }                                      \
    } while (0)

static void load_camera_ndk(void) {
    void *camera = dlopen("libcamera2ndk.so", RTLD_NOW);
    void *media = dlopen("libmediandk.so", RTLD_NOW);
    if (camera == NULL || media == NULL) {
        return;
    }

    LOAD(camera, ACameraManager_create);
    LOAD(camera, ACameraManager_delete);
    LOAD(camera, ACameraManager_getCameraIdList);
    LOAD(camera, ACameraManager_deleteCameraIdList);
    LOAD(camera, ACameraManager_getCameraCharacteristics);
    LOAD(camera, ACameraManager_openCamera);
    LOAD(camera, ACameraDevice_getId);
    LOAD(camera, ACameraDevice_close);
    LOAD(camera, ACameraDevice_createCaptureRequest);
    LOAD(camera, ACameraDevice_createCaptureSession);
    LOAD(camera, ACameraMetadata_getConstEntry);
    LOAD(camera, ACameraMetadata_free);
    LOAD(camera, ACaptureSessionOutputContainer_create);
    LOAD(camera, ACaptureSessionOutputContainer_free);
    LOAD(camera, ACaptureSessionOutputContainer_add);
    LOAD(camera, ACaptureSessionOutputContainer_remove);
    LOAD(camera, ACaptureSessionOutput_create);
    LOAD(camera, ACaptureSessionOutput_free);
    LOAD(camera, ACameraOutputTarget_create);
    LOAD(camera, ACameraOutputTarget_free);
    LOAD(camera, ACaptureRequest_addTarget);
    LOAD(camera, ACaptureRequest_removeTarget);
    LOAD(camera, ACaptureRequest_setEntry_i32);
    LOAD(camera, ACaptureRequest_free);
    LOAD(camera, ACameraCaptureSession_setRepeatingRequest);
    LOAD(camera, ACameraCaptureSession_stopRepeating);
    LOAD(camera, ACameraCaptureSession_close);
    LOAD(media, AImageReader_new);
    LOAD(media, AImageReader_delete);
    LOAD(media, AImageReader_getWindow);
    LOAD(media, AImageReader_setImageListener);
    LOAD(media, AImageReader_acquireLatestImage);
    LOAD(media, AImage_delete);
    LOAD(media, AImage_getTimestamp);
    LOAD(media, AImage_getPlaneRowStride);
    LOAD(media, AImage_getPlanePixelStride);
    LOAD(media, AImage_getPlaneData);
    ndk_ready = 1;
}

static int camera_ndk_available(void) {
    pthread_once(&ndk_once, load_camera_ndk);
    return ndk_ready;
}

ACameraManager *ACameraManager_create(void) {
    if (!camera_ndk_available()) {
        return NULL;
    }
    return ndk.ACameraManager_create();
}

void ACameraManager_delete(ACameraManager *manager) {
    if (camera_ndk_available()) {
        ndk.ACameraManager_delete(manager);
    }
}

camera_status_t ACameraManager_getCameraIdList(ACameraManager *manager, ACameraIdList **cameraIdList) {
    if (!camera_ndk_available()) {
        return ACAMERA_ERROR_CAMERA_DISCONNECTED;
    }
    return ndk.ACameraManager_getCameraIdList(manager, cameraIdList);
}

void ACameraManager_deleteCameraIdList(ACameraIdList *cameraIdList) {
    if (camera_ndk_available()) {
        ndk.ACameraManager_deleteCameraIdList(cameraIdList);
    }
}

camera_status_t ACameraManager_getCameraCharacteristics(ACameraManager *manager, const char *cameraId, ACameraMetadata **characteristics) {
    if (!camera_ndk_available()) {
        return ACAMERA_ERROR_CAMERA_DISCONNECTED;
    }
    return ndk.ACameraManager_getCameraCharacteristics(manager, cameraId, characteristics);
}

camera_status_t ACameraManager_openCamera(ACameraManager *manager, const char *cameraId, ACameraDevice_StateCallbacks *callback, ACameraDevice **device) {
    if (!camera_ndk_available()) {
        return ACAMERA_ERROR_CAMERA_DISCONNECTED;
    }
    return ndk.ACameraManager_openCamera(manager, cameraId, callback, device);
}

const char *ACameraDevice_getId(const ACameraDevice *device) {
    if (!camera_ndk_available()) {
        return NULL;
    }
    return ndk.ACameraDevice_getId(device);
}

camera_status_t ACameraDevice_close(ACameraDevice *device) {
    if (!camera_ndk_available()) {
        return ACAMERA_ERROR_CAMERA_DISCONNECTED;
    }
    return ndk.ACameraDevice_close(device);
}

camera_status_t ACameraDevice_createCaptureRequest(const ACameraDevice *device, ACameraDevice_request_template templateId, ACaptureRequest **request) {
    if (!camera_ndk_available()) {
        return ACAMERA_ERROR_CAMERA_DISCONNECTED;
    }
    return ndk.ACameraDevice_createCaptureRequest(device, templateId, request);
}

camera_status_t ACameraDevice_createCaptureSession(ACameraDevice *device, const ACaptureSessionOutputContainer *outputs, const ACameraCaptureSession_stateCallbacks *callbacks, ACameraCaptureSession **session) {
    if (!camera_ndk_available()) {
        return ACAMERA_ERROR_CAMERA_DISCONNECTED;
    }
    return ndk.ACameraDevice_createCaptureSession(device, outputs, callbacks, session);
}

camera_status_t ACameraMetadata_getConstEntry(const ACameraMetadata *metadata, uint32_t tag, ACameraMetadata_const_entry *entry) {
    if (!camera_ndk_available()) {
        return ACAMERA_ERROR_CAMERA_DISCONNECTED;
    }
    return ndk.ACameraMetadata_getConstEntry(metadata, tag, entry);
}

void ACameraMetadata_free(ACameraMetadata *metadata) {
    if (camera_ndk_available()) {
        ndk.ACameraMetadata_free(metadata);
    }
}

camera_status_t ACaptureSessionOutputContainer_create(ACaptureSessionOutputContainer **container) {
    if (!camera_ndk_available()) {
        return ACAMERA_ERROR_CAMERA_DISCONNECTED;
    }
    return ndk.ACaptureSessionOutputContainer_create(container);
}

void ACaptureSessionOutputContainer_free(ACaptureSessionOutputContainer *container) {
    if (camera_ndk_available()) {
        ndk.ACaptureSessionOutputContainer_free(container);
    }
}

camera_status_t ACaptureSessionOutputContainer_add(ACaptureSessionOutputContainer *container, const ACaptureSessionOutput *output) {
    if (!camera_ndk_available()) {
        return ACAMERA_ERROR_CAMERA_DISCONNECTED;
    }
    return ndk.ACaptureSessionOutputContainer_add(container, output);
}

camera_status_t ACaptureSessionOutputContainer_remove(ACaptureSessionOutputContainer *container, const ACaptureSessionOutput *output) {
    if (!camera_ndk_available()) {
        return ACAMERA_ERROR_CAMERA_DISCONNECTED;
    }
    return ndk.ACaptureSessionOutputContainer_remove(container, output);
}

camera_status_t ACaptureSessionOutput_create(ANativeWindow *anw, ACaptureSessionOutput **output) {
    if (!camera_ndk_available()) {
        return ACAMERA_ERROR_CAMERA_DISCONNECTED;
    }
    return ndk.ACaptureSessionOutput_create(anw, output);
}

void ACaptureSessionOutput_free(ACaptureSessionOutput *output) {
    if (camera_ndk_available()) {
        ndk.ACaptureSessionOutput_free(output);
    }
}

camera_status_t ACameraOutputTarget_create(ANativeWindow *window, ACameraOutputTarget **output) {
    if (!camera_ndk_available()) {
        return ACAMERA_ERROR_CAMERA_DISCONNECTED;
    }
    return ndk.ACameraOutputTarget_create(window, output);
}

void ACameraOutputTarget_free(ACameraOutputTarget *output) {
    if (camera_ndk_available()) {
        ndk.ACameraOutputTarget_free(output);
    }
}

camera_status_t ACaptureRequest_addTarget(ACaptureRequest *request, const ACameraOutputTarget *output) {
    if (!camera_ndk_available()) {
        return ACAMERA_ERROR_CAMERA_DISCONNECTED;
    }
    return ndk.ACaptureRequest_addTarget(request, output);
}

camera_status_t ACaptureRequest_removeTarget(ACaptureRequest *request, const ACameraOutputTarget *output) {
    if (!camera_ndk_available()) {
        return ACAMERA_ERROR_CAMERA_DISCONNECTED;
    }
    return ndk.ACaptureRequest_removeTarget(request, output);
}

camera_status_t ACaptureRequest_setEntry_i32(ACaptureRequest *request, uint32_t tag, uint32_t count, const int32_t *data) {
    if (!camera_ndk_available()) {
        return ACAMERA_ERROR_CAMERA_DISCONNECTED;
    }
    return ndk.ACaptureRequest_setEntry_i32(request, tag, count, data);
}

void ACaptureRequest_free(ACaptureRequest *request) {
    if (camera_ndk_available()) {
        ndk.ACaptureRequest_free(request);
    }
}

camera_status_t ACameraCaptureSession_setRepeatingRequest(ACameraCaptureSession *session, void *callbacks, int numRequests, ACaptureRequest **requests, int *captureSequenceId) {
    if (!camera_ndk_available()) {
        return ACAMERA_ERROR_CAMERA_DISCONNECTED;
    }
    return ndk.ACameraCaptureSession_setRepeatingRequest(session, callbacks, numRequests, requests, captureSequenceId);
}

camera_status_t ACameraCaptureSession_stopRepeating(ACameraCaptureSession *session) {
    if (!camera_ndk_available()) {
        return ACAMERA_ERROR_CAMERA_DISCONNECTED;
    }
    return ndk.ACameraCaptureSession_stopRepeating(session);
}

void ACameraCaptureSession_close(ACameraCaptureSession *session) {
    if (camera_ndk_available()) {
        ndk.ACameraCaptureSession_close(session);
    }
}

media_status_t AImageReader_new(int32_t width, int32_t height, int32_t format, int32_t maxImages, AImageReader **reader) {
    if (!camera_ndk_available()) {
        return AMEDIA_ERROR_UNKNOWN;
    }
    return ndk.AImageReader_new(width, height, format, maxImages, reader);
}

void AImageReader_delete(AImageReader *reader) {
    if (camera_ndk_available()) {
        ndk.AImageReader_delete(reader);
    }
}

media_status_t AImageReader_getWindow(AImageReader *reader, ANativeWindow **window) {
    if (!camera_ndk_available()) {
        return AMEDIA_ERROR_UNKNOWN;
    }
    return ndk.AImageReader_getWindow(reader, window);
}

media_status_t AImageReader_setImageListener(AImageReader *reader, AImageReader_ImageListener *listener) {
    if (!camera_ndk_available()) {
        return AMEDIA_ERROR_UNKNOWN;
    }
    return ndk.AImageReader_setImageListener(reader, listener);
}

media_status_t AImageReader_acquireLatestImage(AImageReader *reader, AImage **image) {
    if (!camera_ndk_available()) {
        return AMEDIA_ERROR_UNKNOWN;
    }
    return ndk.AImageReader_acquireLatestImage(reader, image);
}

void AImage_delete(AImage *image) {
    if (camera_ndk_available()) {
        ndk.AImage_delete(image);
    }
}

media_status_t AImage_getTimestamp(const AImage *image, int64_t *timestampNs) {
    if (!camera_ndk_available()) {
        return AMEDIA_ERROR_UNKNOWN;
    }
    return ndk.AImage_getTimestamp(image, timestampNs);
}

media_status_t AImage_getPlaneRowStride(const AImage *image, int planeIdx, int32_t *rowStride) {
    if (!camera_ndk_available()) {
        return AMEDIA_ERROR_UNKNOWN;
    }
    return ndk.AImage_getPlaneRowStride(image, planeIdx, rowStride);
}

media_status_t AImage_getPlanePixelStride(const AImage *image, int planeIdx, int32_t *pixelStride) {
    if (!camera_ndk_available()) {
        return AMEDIA_ERROR_UNKNOWN;
    }
    return ndk.AImage_getPlanePixelStride(image, planeIdx, pixelStride);
}

media_status_t AImage_getPlaneData(const AImage *image, int planeIdx, uint8_t **data, int *dataLength) {
    if (!camera_ndk_available()) {
        return AMEDIA_ERROR_UNKNOWN;
    }
    return ndk.AImage_getPlaneData(image, planeIdx, data, dataLength);
}
