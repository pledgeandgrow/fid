[All modules](index.md)

# ros2_msg -- Fides Documentation

## Contents

- [CdrWriter](#CdrWriter) (struct)
- [cdr_writer](#cdr_writer) (function)
- [cdr_writer_raw](#cdr_writer_raw) (function)
- [cdr_align](#cdr_align) (function)
- [cdr_write_u8](#cdr_write_u8) (function)
- [cdr_write_bool](#cdr_write_bool) (function)
- [cdr_write_u16](#cdr_write_u16) (function)
- [cdr_write_u32](#cdr_write_u32) (function)
- [cdr_write_i32](#cdr_write_i32) (function)
- [cdr_write_u64](#cdr_write_u64) (function)
- [cdr_write_f64](#cdr_write_f64) (function)
- [cdr_write_f32](#cdr_write_f32) (function)
- [cdr_write_string](#cdr_write_string) (function)
- [cdr_write_f64_array](#cdr_write_f64_array) (function)
- [cdr_write_f32_seq](#cdr_write_f32_seq) (function)
- [cdr_write_u8_seq](#cdr_write_u8_seq) (function)
- [CdrReader](#CdrReader) (struct)
- [cdr_reader](#cdr_reader) (function)
- [cdr_reader_raw](#cdr_reader_raw) (function)
- [cdr_read_u8](#cdr_read_u8) (function)
- [cdr_read_bool](#cdr_read_bool) (function)
- [cdr_read_u16](#cdr_read_u16) (function)
- [cdr_read_u32](#cdr_read_u32) (function)
- [cdr_read_i32](#cdr_read_i32) (function)
- [cdr_read_u64](#cdr_read_u64) (function)
- [cdr_read_f64](#cdr_read_f64) (function)
- [cdr_read_f32](#cdr_read_f32) (function)
- [cdr_read_string](#cdr_read_string) (function)
- [cdr_read_f64_array](#cdr_read_f64_array) (function)
- [cdr_read_f32_seq](#cdr_read_f32_seq) (function)
- [cdr_read_u8_seq](#cdr_read_u8_seq) (function)
- [cdr_f32_bits](#cdr_f32_bits) (function)
- [cdr_f32_from_bits](#cdr_f32_from_bits) (function)
- [RosTime](#RosTime) (struct)
- [RosHeader](#RosHeader) (struct)
- [RosVector3](#RosVector3) (struct)
- [RosQuaternion](#RosQuaternion) (struct)
- [RosPose](#RosPose) (struct)
- [RosTwist](#RosTwist) (struct)
- [RosTransform](#RosTransform) (struct)
- [RosTransformStamped](#RosTransformStamped) (struct)
- [RosPoseStamped](#RosPoseStamped) (struct)
- [RosPoseWithCov](#RosPoseWithCov) (struct)
- [RosTwistWithCov](#RosTwistWithCov) (struct)
- [RosImu](#RosImu) (struct)
- [RosLaserScan](#RosLaserScan) (struct)
- [ROS_PF_INT8:](#ROS_PF_INT8:) (const)
- [ROS_PF_UINT8:](#ROS_PF_UINT8:) (const)
- [ROS_PF_INT16:](#ROS_PF_INT16:) (const)
- [ROS_PF_UINT16:](#ROS_PF_UINT16:) (const)
- [ROS_PF_INT32:](#ROS_PF_INT32:) (const)
- [ROS_PF_UINT32:](#ROS_PF_UINT32:) (const)
- [ROS_PF_FLOAT32:](#ROS_PF_FLOAT32:) (const)
- [ROS_PF_FLOAT64:](#ROS_PF_FLOAT64:) (const)
- [RosPointField](#RosPointField) (struct)
- [RosPointCloud2](#RosPointCloud2) (struct)
- [RosOdometry](#RosOdometry) (struct)
- [ros_time](#ros_time) (function)
- [ros_header](#ros_header) (function)
- [ros_vec3](#ros_vec3) (function)
- [ros_quat](#ros_quat) (function)
- [ros_pose](#ros_pose) (function)
- [ros_twist](#ros_twist) (function)
- [ros_zeros](#ros_zeros) (function)
- [ros_write_time](#ros_write_time) (function)
- [ros_read_time](#ros_read_time) (function)
- [ros_write_header](#ros_write_header) (function)
- [ros_read_header](#ros_read_header) (function)
- [ros_write_vec3](#ros_write_vec3) (function)
- [ros_read_vec3](#ros_read_vec3) (function)
- [ros_write_quat](#ros_write_quat) (function)
- [ros_read_quat](#ros_read_quat) (function)
- [ros_write_pose](#ros_write_pose) (function)
- [ros_read_pose](#ros_read_pose) (function)
- [ros_write_twist](#ros_write_twist) (function)
- [ros_read_twist](#ros_read_twist) (function)
- [ros_write_transform](#ros_write_transform) (function)
- [ros_read_transform](#ros_read_transform) (function)
- [ros_write_transform_stamped](#ros_write_transform_stamped) (function)
- [ros_read_transform_stamped](#ros_read_transform_stamped) (function)
- [ros_write_pose_stamped](#ros_write_pose_stamped) (function)
- [ros_read_pose_stamped](#ros_read_pose_stamped) (function)
- [ros_write_pose_cov](#ros_write_pose_cov) (function)
- [ros_read_pose_cov](#ros_read_pose_cov) (function)
- [ros_write_twist_cov](#ros_write_twist_cov) (function)
- [ros_read_twist_cov](#ros_read_twist_cov) (function)
- [ros_write_imu](#ros_write_imu) (function)
- [ros_read_imu](#ros_read_imu) (function)
- [ros_write_laser_scan](#ros_write_laser_scan) (function)
- [ros_read_laser_scan](#ros_read_laser_scan) (function)
- [ros_write_point_field](#ros_write_point_field) (function)
- [ros_read_point_field](#ros_read_point_field) (function)
- [ros_write_point_cloud2](#ros_write_point_cloud2) (function)
- [ros_read_point_cloud2](#ros_read_point_cloud2) (function)
- [ros_write_odometry](#ros_write_odometry) (function)
- [ros_read_odometry](#ros_read_odometry) (function)
- [ros_serialize_header](#ros_serialize_header) (function)
- [ros_serialize_twist](#ros_serialize_twist) (function)
- [ros_serialize_pose_stamped](#ros_serialize_pose_stamped) (function)
- [ros_serialize_transform_stamped](#ros_serialize_transform_stamped) (function)
- [ros_serialize_imu](#ros_serialize_imu) (function)
- [ros_serialize_laser_scan](#ros_serialize_laser_scan) (function)
- [ros_serialize_point_cloud2](#ros_serialize_point_cloud2) (function)
- [ros_serialize_odometry](#ros_serialize_odometry) (function)
- [ros_serialize_string](#ros_serialize_string) (function)
- [ros_cloud_xyz](#ros_cloud_xyz) (function)
- [ros_cloud_field_offset](#ros_cloud_field_offset) (function)
- [ros_cloud_get_f32](#ros_cloud_get_f32) (function)
- [cdr_hex](#cdr_hex) (function)

## CdrWriter

```fides
pub struct CdrWriter {
```

**Fields:**

| Name | Type |
|------|------|
| `buf` | `Vec<i64>` |
| `origin` | `i64` |

*Defined at line 41*

---

## cdr_writer

A writer with the little-endian encapsulation header (00 01 00 00).

```fides
pub fn cdr_writer() -> CdrWriter {
```

**Returns:** `CdrWriter`

*Defined at line 48*

---

## cdr_writer_raw

A writer with no encapsulation header (for a nested buffer).

```fides
pub fn cdr_writer_raw() -> CdrWriter {
```

**Returns:** `CdrWriter`

*Defined at line 60*

---

## cdr_align

```fides
pub fn cdr_align(w: &mut CdrWriter, n: i64) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `n`: `i64`

*Defined at line 66*

---

## cdr_write_u8

```fides
pub fn cdr_write_u8(w: &mut CdrWriter, v: i64) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `v`: `i64`

*Defined at line 82*

---

## cdr_write_bool

```fides
pub fn cdr_write_bool(w: &mut CdrWriter, v: bool) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `v`: `bool`

*Defined at line 85*

---

## cdr_write_u16

```fides
pub fn cdr_write_u16(w: &mut CdrWriter, v: i64) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `v`: `i64`

*Defined at line 92*

---

## cdr_write_u32

```fides
pub fn cdr_write_u32(w: &mut CdrWriter, v: i64) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `v`: `i64`

*Defined at line 95*

---

## cdr_write_i32

```fides
pub fn cdr_write_i32(w: &mut CdrWriter, v: i64) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `v`: `i64`

*Defined at line 98*

---

## cdr_write_u64

```fides
pub fn cdr_write_u64(w: &mut CdrWriter, v: i64) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `v`: `i64`

*Defined at line 101*

---

## cdr_write_f64

```fides
pub fn cdr_write_f64(w: &mut CdrWriter, v: f64) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `v`: `f64`

*Defined at line 104*

---

## cdr_write_f32

```fides
pub fn cdr_write_f32(w: &mut CdrWriter, v: f64) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `v`: `f64`

*Defined at line 107*

---

## cdr_write_string

```fides
pub fn cdr_write_string(w: &mut CdrWriter, s: str) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `s`: `str`

*Defined at line 111*

---

## cdr_write_f64_array

```fides
pub fn cdr_write_f64_array(w: &mut CdrWriter, v: &Vec<f64>, n: i64) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `v`: `&Vec<f64>`
- `n`: `i64`

*Defined at line 122*

---

## cdr_write_f32_seq

```fides
pub fn cdr_write_f32_seq(w: &mut CdrWriter, v: &Vec<f64>) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `v`: `&Vec<f64>`

*Defined at line 134*

---

## cdr_write_u8_seq

```fides
pub fn cdr_write_u8_seq(w: &mut CdrWriter, v: &Vec<i64>) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `v`: `&Vec<i64>`

*Defined at line 143*

---

## CdrReader

```fides
pub struct CdrReader {
```

**Fields:**

| Name | Type |
|------|------|
| `buf` | `Vec<i64>` |
| `pos` | `i64` |
| `origin` | `i64` |
| `little` | `bool` |
| `ok` | `bool` |

*Defined at line 154*

---

## cdr_reader

A reader over a serialized message with its encapsulation header.

```fides
pub fn cdr_reader(b: Vec<i64>) -> CdrReader {
```

**Parameters:**

- `b`: `Vec<i64>`

**Returns:** `CdrReader`

*Defined at line 163*

---

## cdr_reader_raw

A reader without encapsulation header.

```fides
pub fn cdr_reader_raw(b: Vec<i64>, little: bool) -> CdrReader {
```

**Parameters:**

- `b`: `Vec<i64>`
- `little`: `bool`

**Returns:** `CdrReader`

*Defined at line 182*

---

## cdr_read_u8

```fides
pub fn cdr_read_u8(r: &mut CdrReader) -> i64 {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `i64`

*Defined at line 212*

---

## cdr_read_bool

```fides
pub fn cdr_read_bool(r: &mut CdrReader) -> bool {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `bool`

*Defined at line 215*

---

## cdr_read_u16

```fides
pub fn cdr_read_u16(r: &mut CdrReader) -> i64 {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `i64`

*Defined at line 218*

---

## cdr_read_u32

```fides
pub fn cdr_read_u32(r: &mut CdrReader) -> i64 {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `i64`

*Defined at line 221*

---

## cdr_read_i32

A signed 32-bit value.

```fides
pub fn cdr_read_i32(r: &mut CdrReader) -> i64 {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `i64`

*Defined at line 226*

---

## cdr_read_u64

```fides
pub fn cdr_read_u64(r: &mut CdrReader) -> i64 {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `i64`

*Defined at line 234*

---

## cdr_read_f64

```fides
pub fn cdr_read_f64(r: &mut CdrReader) -> f64 {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `f64`

*Defined at line 237*

---

## cdr_read_f32

```fides
pub fn cdr_read_f32(r: &mut CdrReader) -> f64 {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `f64`

*Defined at line 240*

---

## cdr_read_string

```fides
pub fn cdr_read_string(r: &mut CdrReader) -> str {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `str`

*Defined at line 244*

---

## cdr_read_f64_array

```fides
pub fn cdr_read_f64_array(r: &mut CdrReader, n: i64) -> Vec<f64> {
```

**Parameters:**

- `r`: `&mut CdrReader`
- `n`: `i64`

**Returns:** `Vec<f64>`

*Defined at line 262*

---

## cdr_read_f32_seq

```fides
pub fn cdr_read_f32_seq(r: &mut CdrReader) -> Vec<f64> {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `Vec<f64>`

*Defined at line 272*

---

## cdr_read_u8_seq

```fides
pub fn cdr_read_u8_seq(r: &mut CdrReader) -> Vec<i64> {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `Vec<i64>`

*Defined at line 287*

---

## cdr_f32_bits

```fides
pub fn cdr_f32_bits(x: f64) -> i64 {
```

**Parameters:**

- `x`: `f64`

**Returns:** `i64`

*Defined at line 307*

---

## cdr_f32_from_bits

```fides
pub fn cdr_f32_from_bits(bits: i64) -> f64 {
```

**Parameters:**

- `bits`: `i64`

**Returns:** `f64`

*Defined at line 353*

---

## RosTime

```fides
pub struct RosTime {
```

**Fields:**

| Name | Type |
|------|------|
| `sec` | `i64` |
| `nanosec` | `i64` |

*Defined at line 381*

---

## RosHeader

```fides
pub struct RosHeader {
```

**Fields:**

| Name | Type |
|------|------|
| `stamp` | `RosTime` |
| `frame_id` | `str` |

*Defined at line 386*

---

## RosVector3

```fides
pub struct RosVector3 {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `f64` |
| `y` | `f64` |
| `z` | `f64` |

*Defined at line 391*

---

## RosQuaternion

```fides
pub struct RosQuaternion {
```

**Fields:**

| Name | Type |
|------|------|
| `x` | `f64` |
| `y` | `f64` |
| `z` | `f64` |
| `w` | `f64` |

*Defined at line 397*

---

## RosPose

```fides
pub struct RosPose {
```

**Fields:**

| Name | Type |
|------|------|
| `position` | `RosVector3` |
| `orientation` | `RosQuaternion` |

*Defined at line 404*

---

## RosTwist

```fides
pub struct RosTwist {
```

**Fields:**

| Name | Type |
|------|------|
| `linear` | `RosVector3` |
| `angular` | `RosVector3` |

*Defined at line 410*

---

## RosTransform

```fides
pub struct RosTransform {
```

**Fields:**

| Name | Type |
|------|------|
| `translation` | `RosVector3` |
| `rotation` | `RosQuaternion` |

*Defined at line 415*

---

## RosTransformStamped

```fides
pub struct RosTransformStamped {
```

**Fields:**

| Name | Type |
|------|------|
| `header` | `RosHeader` |
| `child_frame_id` | `str` |
| `transform` | `RosTransform` |

*Defined at line 420*

---

## RosPoseStamped

```fides
pub struct RosPoseStamped {
```

**Fields:**

| Name | Type |
|------|------|
| `header` | `RosHeader` |
| `pose` | `RosPose` |

*Defined at line 426*

---

## RosPoseWithCov

```fides
pub struct RosPoseWithCov {
```

**Fields:**

| Name | Type |
|------|------|
| `pose` | `RosPose` |
| `covariance` | `Vec<f64>` |

*Defined at line 431*

---

## RosTwistWithCov

```fides
pub struct RosTwistWithCov {
```

**Fields:**

| Name | Type |
|------|------|
| `twist` | `RosTwist` |
| `covariance` | `Vec<f64>` |

*Defined at line 437*

---

## RosImu

```fides
pub struct RosImu {
```

**Fields:**

| Name | Type |
|------|------|
| `header` | `RosHeader` |
| `orientation` | `RosQuaternion` |
| `orientation_covariance` | `Vec<f64>` |
| `angular_velocity` | `RosVector3` |
| `angular_velocity_covariance` | `Vec<f64>` |
| `linear_acceleration` | `RosVector3` |
| `linear_acceleration_covariance` | `Vec<f64>` |

*Defined at line 442*

---

## RosLaserScan

```fides
pub struct RosLaserScan {
```

**Fields:**

| Name | Type |
|------|------|
| `header` | `RosHeader` |
| `angle_min` | `f64` |
| `angle_max` | `f64` |
| `angle_increment` | `f64` |
| `time_increment` | `f64` |
| `scan_time` | `f64` |
| `range_min` | `f64` |
| `range_max` | `f64` |
| `ranges` | `Vec<f64>` |
| `intensities` | `Vec<f64>` |

*Defined at line 452*

---

## ROS_PF_INT8:

PointField datatypes.

```fides
pub const ROS_PF_INT8: i64 = 1;
```

*Defined at line 466*

---

## ROS_PF_UINT8:

```fides
pub const ROS_PF_UINT8: i64 = 2;
```

*Defined at line 467*

---

## ROS_PF_INT16:

```fides
pub const ROS_PF_INT16: i64 = 3;
```

*Defined at line 468*

---

## ROS_PF_UINT16:

```fides
pub const ROS_PF_UINT16: i64 = 4;
```

*Defined at line 469*

---

## ROS_PF_INT32:

```fides
pub const ROS_PF_INT32: i64 = 5;
```

*Defined at line 470*

---

## ROS_PF_UINT32:

```fides
pub const ROS_PF_UINT32: i64 = 6;
```

*Defined at line 471*

---

## ROS_PF_FLOAT32:

```fides
pub const ROS_PF_FLOAT32: i64 = 7;
```

*Defined at line 472*

---

## ROS_PF_FLOAT64:

```fides
pub const ROS_PF_FLOAT64: i64 = 8;
```

*Defined at line 473*

---

## RosPointField

```fides
pub struct RosPointField {
```

**Fields:**

| Name | Type |
|------|------|
| `name` | `str` |
| `offset` | `i64` |
| `datatype` | `i64` |
| `count` | `i64` |

*Defined at line 475*

---

## RosPointCloud2

```fides
pub struct RosPointCloud2 {
```

**Fields:**

| Name | Type |
|------|------|
| `header` | `RosHeader` |
| `height` | `i64` |
| `width` | `i64` |
| `fields` | `Vec<RosPointField>` |
| `is_bigendian` | `bool` |
| `point_step` | `i64` |
| `row_step` | `i64` |
| `data` | `Vec<i64>` |
| `is_dense` | `bool` |

*Defined at line 482*

---

## RosOdometry

```fides
pub struct RosOdometry {
```

**Fields:**

| Name | Type |
|------|------|
| `header` | `RosHeader` |
| `child_frame_id` | `str` |
| `pose` | `RosPoseWithCov` |
| `twist` | `RosTwistWithCov` |

*Defined at line 494*

---

## ros_time

```fides
pub fn ros_time(sec: i64, nanosec: i64) -> RosTime {
```

**Parameters:**

- `sec`: `i64`
- `nanosec`: `i64`

**Returns:** `RosTime`

*Defined at line 503*

---

## ros_header

```fides
pub fn ros_header(sec: i64, nanosec: i64, frame: str) -> RosHeader {
```

**Parameters:**

- `sec`: `i64`
- `nanosec`: `i64`
- `frame`: `str`

**Returns:** `RosHeader`

*Defined at line 509*

---

## ros_vec3

```fides
pub fn ros_vec3(x: f64, y: f64, z: f64) -> RosVector3 {
```

**Parameters:**

- `x`: `f64`
- `y`: `f64`
- `z`: `f64`

**Returns:** `RosVector3`

*Defined at line 515*

---

## ros_quat

```fides
pub fn ros_quat(x: f64, y: f64, z: f64, w: f64) -> RosQuaternion {
```

**Parameters:**

- `x`: `f64`
- `y`: `f64`
- `z`: `f64`
- `w`: `f64`

**Returns:** `RosQuaternion`

*Defined at line 521*

---

## ros_pose

```fides
pub fn ros_pose(p: RosVector3, q: RosQuaternion) -> RosPose {
```

**Parameters:**

- `p`: `RosVector3`
- `q`: `RosQuaternion`

**Returns:** `RosPose`

*Defined at line 527*

---

## ros_twist

```fides
pub fn ros_twist(lin: RosVector3, ang: RosVector3) -> RosTwist {
```

**Parameters:**

- `lin`: `RosVector3`
- `ang`: `RosVector3`

**Returns:** `RosTwist`

*Defined at line 533*

---

## ros_zeros

n zeros (covariance arrays).

```fides
pub fn ros_zeros(n: i64) -> Vec<f64> {
```

**Parameters:**

- `n`: `i64`

**Returns:** `Vec<f64>`

*Defined at line 540*

---

## ros_write_time

```fides
pub fn ros_write_time(w: &mut CdrWriter, t: &RosTime) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `t`: `&RosTime`

*Defined at line 552*

---

## ros_read_time

```fides
pub fn ros_read_time(r: &mut CdrReader) -> RosTime {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `RosTime`

*Defined at line 557*

---

## ros_write_header

```fides
pub fn ros_write_header(w: &mut CdrWriter, h: &RosHeader) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `h`: `&RosHeader`

*Defined at line 565*

---

## ros_read_header

```fides
pub fn ros_read_header(r: &mut CdrReader) -> RosHeader {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `RosHeader`

*Defined at line 570*

---

## ros_write_vec3

```fides
pub fn ros_write_vec3(w: &mut CdrWriter, v: &RosVector3) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `v`: `&RosVector3`

*Defined at line 578*

---

## ros_read_vec3

```fides
pub fn ros_read_vec3(r: &mut CdrReader) -> RosVector3 {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `RosVector3`

*Defined at line 584*

---

## ros_write_quat

```fides
pub fn ros_write_quat(w: &mut CdrWriter, q: &RosQuaternion) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `q`: `&RosQuaternion`

*Defined at line 593*

---

## ros_read_quat

```fides
pub fn ros_read_quat(r: &mut CdrReader) -> RosQuaternion {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `RosQuaternion`

*Defined at line 600*

---

## ros_write_pose

```fides
pub fn ros_write_pose(w: &mut CdrWriter, p: &RosPose) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `p`: `&RosPose`

*Defined at line 610*

---

## ros_read_pose

```fides
pub fn ros_read_pose(r: &mut CdrReader) -> RosPose {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `RosPose`

*Defined at line 615*

---

## ros_write_twist

```fides
pub fn ros_write_twist(w: &mut CdrWriter, t: &RosTwist) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `t`: `&RosTwist`

*Defined at line 623*

---

## ros_read_twist

```fides
pub fn ros_read_twist(r: &mut CdrReader) -> RosTwist {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `RosTwist`

*Defined at line 628*

---

## ros_write_transform

```fides
pub fn ros_write_transform(w: &mut CdrWriter, t: &RosTransform) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `t`: `&RosTransform`

*Defined at line 636*

---

## ros_read_transform

```fides
pub fn ros_read_transform(r: &mut CdrReader) -> RosTransform {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `RosTransform`

*Defined at line 641*

---

## ros_write_transform_stamped

```fides
pub fn ros_write_transform_stamped(w: &mut CdrWriter, t: &RosTransformStamped) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `t`: `&RosTransformStamped`

*Defined at line 649*

---

## ros_read_transform_stamped

```fides
pub fn ros_read_transform_stamped(r: &mut CdrReader) -> RosTransformStamped {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `RosTransformStamped`

*Defined at line 655*

---

## ros_write_pose_stamped

```fides
pub fn ros_write_pose_stamped(w: &mut CdrWriter, p: &RosPoseStamped) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `p`: `&RosPoseStamped`

*Defined at line 664*

---

## ros_read_pose_stamped

```fides
pub fn ros_read_pose_stamped(r: &mut CdrReader) -> RosPoseStamped {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `RosPoseStamped`

*Defined at line 669*

---

## ros_write_pose_cov

```fides
pub fn ros_write_pose_cov(w: &mut CdrWriter, p: &RosPoseWithCov) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `p`: `&RosPoseWithCov`

*Defined at line 677*

---

## ros_read_pose_cov

```fides
pub fn ros_read_pose_cov(r: &mut CdrReader) -> RosPoseWithCov {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `RosPoseWithCov`

*Defined at line 682*

---

## ros_write_twist_cov

```fides
pub fn ros_write_twist_cov(w: &mut CdrWriter, t: &RosTwistWithCov) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `t`: `&RosTwistWithCov`

*Defined at line 690*

---

## ros_read_twist_cov

```fides
pub fn ros_read_twist_cov(r: &mut CdrReader) -> RosTwistWithCov {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `RosTwistWithCov`

*Defined at line 695*

---

## ros_write_imu

```fides
pub fn ros_write_imu(w: &mut CdrWriter, m: &RosImu) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `m`: `&RosImu`

*Defined at line 703*

---

## ros_read_imu

```fides
pub fn ros_read_imu(r: &mut CdrReader) -> RosImu {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `RosImu`

*Defined at line 713*

---

## ros_write_laser_scan

```fides
pub fn ros_write_laser_scan(w: &mut CdrWriter, s: &RosLaserScan) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `s`: `&RosLaserScan`

*Defined at line 726*

---

## ros_read_laser_scan

```fides
pub fn ros_read_laser_scan(r: &mut CdrReader) -> RosLaserScan {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `RosLaserScan`

*Defined at line 739*

---

## ros_write_point_field

```fides
pub fn ros_write_point_field(w: &mut CdrWriter, f: &RosPointField) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `f`: `&RosPointField`

*Defined at line 755*

---

## ros_read_point_field

```fides
pub fn ros_read_point_field(r: &mut CdrReader) -> RosPointField {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `RosPointField`

*Defined at line 762*

---

## ros_write_point_cloud2

```fides
pub fn ros_write_point_cloud2(w: &mut CdrWriter, c: &RosPointCloud2) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `c`: `&RosPointCloud2`

*Defined at line 772*

---

## ros_read_point_cloud2

```fides
pub fn ros_read_point_cloud2(r: &mut CdrReader) -> RosPointCloud2 {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `RosPointCloud2`

*Defined at line 790*

---

## ros_write_odometry

```fides
pub fn ros_write_odometry(w: &mut CdrWriter, o: &RosOdometry) {
```

**Parameters:**

- `w`: `&mut CdrWriter`
- `o`: `&RosOdometry`

*Defined at line 811*

---

## ros_read_odometry

```fides
pub fn ros_read_odometry(r: &mut CdrReader) -> RosOdometry {
```

**Parameters:**

- `r`: `&mut CdrReader`

**Returns:** `RosOdometry`

*Defined at line 818*

---

## ros_serialize_header

```fides
pub fn ros_serialize_header(h: &RosHeader) -> Vec<i64> {
```

**Parameters:**

- `h`: `&RosHeader`

**Returns:** `Vec<i64>`

*Defined at line 830*

---

## ros_serialize_twist

```fides
pub fn ros_serialize_twist(t: &RosTwist) -> Vec<i64> {
```

**Parameters:**

- `t`: `&RosTwist`

**Returns:** `Vec<i64>`

*Defined at line 836*

---

## ros_serialize_pose_stamped

```fides
pub fn ros_serialize_pose_stamped(p: &RosPoseStamped) -> Vec<i64> {
```

**Parameters:**

- `p`: `&RosPoseStamped`

**Returns:** `Vec<i64>`

*Defined at line 842*

---

## ros_serialize_transform_stamped

```fides
pub fn ros_serialize_transform_stamped(t: &RosTransformStamped) -> Vec<i64> {
```

**Parameters:**

- `t`: `&RosTransformStamped`

**Returns:** `Vec<i64>`

*Defined at line 848*

---

## ros_serialize_imu

```fides
pub fn ros_serialize_imu(m: &RosImu) -> Vec<i64> {
```

**Parameters:**

- `m`: `&RosImu`

**Returns:** `Vec<i64>`

*Defined at line 854*

---

## ros_serialize_laser_scan

```fides
pub fn ros_serialize_laser_scan(s: &RosLaserScan) -> Vec<i64> {
```

**Parameters:**

- `s`: `&RosLaserScan`

**Returns:** `Vec<i64>`

*Defined at line 860*

---

## ros_serialize_point_cloud2

```fides
pub fn ros_serialize_point_cloud2(c: &RosPointCloud2) -> Vec<i64> {
```

**Parameters:**

- `c`: `&RosPointCloud2`

**Returns:** `Vec<i64>`

*Defined at line 866*

---

## ros_serialize_odometry

```fides
pub fn ros_serialize_odometry(o: &RosOdometry) -> Vec<i64> {
```

**Parameters:**

- `o`: `&RosOdometry`

**Returns:** `Vec<i64>`

*Defined at line 872*

---

## ros_serialize_string

std_msgs/String.

```fides
pub fn ros_serialize_string(s: str) -> Vec<i64> {
```

**Parameters:**

- `s`: `str`

**Returns:** `Vec<i64>`

*Defined at line 879*

---

## ros_cloud_xyz

An unorganized (height 1) cloud of x, y, z float32 points, little-endian.

```fides
pub fn ros_cloud_xyz(h: RosHeader, xs: &Vec<f64>, ys: &Vec<f64>, zs: &Vec<f64>) -> RosPointCloud2 {
```

**Parameters:**

- `h`: `RosHeader`
- `xs`: `&Vec<f64>`
- `ys`: `&Vec<f64>`
- `zs`: `&Vec<f64>`

**Returns:** `RosPointCloud2`

*Defined at line 888*

---

## ros_cloud_field_offset

The offset of a named field (-1 when the cloud has none).

```fides
pub fn ros_cloud_field_offset(c: &RosPointCloud2, name: str) -> i64 {
```

**Parameters:**

- `c`: `&RosPointCloud2`
- `name`: `str`

**Returns:** `i64`

*Defined at line 929*

---

## ros_cloud_get_f32

A float32 field of point i (0.0 when out of range or absent).

```fides
pub fn ros_cloud_get_f32(c: &RosPointCloud2, i: i64, name: str) -> f64 {
```

**Parameters:**

- `c`: `&RosPointCloud2`
- `i`: `i64`
- `name`: `str`

**Returns:** `f64`

*Defined at line 941*

---

## cdr_hex

Hex dump "00 01 00 00 ..." of bytes.

```fides
pub fn cdr_hex(b: &Vec<i64>) -> str {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `str`

*Defined at line 961*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
