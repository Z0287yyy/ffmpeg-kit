#!/bin/bash

# UPDATE BUILD FLAGS
# cpu-features.h is installed by this build. Pass that path directly so the
# compile does not depend on pkg-config being available.
export CFLAGS="$CFLAGS -I${LIB_INSTALL_BASE}/cpu-features/include/ndk_compat"
export LDFLAGS="$LDFLAGS -L${LIB_INSTALL_BASE}/cpu-features/lib -lndk_compat"

if [[ -f "${INSTALL_PKG_CONFIG_DIR}/libiconv.pc" ]] && command -v pkg-config >/dev/null 2>&1; then
  export CFLAGS="$CFLAGS $(pkg-config --cflags libiconv)"
  export LDFLAGS="$LDFLAGS $(pkg-config --libs --static libiconv)"
fi

# SET BUILD OPTIONS
case ${ARCH} in
arm-v7a-neon)
  ASM_OPTIONS=arm
  CFLAGS+=" -DHAVE_NEON -DANDROID_NDK"
  ;;
arm64-v8a)
  ASM_OPTIONS=arm64
  CFLAGS+=" -DHAVE_NEON_AARCH64 -DANDROID_NDK"
  ;;
x86*)
  ASM_OPTIONS=x86
  CFLAGS+=" -DHAVE_AVX2 -DANDROID_NDK"
  ;;
esac

# ALWAYS CLEAN THE PREVIOUS BUILD
make clean 2>/dev/null 1>/dev/null

# DISCARD APPLE WORKAROUNDS
git checkout "${BASEDIR}"/src/"${LIB_NAME}"/build || return 1
git checkout "${BASEDIR}"/src/"${LIB_NAME}"/codec || return 1

# WORKAROUND TO DISABLE PARTS THAT COMPILE cpu-features INTO libopenh264.a
${SED_INLINE} 's/^COMMON_INCLUDES +=/# COMMON_INCLUDES +=/' "${BASEDIR}"/src/"${LIB_NAME}"/build/platform-android.mk
${SED_INLINE} 's/^COMMON_OBJS +=/# COMMON_OBJS +=/' "${BASEDIR}"/src/"${LIB_NAME}"/build/platform-android.mk
${SED_INLINE} 's/^COMMON_CFLAGS +=/# COMMON_CFLAGS +=/' "${BASEDIR}"/src/"${LIB_NAME}"/build/platform-android.mk

make -j$(get_cpu_count) \
  ARCH="$(get_toolchain_arch)" \
  CC="$CC" \
  CFLAGS="$CFLAGS" \
  CXX="$CXX" \
  CXXFLAGS="${CXXFLAGS}" \
  LDFLAGS="${LDFLAGS}" \
  OS=android \
  PREFIX="${LIB_INSTALL_PREFIX}" \
  NDKLEVEL="${API}" \
  NDKROOT="${ANDROID_NDK_ROOT}" \
  NDK_TOOLCHAIN_VERSION=clang \
  AR="$AR" \
  ASM_OPTIONS=${ASM_OPTIONS} \
  TARGET="android-${API}" install-static || return 1

# MANUALLY COPY PKG-CONFIG FILES
cp "${BASEDIR}"/src/"${LIB_NAME}"/openh264-static.pc "${INSTALL_PKG_CONFIG_DIR}"/openh264.pc || return 1
