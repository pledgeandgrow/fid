[All modules](index.md)

# android_ndk -- Fides Documentation

## Contents

- [ANDROID_LOG_UNKNOWN:](#ANDROID_LOG_UNKNOWN:) (const)
- [ANDROID_LOG_DEFAULT:](#ANDROID_LOG_DEFAULT:) (const)
- [ANDROID_LOG_VERBOSE:](#ANDROID_LOG_VERBOSE:) (const)
- [ANDROID_LOG_DEBUG:](#ANDROID_LOG_DEBUG:) (const)
- [ANDROID_LOG_INFO:](#ANDROID_LOG_INFO:) (const)
- [ANDROID_LOG_WARN:](#ANDROID_LOG_WARN:) (const)
- [ANDROID_LOG_ERROR:](#ANDROID_LOG_ERROR:) (const)
- [ANDROID_LOG_FATAL:](#ANDROID_LOG_FATAL:) (const)
- [ANDROID_LOG_SILENT:](#ANDROID_LOG_SILENT:) (const)
- [NDK_LOG_MAX_PAYLOAD:](#NDK_LOG_MAX_PAYLOAD:) (const)
- [ALOOPER_PREPARE_ALLOW_NON_CALLBACKS:](#ALOOPER_PREPARE_ALLOW_NON_CALLBACKS:) (const)
- [ALOOPER_POLL_WAKE:](#ALOOPER_POLL_WAKE:) (const)
- [ALOOPER_POLL_CALLBACK:](#ALOOPER_POLL_CALLBACK:) (const)
- [ALOOPER_POLL_TIMEOUT:](#ALOOPER_POLL_TIMEOUT:) (const)
- [ALOOPER_POLL_ERROR:](#ALOOPER_POLL_ERROR:) (const)
- [ALOOPER_EVENT_INPUT:](#ALOOPER_EVENT_INPUT:) (const)
- [ALOOPER_EVENT_OUTPUT:](#ALOOPER_EVENT_OUTPUT:) (const)
- [ALOOPER_EVENT_ERROR:](#ALOOPER_EVENT_ERROR:) (const)
- [ALOOPER_EVENT_HANGUP:](#ALOOPER_EVENT_HANGUP:) (const)
- [LOOPER_ID_MAIN:](#LOOPER_ID_MAIN:) (const)
- [LOOPER_ID_INPUT:](#LOOPER_ID_INPUT:) (const)
- [LOOPER_ID_USER:](#LOOPER_ID_USER:) (const)
- [APP_CMD_INPUT_CHANGED:](#APP_CMD_INPUT_CHANGED:) (const)
- [APP_CMD_INIT_WINDOW:](#APP_CMD_INIT_WINDOW:) (const)
- [APP_CMD_TERM_WINDOW:](#APP_CMD_TERM_WINDOW:) (const)
- [APP_CMD_WINDOW_RESIZED:](#APP_CMD_WINDOW_RESIZED:) (const)
- [APP_CMD_WINDOW_REDRAW_NEEDED:](#APP_CMD_WINDOW_REDRAW_NEEDED:) (const)
- [APP_CMD_CONTENT_RECT_CHANGED:](#APP_CMD_CONTENT_RECT_CHANGED:) (const)
- [APP_CMD_GAINED_FOCUS:](#APP_CMD_GAINED_FOCUS:) (const)
- [APP_CMD_LOST_FOCUS:](#APP_CMD_LOST_FOCUS:) (const)
- [APP_CMD_CONFIG_CHANGED:](#APP_CMD_CONFIG_CHANGED:) (const)
- [APP_CMD_LOW_MEMORY:](#APP_CMD_LOW_MEMORY:) (const)
- [APP_CMD_START:](#APP_CMD_START:) (const)
- [APP_CMD_RESUME:](#APP_CMD_RESUME:) (const)
- [APP_CMD_SAVE_STATE:](#APP_CMD_SAVE_STATE:) (const)
- [APP_CMD_PAUSE:](#APP_CMD_PAUSE:) (const)
- [APP_CMD_STOP:](#APP_CMD_STOP:) (const)
- [APP_CMD_DESTROY:](#APP_CMD_DESTROY:) (const)
- [AINPUT_EVENT_TYPE_KEY:](#AINPUT_EVENT_TYPE_KEY:) (const)
- [AINPUT_EVENT_TYPE_MOTION:](#AINPUT_EVENT_TYPE_MOTION:) (const)
- [AINPUT_SOURCE_KEYBOARD:](#AINPUT_SOURCE_KEYBOARD:) (const)
- [AINPUT_SOURCE_DPAD:](#AINPUT_SOURCE_DPAD:) (const)
- [AINPUT_SOURCE_GAMEPAD:](#AINPUT_SOURCE_GAMEPAD:) (const)
- [AINPUT_SOURCE_TOUCHSCREEN:](#AINPUT_SOURCE_TOUCHSCREEN:) (const)
- [AINPUT_SOURCE_MOUSE:](#AINPUT_SOURCE_MOUSE:) (const)
- [AINPUT_SOURCE_STYLUS:](#AINPUT_SOURCE_STYLUS:) (const)
- [AINPUT_SOURCE_JOYSTICK:](#AINPUT_SOURCE_JOYSTICK:) (const)
- [AMOTION_EVENT_ACTION_MASK:](#AMOTION_EVENT_ACTION_MASK:) (const)
- [AMOTION_EVENT_ACTION_POINTER_INDEX_MASK:](#AMOTION_EVENT_ACTION_POINTER_INDEX_MASK:) (const)
- [AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT:](#AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT:) (const)
- [AMOTION_EVENT_ACTION_DOWN:](#AMOTION_EVENT_ACTION_DOWN:) (const)
- [AMOTION_EVENT_ACTION_UP:](#AMOTION_EVENT_ACTION_UP:) (const)
- [AMOTION_EVENT_ACTION_MOVE:](#AMOTION_EVENT_ACTION_MOVE:) (const)
- [AMOTION_EVENT_ACTION_CANCEL:](#AMOTION_EVENT_ACTION_CANCEL:) (const)
- [AMOTION_EVENT_ACTION_OUTSIDE:](#AMOTION_EVENT_ACTION_OUTSIDE:) (const)
- [AMOTION_EVENT_ACTION_POINTER_DOWN:](#AMOTION_EVENT_ACTION_POINTER_DOWN:) (const)
- [AMOTION_EVENT_ACTION_POINTER_UP:](#AMOTION_EVENT_ACTION_POINTER_UP:) (const)
- [AMOTION_EVENT_ACTION_HOVER_MOVE:](#AMOTION_EVENT_ACTION_HOVER_MOVE:) (const)
- [AMOTION_EVENT_ACTION_SCROLL:](#AMOTION_EVENT_ACTION_SCROLL:) (const)
- [AKEY_EVENT_ACTION_DOWN:](#AKEY_EVENT_ACTION_DOWN:) (const)
- [AKEY_EVENT_ACTION_UP:](#AKEY_EVENT_ACTION_UP:) (const)
- [AKEY_EVENT_ACTION_MULTIPLE:](#AKEY_EVENT_ACTION_MULTIPLE:) (const)
- [AMETA_ALT_ON:](#AMETA_ALT_ON:) (const)
- [AMETA_SHIFT_ON:](#AMETA_SHIFT_ON:) (const)
- [AMETA_CTRL_ON:](#AMETA_CTRL_ON:) (const)
- [AKEYCODE_BACK:](#AKEYCODE_BACK:) (const)
- [AKEYCODE_0:](#AKEYCODE_0:) (const)
- [AKEYCODE_DPAD_UP:](#AKEYCODE_DPAD_UP:) (const)
- [AKEYCODE_DPAD_DOWN:](#AKEYCODE_DPAD_DOWN:) (const)
- [AKEYCODE_DPAD_LEFT:](#AKEYCODE_DPAD_LEFT:) (const)
- [AKEYCODE_DPAD_RIGHT:](#AKEYCODE_DPAD_RIGHT:) (const)
- [AKEYCODE_DPAD_CENTER:](#AKEYCODE_DPAD_CENTER:) (const)
- [AKEYCODE_VOLUME_UP:](#AKEYCODE_VOLUME_UP:) (const)
- [AKEYCODE_VOLUME_DOWN:](#AKEYCODE_VOLUME_DOWN:) (const)
- [AKEYCODE_A:](#AKEYCODE_A:) (const)
- [AKEYCODE_SPACE:](#AKEYCODE_SPACE:) (const)
- [AKEYCODE_ENTER:](#AKEYCODE_ENTER:) (const)
- [AKEYCODE_DEL:](#AKEYCODE_DEL:) (const)
- [AKEYCODE_MENU:](#AKEYCODE_MENU:) (const)
- [AKEYCODE_ESCAPE:](#AKEYCODE_ESCAPE:) (const)
- [AKEYCODE_BUTTON_A:](#AKEYCODE_BUTTON_A:) (const)
- [AKEYCODE_BUTTON_B:](#AKEYCODE_BUTTON_B:) (const)
- [WINDOW_FORMAT_RGBA_8888:](#WINDOW_FORMAT_RGBA_8888:) (const)
- [WINDOW_FORMAT_RGBX_8888:](#WINDOW_FORMAT_RGBX_8888:) (const)
- [WINDOW_FORMAT_RGB_565:](#WINDOW_FORMAT_RGB_565:) (const)
- [AASSET_MODE_UNKNOWN:](#AASSET_MODE_UNKNOWN:) (const)
- [AASSET_MODE_RANDOM:](#AASSET_MODE_RANDOM:) (const)
- [AASSET_MODE_STREAMING:](#AASSET_MODE_STREAMING:) (const)
- [AASSET_MODE_BUFFER:](#AASSET_MODE_BUFFER:) (const)
- [NDK_ACTIVITY_CALLBACKS:](#NDK_ACTIVITY_CALLBACKS:) (const)
- [NDK_ACTIVITY_VM:](#NDK_ACTIVITY_VM:) (const)
- [NDK_ACTIVITY_ENV:](#NDK_ACTIVITY_ENV:) (const)
- [NDK_ACTIVITY_CLAZZ:](#NDK_ACTIVITY_CLAZZ:) (const)
- [NDK_ACTIVITY_INTERNAL_PATH:](#NDK_ACTIVITY_INTERNAL_PATH:) (const)
- [NDK_ACTIVITY_EXTERNAL_PATH:](#NDK_ACTIVITY_EXTERNAL_PATH:) (const)
- [NDK_ACTIVITY_SDK_VERSION:](#NDK_ACTIVITY_SDK_VERSION:) (const)
- [NDK_ACTIVITY_INSTANCE:](#NDK_ACTIVITY_INSTANCE:) (const)
- [NDK_ACTIVITY_ASSET_MANAGER:](#NDK_ACTIVITY_ASSET_MANAGER:) (const)
- [NDK_ACTIVITY_OBB_PATH:](#NDK_ACTIVITY_OBB_PATH:) (const)
- [ndk_callback_names](#ndk_callback_names) (function)
- [ndk_callback_offset](#ndk_callback_offset) (function)
- [ndk_callback_cmd](#ndk_callback_cmd) (function)
- [ndk_cmd_name](#ndk_cmd_name) (function)
- [NdkWindowBuffer](#NdkWindowBuffer) (struct)
- [ndk_read_window_buffer](#ndk_read_window_buffer) (function)
- [ndk_activity_sdk_version](#ndk_activity_sdk_version) (function)
- [ndk_activity_asset_manager](#ndk_activity_asset_manager) (function)
- [ndk_format_bpp](#ndk_format_bpp) (function)
- [ndk_pack_pixel](#ndk_pack_pixel) (function)
- [ndk_fill_rect](#ndk_fill_rect) (function)
- [NdkInput](#NdkInput) (struct)
- [ndk_no_input](#ndk_no_input) (function)
- [ndk_key_input](#ndk_key_input) (function)
- [ndk_motion_action](#ndk_motion_action) (function)
- [ndk_motion_pointer_index](#ndk_motion_pointer_index) (function)
- [ndk_motion_make_action](#ndk_motion_make_action) (function)
- [ndk_source_is](#ndk_source_is) (function)
- [NdkPointers](#NdkPointers) (struct)
- [ndk_pointers_new](#ndk_pointers_new) (function)
- [ndk_pointers_update](#ndk_pointers_update) (function)
- [NdkApp](#NdkApp) (struct)
- [ndk_app_new](#ndk_app_new) (function)
- [ndk_app_animating](#ndk_app_animating) (function)
- [ndk_poll_timeout](#ndk_poll_timeout) (function)
- [ndk_app_handle_cmd](#ndk_app_handle_cmd) (function)
- [ndk_app_handle_input](#ndk_app_handle_input) (function)
- [NdkLoopSeam](#NdkLoopSeam) (struct)
- [ndk_run_loop](#ndk_run_loop) (function)
- [ndk_log_letter](#ndk_log_letter) (function)
- [ndk_log_line](#ndk_log_line) (function)
- [ndk_log_chunks](#ndk_log_chunks) (function)
- [ndk_log](#ndk_log) (function)
- [ndk_asset_read_all](#ndk_asset_read_all) (function)

## ANDROID_LOG_UNKNOWN:

```fides
pub const ANDROID_LOG_UNKNOWN: i64 = 0;
```

*Defined at line 98*

---

## ANDROID_LOG_DEFAULT:

```fides
pub const ANDROID_LOG_DEFAULT: i64 = 1;
```

*Defined at line 99*

---

## ANDROID_LOG_VERBOSE:

```fides
pub const ANDROID_LOG_VERBOSE: i64 = 2;
```

*Defined at line 100*

---

## ANDROID_LOG_DEBUG:

```fides
pub const ANDROID_LOG_DEBUG: i64 = 3;
```

*Defined at line 101*

---

## ANDROID_LOG_INFO:

```fides
pub const ANDROID_LOG_INFO: i64 = 4;
```

*Defined at line 102*

---

## ANDROID_LOG_WARN:

```fides
pub const ANDROID_LOG_WARN: i64 = 5;
```

*Defined at line 103*

---

## ANDROID_LOG_ERROR:

```fides
pub const ANDROID_LOG_ERROR: i64 = 6;
```

*Defined at line 104*

---

## ANDROID_LOG_FATAL:

```fides
pub const ANDROID_LOG_FATAL: i64 = 7;
```

*Defined at line 105*

---

## ANDROID_LOG_SILENT:

```fides
pub const ANDROID_LOG_SILENT: i64 = 8;
```

*Defined at line 106*

---

## NDK_LOG_MAX_PAYLOAD:

LOGGER_ENTRY_MAX_PAYLOAD: one log entry's tag + text + NULs at most.

```fides
pub const NDK_LOG_MAX_PAYLOAD: i64 = 4068;
```

*Defined at line 108*

---

## ALOOPER_PREPARE_ALLOW_NON_CALLBACKS:

```fides
pub const ALOOPER_PREPARE_ALLOW_NON_CALLBACKS: i64 = 1;
```

*Defined at line 111*

---

## ALOOPER_POLL_WAKE:

```fides
pub const ALOOPER_POLL_WAKE: i64 = -1;
```

*Defined at line 112*

---

## ALOOPER_POLL_CALLBACK:

```fides
pub const ALOOPER_POLL_CALLBACK: i64 = -2;
```

*Defined at line 113*

---

## ALOOPER_POLL_TIMEOUT:

```fides
pub const ALOOPER_POLL_TIMEOUT: i64 = -3;
```

*Defined at line 114*

---

## ALOOPER_POLL_ERROR:

```fides
pub const ALOOPER_POLL_ERROR: i64 = -4;
```

*Defined at line 115*

---

## ALOOPER_EVENT_INPUT:

```fides
pub const ALOOPER_EVENT_INPUT: i64 = 1;
```

*Defined at line 116*

---

## ALOOPER_EVENT_OUTPUT:

```fides
pub const ALOOPER_EVENT_OUTPUT: i64 = 2;
```

*Defined at line 117*

---

## ALOOPER_EVENT_ERROR:

```fides
pub const ALOOPER_EVENT_ERROR: i64 = 4;
```

*Defined at line 118*

---

## ALOOPER_EVENT_HANGUP:

```fides
pub const ALOOPER_EVENT_HANGUP: i64 = 8;
```

*Defined at line 119*

---

## LOOPER_ID_MAIN:

```fides
pub const LOOPER_ID_MAIN: i64 = 1;
```

*Defined at line 122*

---

## LOOPER_ID_INPUT:

```fides
pub const LOOPER_ID_INPUT: i64 = 2;
```

*Defined at line 123*

---

## LOOPER_ID_USER:

```fides
pub const LOOPER_ID_USER: i64 = 3;
```

*Defined at line 124*

---

## APP_CMD_INPUT_CHANGED:

```fides
pub const APP_CMD_INPUT_CHANGED: i64 = 0;
```

*Defined at line 125*

---

## APP_CMD_INIT_WINDOW:

```fides
pub const APP_CMD_INIT_WINDOW: i64 = 1;
```

*Defined at line 126*

---

## APP_CMD_TERM_WINDOW:

```fides
pub const APP_CMD_TERM_WINDOW: i64 = 2;
```

*Defined at line 127*

---

## APP_CMD_WINDOW_RESIZED:

```fides
pub const APP_CMD_WINDOW_RESIZED: i64 = 3;
```

*Defined at line 128*

---

## APP_CMD_WINDOW_REDRAW_NEEDED:

```fides
pub const APP_CMD_WINDOW_REDRAW_NEEDED: i64 = 4;
```

*Defined at line 129*

---

## APP_CMD_CONTENT_RECT_CHANGED:

```fides
pub const APP_CMD_CONTENT_RECT_CHANGED: i64 = 5;
```

*Defined at line 130*

---

## APP_CMD_GAINED_FOCUS:

```fides
pub const APP_CMD_GAINED_FOCUS: i64 = 6;
```

*Defined at line 131*

---

## APP_CMD_LOST_FOCUS:

```fides
pub const APP_CMD_LOST_FOCUS: i64 = 7;
```

*Defined at line 132*

---

## APP_CMD_CONFIG_CHANGED:

```fides
pub const APP_CMD_CONFIG_CHANGED: i64 = 8;
```

*Defined at line 133*

---

## APP_CMD_LOW_MEMORY:

```fides
pub const APP_CMD_LOW_MEMORY: i64 = 9;
```

*Defined at line 134*

---

## APP_CMD_START:

```fides
pub const APP_CMD_START: i64 = 10;
```

*Defined at line 135*

---

## APP_CMD_RESUME:

```fides
pub const APP_CMD_RESUME: i64 = 11;
```

*Defined at line 136*

---

## APP_CMD_SAVE_STATE:

```fides
pub const APP_CMD_SAVE_STATE: i64 = 12;
```

*Defined at line 137*

---

## APP_CMD_PAUSE:

```fides
pub const APP_CMD_PAUSE: i64 = 13;
```

*Defined at line 138*

---

## APP_CMD_STOP:

```fides
pub const APP_CMD_STOP: i64 = 14;
```

*Defined at line 139*

---

## APP_CMD_DESTROY:

```fides
pub const APP_CMD_DESTROY: i64 = 15;
```

*Defined at line 140*

---

## AINPUT_EVENT_TYPE_KEY:

```fides
pub const AINPUT_EVENT_TYPE_KEY: i64 = 1;
```

*Defined at line 143*

---

## AINPUT_EVENT_TYPE_MOTION:

```fides
pub const AINPUT_EVENT_TYPE_MOTION: i64 = 2;
```

*Defined at line 144*

---

## AINPUT_SOURCE_KEYBOARD:

```fides
pub const AINPUT_SOURCE_KEYBOARD: i64 = 0x00000101;
```

*Defined at line 145*

---

## AINPUT_SOURCE_DPAD:

```fides
pub const AINPUT_SOURCE_DPAD: i64 = 0x00000201;
```

*Defined at line 146*

---

## AINPUT_SOURCE_GAMEPAD:

```fides
pub const AINPUT_SOURCE_GAMEPAD: i64 = 0x00000401;
```

*Defined at line 147*

---

## AINPUT_SOURCE_TOUCHSCREEN:

```fides
pub const AINPUT_SOURCE_TOUCHSCREEN: i64 = 0x00001002;
```

*Defined at line 148*

---

## AINPUT_SOURCE_MOUSE:

```fides
pub const AINPUT_SOURCE_MOUSE: i64 = 0x00002002;
```

*Defined at line 149*

---

## AINPUT_SOURCE_STYLUS:

```fides
pub const AINPUT_SOURCE_STYLUS: i64 = 0x00004002;
```

*Defined at line 150*

---

## AINPUT_SOURCE_JOYSTICK:

```fides
pub const AINPUT_SOURCE_JOYSTICK: i64 = 0x01000010;
```

*Defined at line 151*

---

## AMOTION_EVENT_ACTION_MASK:

```fides
pub const AMOTION_EVENT_ACTION_MASK: i64 = 0xff;
```

*Defined at line 152*

---

## AMOTION_EVENT_ACTION_POINTER_INDEX_MASK:

```fides
pub const AMOTION_EVENT_ACTION_POINTER_INDEX_MASK: i64 = 0xff00;
```

*Defined at line 153*

---

## AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT:

```fides
pub const AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT: i64 = 8;
```

*Defined at line 154*

---

## AMOTION_EVENT_ACTION_DOWN:

```fides
pub const AMOTION_EVENT_ACTION_DOWN: i64 = 0;
```

*Defined at line 155*

---

## AMOTION_EVENT_ACTION_UP:

```fides
pub const AMOTION_EVENT_ACTION_UP: i64 = 1;
```

*Defined at line 156*

---

## AMOTION_EVENT_ACTION_MOVE:

```fides
pub const AMOTION_EVENT_ACTION_MOVE: i64 = 2;
```

*Defined at line 157*

---

## AMOTION_EVENT_ACTION_CANCEL:

```fides
pub const AMOTION_EVENT_ACTION_CANCEL: i64 = 3;
```

*Defined at line 158*

---

## AMOTION_EVENT_ACTION_OUTSIDE:

```fides
pub const AMOTION_EVENT_ACTION_OUTSIDE: i64 = 4;
```

*Defined at line 159*

---

## AMOTION_EVENT_ACTION_POINTER_DOWN:

```fides
pub const AMOTION_EVENT_ACTION_POINTER_DOWN: i64 = 5;
```

*Defined at line 160*

---

## AMOTION_EVENT_ACTION_POINTER_UP:

```fides
pub const AMOTION_EVENT_ACTION_POINTER_UP: i64 = 6;
```

*Defined at line 161*

---

## AMOTION_EVENT_ACTION_HOVER_MOVE:

```fides
pub const AMOTION_EVENT_ACTION_HOVER_MOVE: i64 = 7;
```

*Defined at line 162*

---

## AMOTION_EVENT_ACTION_SCROLL:

```fides
pub const AMOTION_EVENT_ACTION_SCROLL: i64 = 8;
```

*Defined at line 163*

---

## AKEY_EVENT_ACTION_DOWN:

```fides
pub const AKEY_EVENT_ACTION_DOWN: i64 = 0;
```

*Defined at line 164*

---

## AKEY_EVENT_ACTION_UP:

```fides
pub const AKEY_EVENT_ACTION_UP: i64 = 1;
```

*Defined at line 165*

---

## AKEY_EVENT_ACTION_MULTIPLE:

```fides
pub const AKEY_EVENT_ACTION_MULTIPLE: i64 = 2;
```

*Defined at line 166*

---

## AMETA_ALT_ON:

```fides
pub const AMETA_ALT_ON: i64 = 0x02;
```

*Defined at line 167*

---

## AMETA_SHIFT_ON:

```fides
pub const AMETA_SHIFT_ON: i64 = 0x01;
```

*Defined at line 168*

---

## AMETA_CTRL_ON:

```fides
pub const AMETA_CTRL_ON: i64 = 0x1000;
```

*Defined at line 169*

---

## AKEYCODE_BACK:

```fides
pub const AKEYCODE_BACK: i64 = 4;
```

*Defined at line 170*

---

## AKEYCODE_0:

```fides
pub const AKEYCODE_0: i64 = 7;
```

*Defined at line 171*

---

## AKEYCODE_DPAD_UP:

```fides
pub const AKEYCODE_DPAD_UP: i64 = 19;
```

*Defined at line 172*

---

## AKEYCODE_DPAD_DOWN:

```fides
pub const AKEYCODE_DPAD_DOWN: i64 = 20;
```

*Defined at line 173*

---

## AKEYCODE_DPAD_LEFT:

```fides
pub const AKEYCODE_DPAD_LEFT: i64 = 21;
```

*Defined at line 174*

---

## AKEYCODE_DPAD_RIGHT:

```fides
pub const AKEYCODE_DPAD_RIGHT: i64 = 22;
```

*Defined at line 175*

---

## AKEYCODE_DPAD_CENTER:

```fides
pub const AKEYCODE_DPAD_CENTER: i64 = 23;
```

*Defined at line 176*

---

## AKEYCODE_VOLUME_UP:

```fides
pub const AKEYCODE_VOLUME_UP: i64 = 24;
```

*Defined at line 177*

---

## AKEYCODE_VOLUME_DOWN:

```fides
pub const AKEYCODE_VOLUME_DOWN: i64 = 25;
```

*Defined at line 178*

---

## AKEYCODE_A:

```fides
pub const AKEYCODE_A: i64 = 29;
```

*Defined at line 179*

---

## AKEYCODE_SPACE:

```fides
pub const AKEYCODE_SPACE: i64 = 62;
```

*Defined at line 180*

---

## AKEYCODE_ENTER:

```fides
pub const AKEYCODE_ENTER: i64 = 66;
```

*Defined at line 181*

---

## AKEYCODE_DEL:

```fides
pub const AKEYCODE_DEL: i64 = 67;
```

*Defined at line 182*

---

## AKEYCODE_MENU:

```fides
pub const AKEYCODE_MENU: i64 = 82;
```

*Defined at line 183*

---

## AKEYCODE_ESCAPE:

```fides
pub const AKEYCODE_ESCAPE: i64 = 111;
```

*Defined at line 184*

---

## AKEYCODE_BUTTON_A:

```fides
pub const AKEYCODE_BUTTON_A: i64 = 96;
```

*Defined at line 185*

---

## AKEYCODE_BUTTON_B:

```fides
pub const AKEYCODE_BUTTON_B: i64 = 97;
```

*Defined at line 186*

---

## WINDOW_FORMAT_RGBA_8888:

```fides
pub const WINDOW_FORMAT_RGBA_8888: i64 = 1;
```

*Defined at line 189*

---

## WINDOW_FORMAT_RGBX_8888:

```fides
pub const WINDOW_FORMAT_RGBX_8888: i64 = 2;
```

*Defined at line 190*

---

## WINDOW_FORMAT_RGB_565:

```fides
pub const WINDOW_FORMAT_RGB_565: i64 = 4;
```

*Defined at line 191*

---

## AASSET_MODE_UNKNOWN:

```fides
pub const AASSET_MODE_UNKNOWN: i64 = 0;
```

*Defined at line 194*

---

## AASSET_MODE_RANDOM:

```fides
pub const AASSET_MODE_RANDOM: i64 = 1;
```

*Defined at line 195*

---

## AASSET_MODE_STREAMING:

```fides
pub const AASSET_MODE_STREAMING: i64 = 2;
```

*Defined at line 196*

---

## AASSET_MODE_BUFFER:

```fides
pub const AASSET_MODE_BUFFER: i64 = 3;
```

*Defined at line 197*

---

## NDK_ACTIVITY_CALLBACKS:

```fides
pub const NDK_ACTIVITY_CALLBACKS: i64 = 0;
```

*Defined at line 202*

---

## NDK_ACTIVITY_VM:

```fides
pub const NDK_ACTIVITY_VM: i64 = 8;
```

*Defined at line 203*

---

## NDK_ACTIVITY_ENV:

```fides
pub const NDK_ACTIVITY_ENV: i64 = 16;
```

*Defined at line 204*

---

## NDK_ACTIVITY_CLAZZ:

```fides
pub const NDK_ACTIVITY_CLAZZ: i64 = 24;
```

*Defined at line 205*

---

## NDK_ACTIVITY_INTERNAL_PATH:

```fides
pub const NDK_ACTIVITY_INTERNAL_PATH: i64 = 32;
```

*Defined at line 206*

---

## NDK_ACTIVITY_EXTERNAL_PATH:

```fides
pub const NDK_ACTIVITY_EXTERNAL_PATH: i64 = 40;
```

*Defined at line 207*

---

## NDK_ACTIVITY_SDK_VERSION:

```fides
pub const NDK_ACTIVITY_SDK_VERSION: i64 = 48;  // int32_t
```

*Defined at line 208*

---

## NDK_ACTIVITY_INSTANCE:

```fides
pub const NDK_ACTIVITY_INSTANCE: i64 = 56;
```

*Defined at line 209*

---

## NDK_ACTIVITY_ASSET_MANAGER:

```fides
pub const NDK_ACTIVITY_ASSET_MANAGER: i64 = 64;
```

*Defined at line 210*

---

## NDK_ACTIVITY_OBB_PATH:

```fides
pub const NDK_ACTIVITY_OBB_PATH: i64 = 72;
```

*Defined at line 211*

---

## ndk_callback_names

The callback slots of ANativeActivityCallbacks, in header order.

```fides
pub fn ndk_callback_names() -> Vec<str> {
```

**Returns:** `Vec<str>`

*Defined at line 214*

---

## ndk_callback_offset

Byte offset of a callback in ANativeActivityCallbacks, -1 if unknown.

```fides
pub fn ndk_callback_offset(name: str) -> i64 {
```

**Parameters:**

- `name`: `str`

**Returns:** `i64`

*Defined at line 236*

---

## ndk_callback_cmd

The glue command an ANativeActivity callback turns into (-1: none;
focus and window callbacks depend on their argument).

```fides
pub fn ndk_callback_cmd(name: str, arg: i64) -> i64 {
```

**Parameters:**

- `name`: `str`
- `arg`: `i64`

**Returns:** `i64`

*Defined at line 250*

---

## ndk_cmd_name

Name of a glue command.

```fides
pub fn ndk_cmd_name(cmd: i64) -> str {
```

**Parameters:**

- `cmd`: `i64`

**Returns:** `str`

*Defined at line 304*

---

## NdkWindowBuffer

ANativeWindow_Buffer: width, height, stride (pixels), format, bits.

```fides
pub struct NdkWindowBuffer {
```

**Fields:**

| Name | Type |
|------|------|
| `width` | `i64` |
| `height` | `i64` |
| `stride` | `i64` |
| `format` | `i64` |
| `bits` | `i64` |

*Defined at line 313*

---

## ndk_read_window_buffer

Read an ANativeWindow_Buffer at `addr` (after ANativeWindow_lock).

```fides
pub fn ndk_read_window_buffer(addr: i64) -> NdkWindowBuffer {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `NdkWindowBuffer`

*Defined at line 342*

---

## ndk_activity_sdk_version

The SDK version an ANativeActivity reports.

```fides
pub fn ndk_activity_sdk_version(activity: i64) -> i64 {
```

**Parameters:**

- `activity`: `i64`

**Returns:** `i64`

*Defined at line 349*

---

## ndk_activity_asset_manager

The AAssetManager* of an ANativeActivity.

```fides
pub fn ndk_activity_asset_manager(activity: i64) -> i64 {
```

**Parameters:**

- `activity`: `i64`

**Returns:** `i64`

*Defined at line 354*

---

## ndk_format_bpp

Bytes per pixel of a window format (0 if unknown).

```fides
pub fn ndk_format_bpp(format: i64) -> i64 {
```

**Parameters:**

- `format`: `i64`

**Returns:** `i64`

*Defined at line 361*

---

## ndk_pack_pixel

One pixel as the word the buffer holds (little-endian memory: R is
the first byte of RGBA_8888; RGB_565 is R in the top 5 bits).

```fides
pub fn ndk_pack_pixel(format: i64, r: i64, g: i64, b: i64, a: i64) -> i64 {
```

**Parameters:**

- `format`: `i64`
- `r`: `i64`
- `g`: `i64`
- `b`: `i64`
- `a`: `i64`

**Returns:** `i64`

*Defined at line 373*

---

## ndk_fill_rect

Fill a rectangle of a pixel buffer held as words (one per pixel,
`stride` per row), clipped to width x height.

```fides
pub fn ndk_fill_rect(pixels: &mut Vec<i64>, width: i64, height: i64, stride: i64, x: i64, y: i64, w: i64, h: i64, px: i64) {
```

**Parameters:**

- `pixels`: `&mut Vec<i64>`
- `width`: `i64`
- `height`: `i64`
- `stride`: `i64`
- `x`: `i64`
- `y`: `i64`
- `w`: `i64`
- `h`: `i64`
- `px`: `i64`

*Defined at line 388*

---

## NdkInput

An input event as the accessors report it. `kind` is 0 (no event),
AINPUT_EVENT_TYPE_KEY or AINPUT_EVENT_TYPE_MOTION.

```fides
pub struct NdkInput {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `source` | `i64` |
| `action` | `i64,  // the raw action (motion: with pointer index bits)` |
| `key_code` | `i64` |
| `meta` | `i64` |
| `ids` | `Vec<i64>,  // motion: pointer ids by index` |
| `xs` | `Vec<f64>` |
| `ys` | `Vec<f64>` |

*Defined at line 423*

---

## ndk_no_input

```fides
pub fn ndk_no_input() -> NdkInput {
```

**Returns:** `NdkInput`

*Defined at line 434*

---

## ndk_key_input

```fides
pub fn ndk_key_input(action: i64, code: i64, meta: i64) -> NdkInput {
```

**Parameters:**

- `action`: `i64`
- `code`: `i64`
- `meta`: `i64`

**Returns:** `NdkInput`

*Defined at line 440*

---

## ndk_motion_action

The masked action of a motion event (AMOTION_EVENT_ACTION_*).

```fides
pub fn ndk_motion_action(action: i64) -> i64 {
```

**Parameters:**

- `action`: `i64`

**Returns:** `i64`

*Defined at line 451*

---

## ndk_motion_pointer_index

The pointer index of a POINTER_DOWN / POINTER_UP action.

```fides
pub fn ndk_motion_pointer_index(action: i64) -> i64 {
```

**Parameters:**

- `action`: `i64`

**Returns:** `i64`

*Defined at line 456*

---

## ndk_motion_make_action

The raw action for a masked action and a pointer index.

```fides
pub fn ndk_motion_make_action(masked: i64, index: i64) -> i64 {
```

**Parameters:**

- `masked`: `i64`
- `index`: `i64`

**Returns:** `i64`

*Defined at line 461*

---

## ndk_source_is

Does `source` include class bits `cls` (e.g. AINPUT_SOURCE_TOUCHSCREEN)?

```fides
pub fn ndk_source_is(source: i64, cls: i64) -> bool {
```

**Parameters:**

- `source`: `i64`
- `cls`: `i64`

**Returns:** `bool`

*Defined at line 466*

---

## NdkPointers

Active touch pointers by id.

```fides
pub struct NdkPointers {
```

**Fields:**

| Name | Type |
|------|------|
| `ids` | `Vec<i64>` |
| `xs` | `Vec<f64>` |
| `ys` | `Vec<f64>` |
| `downs` | `i64,  // pointers that went down so far` |
| `ups` | `i64` |

*Defined at line 471*

---

## ndk_pointers_new

```fides
pub fn ndk_pointers_new() -> NdkPointers {
```

**Returns:** `NdkPointers`

*Defined at line 479*

---

## ndk_pointers_update

Update the tracker with a motion event (the AMotionEvent semantics:
DOWN/POINTER_DOWN add the indexed pointer, MOVE updates all,
UP/POINTER_UP remove the indexed pointer, CANCEL removes all).

```fides
pub fn ndk_pointers_update(p: &mut NdkPointers, e: &NdkInput) {
```

**Parameters:**

- `p`: `&mut NdkPointers`
- `e`: `&NdkInput`

*Defined at line 523*

---

## NdkApp

```fides
pub struct NdkApp {
```

**Fields:**

| Name | Type |
|------|------|
| `started` | `bool` |
| `resumed` | `bool` |
| `has_window` | `bool` |
| `focused` | `bool` |
| `destroy_requested` | `bool` |
| `width` | `i64` |
| `height` | `i64` |
| `frames` | `i64` |
| `state_saves` | `i64` |
| `low_memory` | `i64` |
| `back_pressed` | `i64` |
| `pointers` | `NdkPointers` |
| `log` | `Vec<str>` |

*Defined at line 555*

---

## ndk_app_new

```fides
pub fn ndk_app_new() -> NdkApp {
```

**Returns:** `NdkApp`

*Defined at line 571*

---

## ndk_app_animating

Should the loop draw frames (and so poll without blocking)?

```fides
pub fn ndk_app_animating(app: &NdkApp) -> bool {
```

**Parameters:**

- `app`: `&NdkApp`

**Returns:** `bool`

*Defined at line 578*

---

## ndk_poll_timeout

The timeout for ALooper_pollOnce: 0 while animating, -1 (block) else.

```fides
pub fn ndk_poll_timeout(app: &NdkApp) -> i64 {
```

**Parameters:**

- `app`: `&NdkApp`

**Returns:** `i64`

*Defined at line 583*

---

## ndk_app_handle_cmd

Apply a glue command. `w`/`h` are the window size for INIT_WINDOW and
WINDOW_RESIZED (ANativeWindow_getWidth/Height on a device).

```fides
pub fn ndk_app_handle_cmd(app: &mut NdkApp, cmd: i64, w: i64, h: i64) {
```

**Parameters:**

- `app`: `&mut NdkApp`
- `cmd`: `i64`
- `w`: `i64`
- `h`: `i64`

*Defined at line 592*

---

## ndk_app_handle_input

Handle an input event: true when consumed (passed to
AInputQueue_finishEvent as `handled`). BACK is consumed and counted.

```fides
pub fn ndk_app_handle_input(app: &mut NdkApp, e: &NdkInput) -> bool {
```

**Parameters:**

- `app`: `&mut NdkApp`
- `e`: `&NdkInput`

**Returns:** `bool`

*Defined at line 626*

---

## NdkLoopSeam

What the loop needs from the platform (see the header).

```fides
pub struct NdkLoopSeam {
```

**Fields:**

| Name | Type |
|------|------|
| `poll` | `fn(i64) -> i64,  // timeout ms -> ident or ALOOPER_POLL_*` |
| `read_cmd` | `fn() -> i64,  // the next glue command` |
| `window_w` | `fn() -> i64` |
| `window_h` | `fn() -> i64` |
| `read_input` | `fn(&mut NdkInput) -> void,  // fills the next event; kind 0: queue empty` |
| `draw` | `fn(i64, i64, i64) -> void,  // frame number, width, height` |

*Defined at line 644*

---

## ndk_run_loop

Run the loop until DESTROY (or `max_polls` polls when >= 0): what
android_main does with the glue.

```fides
pub fn ndk_run_loop(app: &mut NdkApp, seam: NdkLoopSeam, max_polls: i64) -> i64 {
```

**Parameters:**

- `app`: `&mut NdkApp`
- `seam`: `NdkLoopSeam`
- `max_polls`: `i64`

**Returns:** `i64`

*Defined at line 655*

---

## ndk_log_letter

The priority letter logcat prints (V D I W E F; '?' otherwise).

```fides
pub fn ndk_log_letter(prio: i64) -> str {
```

**Parameters:**

- `prio`: `i64`

**Returns:** `str`

*Defined at line 691*

---

## ndk_log_line

A message as `logcat -v brief` shows it: "I/tag: text".

```fides
pub fn ndk_log_line(prio: i64, tag: str, text: str) -> str {
```

**Parameters:**

- `prio`: `i64`
- `tag`: `str`
- `text`: `str`

**Returns:** `str`

*Defined at line 714*

---

## ndk_log_chunks

Split a message so each entry (prio byte, tag, NUL, text, NUL) fits
the logger payload; cuts fall on a newline when there is one in the
chunk, and never inside a UTF-8 sequence.

```fides
pub fn ndk_log_chunks(tag: str, text: str) -> Vec<str> {
```

**Parameters:**

- `tag`: `str`
- `text`: `str`

**Returns:** `Vec<str>`

*Defined at line 721*

---

## ndk_log

Write a message to logcat (device builds), split as ndk_log_chunks.

```fides
pub fn ndk_log(prio: i64, tag: str, text: str) {
```

**Parameters:**

- `prio`: `i64`
- `tag`: `str`
- `text`: `str`

*Defined at line 753*

---

## ndk_asset_read_all

Read a whole asset through AAssetManager (device builds): the bytes,
empty when it does not open.

```fides
pub fn ndk_asset_read_all(mgr: i64, name: str) -> Vec<i64> {
```

**Parameters:**

- `mgr`: `i64`
- `name`: `str`

**Returns:** `Vec<i64>`

*Defined at line 767*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
