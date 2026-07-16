/*
 * SPDX-FileCopyrightText: 2025 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 *
 * Declarations for symbols exported by vendor libpwirishalwrapper.so.
 * Expanded 2026-07-16 for Ace 3 Iris 7P HWC bring-up (MEMC Phase 2 / 2.7).
 * Keep signatures aligned with: nm -D libpwirishalwrapper.so | c++filt
 */

#pragma once

#include <cstdint>

#include <cutils/native_handle.h>
#include <hardware/hwcomposer_defs.h>

namespace sdm {
struct DisplayConfigVariableInfo;
struct LayerStack;
}  // namespace sdm

namespace pxlw {

class PxlwIrisWrapper {
 public:
  static PxlwIrisWrapper *GetInstance();

  // HWC callbacks (stock registers these at bring-up).
  void SetRefreshCB(void (*cb)(void *), void *cookie);
  void SetDsiClkCB(bool (*cb)(void *, bool), void *cookie);
  void SetPanelOscStateCB(bool (*cb)(void *, bool), void *cookie);
  void SetApColorModeCB(void (*cb)(void *, bool), void *cookie);
  void SetAP2ndPowerCB(void (*cb)(void *, bool), void *cookie);
  void SetCaliPatternCB(int (*cb)(void *, unsigned int), void *cookie);

 private:
  PxlwIrisWrapper() = default;
};

// HW iris 7 / 7P wrapper — used when SUPPORTS_PXLW_IRIS7 is defined.
// GetInstance() returns this concrete object (cast via AsIris7Wrapper).
class PxlwIris7Wrapper {
 public:
  PxlwIris7Wrapper();
  virtual ~PxlwIris7Wrapper();

  bool HasIris();
  bool HasIrisDual();
  bool HasSoftIris();
  bool HasIrisDualWithoutCSC();

  int InitPrimaryDisplay(int vsync_period_ns, unsigned int width, unsigned int height);

  void BeforeSetPowerMode(unsigned long display, int mode, bool from_event);
  void AfterSetPowerMode(unsigned long display, int mode, bool from_event);

  void SetActiveConfig(int display_type, int display_id, sdm::DisplayConfigVariableInfo *info);
  void SetActiveConfig(int display_type, int display_id, unsigned int config_index,
                       sdm::DisplayConfigVariableInfo *info);

  void SetColorModeWithRenderIntent(int display_id, int unused, int colorMode, int renderIntent);

  // Scalar present (no LayerStack ABI risk) — Phase 2 first wire.
  void PresentDisplay(unsigned long display);

  // LayerStack path — Phase 2.5+ (ABI-sensitive vs our libsdmcore).
  int BuildLayerStack(int a, int b, sdm::LayerStack *stack, int *out);
  void Present(int a, int b, bool &flag, sdm::LayerStack *stack);
  void BeforeCommitLayerStack(int a, int b, int &x, int &y, int &z);
  void AfterCommitLayerStack(int a, int b, int x, int y);

  void SetDisplayConnected(unsigned long display, bool connected);

  // Layer identity path (Phase 2.7) — feeds IrisService IrisLayer vector used by
  // checkEnterMemcAllow / FRC. Prefer video path (BUFFER_TYPE_VIDEO / YUV).
  // Signatures match libpwirishalwrapper.so (Ace 3 ODM).
  void CreateLayer(unsigned long display, unsigned long layer_id);
  void DestroyLayer(unsigned long display, unsigned long layer_id);
  void SetLayerBuffer(unsigned long display, unsigned long layer_id,
                      const native_handle_t *handle, int acquire_fence);
  void SetLayerZOrder(unsigned long display, unsigned long layer_id, unsigned int z);
  void SetLayerDisplayFrame(unsigned long display, unsigned long layer_id, hwc_rect frame);
  void SetLayerSourceCrop(unsigned long display, unsigned long layer_id, hwc_rect crop);
  void SetLayerTransform(unsigned long display, unsigned long layer_id, int transform);
  void SetLayerCompositionType(unsigned long display, unsigned long layer_id, int type);
  void SetLayerPerFrameMetadata(unsigned long display, unsigned long layer_id,
                                unsigned int num_elements, const int *keys, const float *metadata);
  void SetLayerSetEmpty(int display_type, int display_id, bool empty);
  void SetClientTarget(unsigned long display, int acquire_fence);
  void ChangeLayerType(unsigned long layer_id, int a, int b);

 private:
  friend class PxlwIrisWrapper;
};

class PxlwSoftirisWrapper {
 public:
  PxlwSoftirisWrapper();
  virtual ~PxlwSoftirisWrapper();

  bool HasSoftIris();

  int InitPrimaryDisplay(int vsync_period_ns, unsigned int width, unsigned int height);
  void SetColorModeWithRenderIntent(int display_id, int unused, int colorMode, int renderIntent);

  void BeforeSetPowerMode(unsigned long display, int mode, bool from_event);
  void AfterSetPowerMode(unsigned long display, int mode, bool from_event);
  void SetActiveConfig(int display_type, int display_id, sdm::DisplayConfigVariableInfo *info);
  void Present(int a, int b, bool &flag, sdm::LayerStack *stack);
  void BeforeCommitLayerStack(int a, int b, int &x, int &y, int &z);
  void AfterCommitLayerStack(int a, int b, int x, int y);

 private:
  friend class PxlwIrisWrapper;
};

inline PxlwIris7Wrapper *AsIris7Wrapper(PxlwIrisWrapper *w) {
  return w ? reinterpret_cast<PxlwIris7Wrapper *>(w) : nullptr;
}

inline PxlwSoftirisWrapper *AsSoftirisWrapper(PxlwIrisWrapper *w) {
  return w ? reinterpret_cast<PxlwSoftirisWrapper *>(w) : nullptr;
}

}  // namespace pxlw
