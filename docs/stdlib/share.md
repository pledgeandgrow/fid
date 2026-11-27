[All modules](index.md)

# share -- Fides Documentation

## Contents

- [SHARE_TEXT:](#SHARE_TEXT:) (const)
- [SHARE_URL:](#SHARE_URL:) (const)
- [SHARE_FILE:](#SHARE_FILE:) (const)
- [ANDROID_ACTION_SEND:](#ANDROID_ACTION_SEND:) (const)
- [ANDROID_ACTION_SEND_MULTIPLE:](#ANDROID_ACTION_SEND_MULTIPLE:) (const)
- [ANDROID_ACTION_CHOOSER:](#ANDROID_ACTION_CHOOSER:) (const)
- [ANDROID_EXTRA_TEXT:](#ANDROID_EXTRA_TEXT:) (const)
- [ANDROID_EXTRA_SUBJECT:](#ANDROID_EXTRA_SUBJECT:) (const)
- [ANDROID_EXTRA_TITLE:](#ANDROID_EXTRA_TITLE:) (const)
- [ANDROID_EXTRA_STREAM:](#ANDROID_EXTRA_STREAM:) (const)
- [ANDROID_EXTRA_INTENT:](#ANDROID_EXTRA_INTENT:) (const)
- [ANDROID_FLAG_GRANT_READ_URI_PERMISSION:](#ANDROID_FLAG_GRANT_READ_URI_PERMISSION:) (const)
- [SHARE_TEXT_WARN_BYTES:](#SHARE_TEXT_WARN_BYTES:) (const)
- [ShareItem](#ShareItem) (struct)
- [share_text](#share_text) (function)
- [share_url](#share_url) (function)
- [share_file](#share_file) (function)
- [ShareRequest](#ShareRequest) (struct)
- [share_request_new](#share_request_new) (function)
- [share_add](#share_add) (function)
- [share_common_mime](#share_common_mime) (function)
- [ShareRoot](#ShareRoot) (struct)
- [FileProvider](#FileProvider) (struct)
- [share_file_provider](#share_file_provider) (function)
- [share_provider_root](#share_provider_root) (function)
- [share_content_uri](#share_content_uri) (function)
- [share_file_paths_xml](#share_file_paths_xml) (function)
- [ShareExtra](#ShareExtra) (struct)
- [ShareIntent](#ShareIntent) (struct)
- [share_android_intent](#share_android_intent) (function)
- [share_android_chooser](#share_android_chooser) (function)
- [share_android_am_command](#share_android_am_command) (function)
- [share_android_received](#share_android_received) (function)
- [IOS_ACTIVITY_POST_TO_FACEBOOK:](#IOS_ACTIVITY_POST_TO_FACEBOOK:) (const)
- [IOS_ACTIVITY_MESSAGE:](#IOS_ACTIVITY_MESSAGE:) (const)
- [IOS_ACTIVITY_MAIL:](#IOS_ACTIVITY_MAIL:) (const)
- [IOS_ACTIVITY_PRINT:](#IOS_ACTIVITY_PRINT:) (const)
- [IOS_ACTIVITY_COPY:](#IOS_ACTIVITY_COPY:) (const)
- [IOS_ACTIVITY_AIRDROP:](#IOS_ACTIVITY_AIRDROP:) (const)
- [IOS_ACTIVITY_SAVE_TO_CAMERA_ROLL:](#IOS_ACTIVITY_SAVE_TO_CAMERA_ROLL:) (const)
- [ShareIosItem](#ShareIosItem) (struct)
- [share_ios_items](#share_ios_items) (function)
- [share_ios_check](#share_ios_check) (function)
- [ShareCall](#ShareCall) (struct)
- [share_android_calls](#share_android_calls) (function)
- [share_ios_calls](#share_ios_calls) (function)

## SHARE_TEXT:

```fides
pub const SHARE_TEXT: i64 = 0;
```

*Defined at line 33*

---

## SHARE_URL:

```fides
pub const SHARE_URL: i64 = 1;
```

*Defined at line 34*

---

## SHARE_FILE:

```fides
pub const SHARE_FILE: i64 = 2;
```

*Defined at line 35*

---

## ANDROID_ACTION_SEND:

```fides
pub const ANDROID_ACTION_SEND: str = "android.intent.action.SEND";
```

*Defined at line 37*

---

## ANDROID_ACTION_SEND_MULTIPLE:

```fides
pub const ANDROID_ACTION_SEND_MULTIPLE: str = "android.intent.action.SEND_MULTIPLE";
```

*Defined at line 38*

---

## ANDROID_ACTION_CHOOSER:

```fides
pub const ANDROID_ACTION_CHOOSER: str = "android.intent.action.CHOOSER";
```

*Defined at line 39*

---

## ANDROID_EXTRA_TEXT:

```fides
pub const ANDROID_EXTRA_TEXT: str = "android.intent.extra.TEXT";
```

*Defined at line 40*

---

## ANDROID_EXTRA_SUBJECT:

```fides
pub const ANDROID_EXTRA_SUBJECT: str = "android.intent.extra.SUBJECT";
```

*Defined at line 41*

---

## ANDROID_EXTRA_TITLE:

```fides
pub const ANDROID_EXTRA_TITLE: str = "android.intent.extra.TITLE";
```

*Defined at line 42*

---

## ANDROID_EXTRA_STREAM:

```fides
pub const ANDROID_EXTRA_STREAM: str = "android.intent.extra.STREAM";
```

*Defined at line 43*

---

## ANDROID_EXTRA_INTENT:

```fides
pub const ANDROID_EXTRA_INTENT: str = "android.intent.extra.INTENT";
```

*Defined at line 44*

---

## ANDROID_FLAG_GRANT_READ_URI_PERMISSION:

```fides
pub const ANDROID_FLAG_GRANT_READ_URI_PERMISSION: i64 = 1;
```

*Defined at line 45*

---

## SHARE_TEXT_WARN_BYTES:

Text above this many bytes risks the 1 MB binder transaction limit.

```fides
pub const SHARE_TEXT_WARN_BYTES: i64 = 100000;
```

*Defined at line 47*

---

## ShareItem

```fides
pub struct ShareItem {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `value` | `str,  // the text, the URL, or the file path` |
| `mime` | `str` |

*Defined at line 49*

---

## share_text

```fides
pub fn share_text(t: str) -> ShareItem {
```

**Parameters:**

- `t`: `str`

**Returns:** `ShareItem`

*Defined at line 55*

---

## share_url

```fides
pub fn share_url(u: str) -> ShareItem {
```

**Parameters:**

- `u`: `str`

**Returns:** `ShareItem`

*Defined at line 60*

---

## share_file

A file; `mime` "" takes the type from the extension.

```fides
pub fn share_file(path: str, mime_type: str) -> ShareItem {
```

**Parameters:**

- `path`: `str`
- `mime_type`: `str`

**Returns:** `ShareItem`

*Defined at line 67*

---

## ShareRequest

```fides
pub struct ShareRequest {
```

**Fields:**

| Name | Type |
|------|------|
| `items` | `Vec<ShareItem>` |
| `subject` | `str` |
| `title` | `str,  // chooser title (Android) / unused on iOS` |
| `excluded` | `Vec<str>,  // iOS UIActivityType strings` |

*Defined at line 78*

---

## share_request_new

```fides
pub fn share_request_new() -> ShareRequest {
```

**Returns:** `ShareRequest`

*Defined at line 85*

---

## share_add

```fides
pub fn share_add(r: &mut ShareRequest, item: ShareItem) {
```

**Parameters:**

- `r`: `&mut ShareRequest`
- `item`: `ShareItem`

*Defined at line 91*

---

## share_common_mime

The MIME type that covers all items: the common type, "<top>/*" when
the top-level types agree, else "*/*".

```fides
pub fn share_common_mime(items: &Vec<ShareItem>) -> str {
```

**Parameters:**

- `items`: `&Vec<ShareItem>`

**Returns:** `str`

*Defined at line 97*

---

## ShareRoot

One <paths> entry: kind ("files-path", "cache-path",
"external-files-path", ...), the URI segment `name`, the directory
`root` it stands for on this device, and an optional sub `path`.

```fides
pub struct ShareRoot {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `str` |
| `name` | `str` |
| `root` | `str` |
| `sub` | `str` |

*Defined at line 124*

---

## FileProvider

```fides
pub struct FileProvider {
```

**Fields:**

| Name | Type |
|------|------|
| `authority` | `str,  // e.g. "com.example.app.fileprovider"` |
| `roots` | `Vec<ShareRoot>` |

*Defined at line 131*

---

## share_file_provider

```fides
pub fn share_file_provider(authority: str) -> FileProvider {
```

**Parameters:**

- `authority`: `str`

**Returns:** `FileProvider`

*Defined at line 136*

---

## share_provider_root

```fides
pub fn share_provider_root(fp: &mut FileProvider, kind: str, name: str, root: str, sub: str) {
```

**Parameters:**

- `fp`: `&mut FileProvider`
- `kind`: `str`
- `name`: `str`
- `root`: `str`
- `sub`: `str`

*Defined at line 142*

---

## share_content_uri

The content:// URI of a file ("" when no root contains it or the path
climbs out with "..").

```fides
pub fn share_content_uri(fp: &FileProvider, path: str) -> str {
```

**Parameters:**

- `fp`: `&FileProvider`
- `path`: `str`

**Returns:** `str`

*Defined at line 160*

---

## share_file_paths_xml

res/xml/file_paths.xml for the provider.

```fides
pub fn share_file_paths_xml(fp: &FileProvider) -> str {
```

**Parameters:**

- `fp`: `&FileProvider`

**Returns:** `str`

*Defined at line 197*

---

## ShareExtra

An extra: kind "string", "uri", "uri-list", "intent".

```fides
pub struct ShareExtra {
```

**Fields:**

| Name | Type |
|------|------|
| `key` | `str` |
| `kind` | `str` |
| `values` | `Vec<str>` |

*Defined at line 215*

---

## ShareIntent

```fides
pub struct ShareIntent {
```

**Fields:**

| Name | Type |
|------|------|
| `ok` | `bool` |
| `error` | `str` |
| `action` | `str` |
| `mime` | `str` |
| `extras` | `Vec<ShareExtra>` |
| `flags` | `i64` |
| `clip_uris` | `Vec<str>,  // ClipData items that carry the URI grant` |
| `warnings` | `Vec<str>` |

*Defined at line 221*

---

## share_android_intent

The ACTION_SEND / ACTION_SEND_MULTIPLE intent of a request. Files are
turned into FileProvider URIs.

```fides
pub fn share_android_intent(r: &ShareRequest, fp: &FileProvider) -> ShareIntent {
```

**Parameters:**

- `r`: `&ShareRequest`
- `fp`: `&FileProvider`

**Returns:** `ShareIntent`

*Defined at line 242*

---

## share_android_chooser

Intent.createChooser(intent, title): ACTION_CHOOSER with the target in
EXTRA_INTENT (described here by its action) and the title.

```fides
pub fn share_android_chooser(target: &ShareIntent, title: str) -> ShareIntent {
```

**Parameters:**

- `target`: `&ShareIntent`
- `title`: `str`

**Returns:** `ShareIntent`

*Defined at line 316*

---

## share_android_am_command

The intent as `adb shell am start` arguments (single ACTION_SEND
extras; a URI list as --eua).

```fides
pub fn share_android_am_command(it: &ShareIntent) -> str {
```

**Parameters:**

- `it`: `&ShareIntent`

**Returns:** `str`

*Defined at line 333*

---

## share_android_received

Items of an intent another app sent us (ACTION_SEND or
SEND_MULTIPLE), from its type and extras.

```fides
pub fn share_android_received(action: str, mime_type: str, extras: Vec<ShareExtra>) -> Vec<ShareItem> {
```

**Parameters:**

- `action`: `str`
- `mime_type`: `str`
- `extras`: `Vec<ShareExtra>`

**Returns:** `Vec<ShareItem>`

*Defined at line 367*

---

## IOS_ACTIVITY_POST_TO_FACEBOOK:

```fides
pub const IOS_ACTIVITY_POST_TO_FACEBOOK: str = "com.apple.UIKit.activity.PostToFacebook";
```

*Defined at line 399*

---

## IOS_ACTIVITY_MESSAGE:

```fides
pub const IOS_ACTIVITY_MESSAGE: str = "com.apple.UIKit.activity.Message";
```

*Defined at line 400*

---

## IOS_ACTIVITY_MAIL:

```fides
pub const IOS_ACTIVITY_MAIL: str = "com.apple.UIKit.activity.Mail";
```

*Defined at line 401*

---

## IOS_ACTIVITY_PRINT:

```fides
pub const IOS_ACTIVITY_PRINT: str = "com.apple.UIKit.activity.Print";
```

*Defined at line 402*

---

## IOS_ACTIVITY_COPY:

```fides
pub const IOS_ACTIVITY_COPY: str = "com.apple.UIKit.activity.CopyToPasteboard";
```

*Defined at line 403*

---

## IOS_ACTIVITY_AIRDROP:

```fides
pub const IOS_ACTIVITY_AIRDROP: str = "com.apple.UIKit.activity.AirDrop";
```

*Defined at line 404*

---

## IOS_ACTIVITY_SAVE_TO_CAMERA_ROLL:

```fides
pub const IOS_ACTIVITY_SAVE_TO_CAMERA_ROLL: str = "com.apple.UIKit.activity.SaveToCameraRoll";
```

*Defined at line 405*

---

## ShareIosItem

One activity item: the Objective-C object to make ("NSString" with
the text, "NSURL" with URLWithString:, "NSURL file" with
fileURLWithPath:).

```fides
pub struct ShareIosItem {
```

**Fields:**

| Name | Type |
|------|------|
| `cls` | `str` |
| `ctor` | `str` |
| `value` | `str` |

*Defined at line 410*

---

## share_ios_items

```fides
pub fn share_ios_items(r: &ShareRequest) -> Vec<ShareIosItem> {
```

**Parameters:**

- `r`: `&ShareRequest`

**Returns:** `Vec<ShareIosItem>`

*Defined at line 416*

---

## share_ios_check

Problems presenting the sheet: no items, or an iPad without a source
view for the popover (UIKit raises an exception then).

```fides
pub fn share_ios_check(r: &ShareRequest, is_ipad: bool, has_source_view: bool) -> Vec<str> {
```

**Parameters:**

- `r`: `&ShareRequest`
- `is_ipad`: `bool`
- `has_source_view`: `bool`

**Returns:** `Vec<str>`

*Defined at line 441*

---

## ShareCall

```fides
pub struct ShareCall {
```

**Fields:**

| Name | Type |
|------|------|
| `cls` | `str` |
| `method` | `str` |
| `sig` | `str` |

*Defined at line 454*

---

## share_android_calls

```fides
pub fn share_android_calls() -> Vec<ShareCall> {
```

**Returns:** `Vec<ShareCall>`

*Defined at line 460*

---

## share_ios_calls

```fides
pub fn share_ios_calls() -> Vec<ShareCall> {
```

**Returns:** `Vec<ShareCall>`

*Defined at line 489*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
