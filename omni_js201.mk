#
# Copyright (C) 2022 The Android Open Source Project
# Copyright (C) 2022 SebaUbuntu's TWRP device tree generator
#
# SPDX-License-Identifier: Apache-2.0
$(call inherit-product, $(SRC_TARGET_DIR)/product/base.mk)
$(call inherit-product, vendor/omni/config/common.mk)
$(call inherit-product, build/target/product/embedded.mk)
$(call inherit-product, device/kyocera/js201/device.mk)

PRODUCT_DEVICE := js201
PRODUCT_NAME := omni_js201
PRODUCT_RELEASE_NAME := omni_js201
PRODUCT_BRAND := JUSTSYSTEMS
PRODUCT_MODEL := SZJ-JS201
PRODUCT_MANUFACTURER := kyocera
PRODUCT_GMS_CLIENTID_BASE := android-kyocera
PRODUCT_LOCALES := ja_JP en_US en_AU
PRODUCT_DEFAULT_LANGUAGE := ja
PRODUCT_DEFAULT_REGION := JP
PRODUCT_AAPT_CONFIG := large mdpi hdpi
PRODUCT_AAPT_PREF_CONFIG := mdpi
TARGET_SCREEN_WIDTH := 1280
TARGET_SCREEN_HEIGHT := 800
PRODUCT_RESTRICT_VENDOR_FILES := false
TEMPORARY_DISABLE_PATH_RESTRICTIONS := true


PRODUCT_BUILD_PROP_OVERRIDES += \
    PRIVATE_BUILD_DESC="SZJ201-user 5.1.1 105.0.3530 105.0.3530 test-keys"

BUILD_FINGERPRINT := JUSTSYSTEMS/SZJ201/SZJ201:5.1.1/105.0.3530/105.0.3530:user/test-keys
