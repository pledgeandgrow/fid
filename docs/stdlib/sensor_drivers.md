[All modules](index.md)

# sensor_drivers -- Fides Documentation

## Contents

- [SENSOR_I2C:](#SENSOR_I2C:) (const)
- [SENSOR_SPI:](#SENSOR_SPI:) (const)
- [SensorBus](#SensorBus) (struct)
- [sensor_on_i2c](#sensor_on_i2c) (function)
- [sensor_on_spi](#sensor_on_spi) (function)
- [sensor_read_regs](#sensor_read_regs) (function)
- [sensor_write_reg](#sensor_write_reg) (function)
- [sensor_read_reg](#sensor_read_reg) (function)
- [sensor_command](#sensor_command) (function)
- [sensor_read_raw](#sensor_read_raw) (function)
- [BME280_ADDR:](#BME280_ADDR:) (const)
- [BME280_ADDR_ALT:](#BME280_ADDR_ALT:) (const)
- [BME280_CHIP_ID:](#BME280_CHIP_ID:) (const)
- [BMP280_CHIP_ID:](#BMP280_CHIP_ID:) (const)
- [BME280_REG_ID:](#BME280_REG_ID:) (const)
- [BME280_REG_RESET:](#BME280_REG_RESET:) (const)
- [BME280_REG_CTRL_HUM:](#BME280_REG_CTRL_HUM:) (const)
- [BME280_REG_STATUS:](#BME280_REG_STATUS:) (const)
- [BME280_REG_CTRL_MEAS:](#BME280_REG_CTRL_MEAS:) (const)
- [BME280_REG_CONFIG:](#BME280_REG_CONFIG:) (const)
- [BME280_REG_DATA:](#BME280_REG_DATA:) (const)
- [BME280_MODE_SLEEP:](#BME280_MODE_SLEEP:) (const)
- [BME280_MODE_FORCED:](#BME280_MODE_FORCED:) (const)
- [BME280_MODE_NORMAL:](#BME280_MODE_NORMAL:) (const)
- [Bme280Cal](#Bme280Cal) (struct)
- [bme280_parse_cal](#bme280_parse_cal) (function)
- [bme280_t_fine](#bme280_t_fine) (function)
- [bme280_temp_c100](#bme280_temp_c100) (function)
- [bme280_pressure_q24_8](#bme280_pressure_q24_8) (function)
- [bme280_humidity_q22_10](#bme280_humidity_q22_10) (function)
- [bme280_compensate_f64](#bme280_compensate_f64) (function)
- [bme280_parse_data](#bme280_parse_data) (function)
- [bme280_osrs](#bme280_osrs) (function)
- [Bme280](#Bme280) (struct)
- [bme280_init](#bme280_init) (function)
- [bme280_read](#bme280_read) (function)
- [MPU6050_ADDR:](#MPU6050_ADDR:) (const)
- [MPU6050_WHO_AM_I_VALUE:](#MPU6050_WHO_AM_I_VALUE:) (const)
- [MPU6050_REG_SMPLRT_DIV:](#MPU6050_REG_SMPLRT_DIV:) (const)
- [MPU6050_REG_CONFIG:](#MPU6050_REG_CONFIG:) (const)
- [MPU6050_REG_GYRO_CONFIG:](#MPU6050_REG_GYRO_CONFIG:) (const)
- [MPU6050_REG_ACCEL_CONFIG:](#MPU6050_REG_ACCEL_CONFIG:) (const)
- [MPU6050_REG_ACCEL_XOUT_H:](#MPU6050_REG_ACCEL_XOUT_H:) (const)
- [MPU6050_REG_PWR_MGMT_1:](#MPU6050_REG_PWR_MGMT_1:) (const)
- [MPU6050_REG_WHO_AM_I:](#MPU6050_REG_WHO_AM_I:) (const)
- [mpu6050_accel_lsb_per_g](#mpu6050_accel_lsb_per_g) (function)
- [mpu6050_gyro_lsb10](#mpu6050_gyro_lsb10) (function)
- [Mpu6050](#Mpu6050) (struct)
- [mpu6050_init](#mpu6050_init) (function)
- [mpu6050_scale](#mpu6050_scale) (function)
- [mpu6050_read](#mpu6050_read) (function)
- [VL53L0X_ADDR:](#VL53L0X_ADDR:) (const)
- [VL53L0X_REG_SYSRANGE_START:](#VL53L0X_REG_SYSRANGE_START:) (const)
- [VL53L0X_REG_SYSTEM_INTERRUPT_CLEAR:](#VL53L0X_REG_SYSTEM_INTERRUPT_CLEAR:) (const)
- [VL53L0X_REG_RESULT_INTERRUPT_STATUS:](#VL53L0X_REG_RESULT_INTERRUPT_STATUS:) (const)
- [VL53L0X_REG_RESULT_RANGE_STATUS:](#VL53L0X_REG_RESULT_RANGE_STATUS:) (const)
- [VL53L0X_REG_MODEL_ID:](#VL53L0X_REG_MODEL_ID:) (const)
- [VL53L0X_REG_REVISION_ID:](#VL53L0X_REG_REVISION_ID:) (const)
- [VL53L0X_MODEL_ID_VALUE:](#VL53L0X_MODEL_ID_VALUE:) (const)
- [vl53l0x_range_status](#vl53l0x_range_status) (function)
- [vl53l0x_present](#vl53l0x_present) (function)
- [vl53l0x_start_single](#vl53l0x_start_single) (function)
- [vl53l0x_read_result](#vl53l0x_read_result) (function)
- [SHT3X_ADDR:](#SHT3X_ADDR:) (const)
- [SHT3X_ADDR_ALT:](#SHT3X_ADDR_ALT:) (const)
- [SHT3X_MEAS_HIGH_STRETCH:](#SHT3X_MEAS_HIGH_STRETCH:) (const)
- [SHT3X_MEAS_MED_STRETCH:](#SHT3X_MEAS_MED_STRETCH:) (const)
- [SHT3X_MEAS_LOW_STRETCH:](#SHT3X_MEAS_LOW_STRETCH:) (const)
- [SHT3X_MEAS_HIGH:](#SHT3X_MEAS_HIGH:) (const)
- [SHT3X_MEAS_MED:](#SHT3X_MEAS_MED:) (const)
- [SHT3X_MEAS_LOW:](#SHT3X_MEAS_LOW:) (const)
- [SHT3X_SOFT_RESET:](#SHT3X_SOFT_RESET:) (const)
- [SHT3X_HEATER_ON:](#SHT3X_HEATER_ON:) (const)
- [SHT3X_HEATER_OFF:](#SHT3X_HEATER_OFF:) (const)
- [SHT3X_READ_STATUS:](#SHT3X_READ_STATUS:) (const)
- [SHT3X_CLEAR_STATUS:](#SHT3X_CLEAR_STATUS:) (const)
- [sht3x_crc8](#sht3x_crc8) (function)
- [sht3x_command_bytes](#sht3x_command_bytes) (function)
- [sht3x_temp_c100](#sht3x_temp_c100) (function)
- [sht3x_rh100](#sht3x_rh100) (function)
- [sht3x_decode](#sht3x_decode) (function)
- [sht3x_measure](#sht3x_measure) (function)
- [BH1750_ADDR:](#BH1750_ADDR:) (const)
- [BH1750_ADDR_ALT:](#BH1750_ADDR_ALT:) (const)
- [BH1750_POWER_DOWN:](#BH1750_POWER_DOWN:) (const)
- [BH1750_POWER_ON:](#BH1750_POWER_ON:) (const)
- [BH1750_RESET:](#BH1750_RESET:) (const)
- [BH1750_CONT_H:](#BH1750_CONT_H:) (const)
- [BH1750_CONT_H2:](#BH1750_CONT_H2:) (const)
- [BH1750_CONT_L:](#BH1750_CONT_L:) (const)
- [BH1750_ONE_H:](#BH1750_ONE_H:) (const)
- [BH1750_ONE_H2:](#BH1750_ONE_H2:) (const)
- [BH1750_ONE_L:](#BH1750_ONE_L:) (const)
- [BH1750_MTREG_DEFAULT:](#BH1750_MTREG_DEFAULT:) (const)
- [bh1750_mtreg_commands](#bh1750_mtreg_commands) (function)
- [bh1750_lux100](#bh1750_lux100) (function)
- [bh1750_read_lux100](#bh1750_read_lux100) (function)
- [INA219_ADDR:](#INA219_ADDR:) (const)
- [INA219_REG_CONFIG:](#INA219_REG_CONFIG:) (const)
- [INA219_REG_SHUNT:](#INA219_REG_SHUNT:) (const)
- [INA219_REG_BUS:](#INA219_REG_BUS:) (const)
- [INA219_REG_POWER:](#INA219_REG_POWER:) (const)
- [INA219_REG_CURRENT:](#INA219_REG_CURRENT:) (const)
- [INA219_REG_CAL:](#INA219_REG_CAL:) (const)
- [INA219_CONFIG_DEFAULT:](#INA219_CONFIG_DEFAULT:) (const)
- [ina219_config](#ina219_config) (function)
- [ina219_calibration](#ina219_calibration) (function)
- [ina219_shunt_uv](#ina219_shunt_uv) (function)
- [ina219_bus_mv](#ina219_bus_mv) (function)
- [ina219_conversion_ready](#ina219_conversion_ready) (function)
- [ina219_overflow](#ina219_overflow) (function)
- [ina219_current_ua](#ina219_current_ua) (function)
- [ina219_power_uw](#ina219_power_uw) (function)
- [ina219_current_reg](#ina219_current_reg) (function)
- [ina219_power_reg](#ina219_power_reg) (function)
- [Ina219](#Ina219) (struct)
- [ina219_init](#ina219_init) (function)
- [ina219_read](#ina219_read) (function)

## SENSOR_I2C:

```fides
pub const SENSOR_I2C: i64 = 0;
```

*Defined at line 34*

---

## SENSOR_SPI:

```fides
pub const SENSOR_SPI: i64 = 1;
```

*Defined at line 35*

---

## SensorBus

A sensor's link: an I2C address on a bus, or a chip select on an SPI
bus (register reads with bit 7 set, writes with it clear).

```fides
pub struct SensorBus {
```

**Fields:**

| Name | Type |
|------|------|
| `kind` | `i64` |
| `addr` | `i64` |
| `i2c` | `I2cBus` |
| `spi` | `SpiBus` |

*Defined at line 39*

---

## sensor_on_i2c

```fides
pub fn sensor_on_i2c(bus: I2cBus, addr: i64) -> SensorBus {
```

**Parameters:**

- `bus`: `I2cBus`
- `addr`: `i64`

**Returns:** `SensorBus`

*Defined at line 46*

---

## sensor_on_spi

```fides
pub fn sensor_on_spi(bus: SpiBus) -> SensorBus {
```

**Parameters:**

- `bus`: `SpiBus`

**Returns:** `SensorBus`

*Defined at line 52*

---

## sensor_read_regs

```fides
pub fn sensor_read_regs(s: &mut SensorBus, reg: i64, n: i64) -> Result<Vec<i64>, str> {
```

**Parameters:**

- `s`: `&mut SensorBus`
- `reg`: `i64`
- `n`: `i64`

**Returns:** `Result<Vec<i64>, str>`

*Defined at line 58*

---

## sensor_write_reg

```fides
pub fn sensor_write_reg(s: &mut SensorBus, reg: i64, value: i64) -> bool {
```

**Parameters:**

- `s`: `&mut SensorBus`
- `reg`: `i64`
- `value`: `i64`

**Returns:** `bool`

*Defined at line 65*

---

## sensor_read_reg

```fides
pub fn sensor_read_reg(s: &mut SensorBus, reg: i64) -> i64 {
```

**Parameters:**

- `s`: `&mut SensorBus`
- `reg`: `i64`

**Returns:** `i64`

*Defined at line 74*

---

## sensor_command

Send raw command bytes (I2C devices without registers).

```fides
pub fn sensor_command(s: &mut SensorBus, bytes: Vec<i64>) -> bool {
```

**Parameters:**

- `s`: `&mut SensorBus`
- `bytes`: `Vec<i64>`

**Returns:** `bool`

*Defined at line 86*

---

## sensor_read_raw

```fides
pub fn sensor_read_raw(s: &mut SensorBus, n: i64) -> Result<Vec<i64>, str> {
```

**Parameters:**

- `s`: `&mut SensorBus`
- `n`: `i64`

**Returns:** `Result<Vec<i64>, str>`

*Defined at line 97*

---

## BME280_ADDR:

```fides
pub const BME280_ADDR: i64 = 0x76;
```

*Defined at line 137*

---

## BME280_ADDR_ALT:

```fides
pub const BME280_ADDR_ALT: i64 = 0x77;
```

*Defined at line 138*

---

## BME280_CHIP_ID:

```fides
pub const BME280_CHIP_ID: i64 = 0x60;
```

*Defined at line 139*

---

## BMP280_CHIP_ID:

```fides
pub const BMP280_CHIP_ID: i64 = 0x58;
```

*Defined at line 140*

---

## BME280_REG_ID:

```fides
pub const BME280_REG_ID: i64 = 0xD0;
```

*Defined at line 141*

---

## BME280_REG_RESET:

```fides
pub const BME280_REG_RESET: i64 = 0xE0;
```

*Defined at line 142*

---

## BME280_REG_CTRL_HUM:

```fides
pub const BME280_REG_CTRL_HUM: i64 = 0xF2;
```

*Defined at line 143*

---

## BME280_REG_STATUS:

```fides
pub const BME280_REG_STATUS: i64 = 0xF3;
```

*Defined at line 144*

---

## BME280_REG_CTRL_MEAS:

```fides
pub const BME280_REG_CTRL_MEAS: i64 = 0xF4;
```

*Defined at line 145*

---

## BME280_REG_CONFIG:

```fides
pub const BME280_REG_CONFIG: i64 = 0xF5;
```

*Defined at line 146*

---

## BME280_REG_DATA:

```fides
pub const BME280_REG_DATA: i64 = 0xF7;
```

*Defined at line 147*

---

## BME280_MODE_SLEEP:

```fides
pub const BME280_MODE_SLEEP: i64 = 0;
```

*Defined at line 148*

---

## BME280_MODE_FORCED:

```fides
pub const BME280_MODE_FORCED: i64 = 1;
```

*Defined at line 149*

---

## BME280_MODE_NORMAL:

```fides
pub const BME280_MODE_NORMAL: i64 = 3;
```

*Defined at line 150*

---

## Bme280Cal

```fides
pub struct Bme280Cal {
```

**Fields:**

| Name | Type |
|------|------|
| `t1` | `i64` |
| `t2` | `i64` |
| `t3` | `i64` |
| `p1` | `i64` |
| `p2` | `i64` |
| `p3` | `i64` |
| `p4` | `i64` |
| `p5` | `i64` |
| `p6` | `i64` |
| `p7` | `i64` |
| `p8` | `i64` |
| `p9` | `i64` |
| `h1` | `i64` |
| `h2` | `i64` |
| `h3` | `i64` |
| `h4` | `i64` |
| `h5` | `i64` |
| `h6` | `i64` |

*Defined at line 152*

---

## bme280_parse_cal

Calibration from the 26 bytes at 0x88..0xA1 and the 7 at 0xE1..0xE7
(pass an empty second block for a BMP280).

```fides
pub fn bme280_parse_cal(a: &Vec<i64>, b: &Vec<i64>) -> Bme280Cal {
```

**Parameters:**

- `a`: `&Vec<i64>`
- `b`: `&Vec<i64>`

**Returns:** `Bme280Cal`

*Defined at line 175*

---

## bme280_t_fine

The datasheet's t_fine (shared by pressure and humidity).

```fides
pub fn bme280_t_fine(c: &Bme280Cal, adc_t: i64) -> i64 {
```

**Parameters:**

- `c`: `&Bme280Cal`
- `adc_t`: `i64`

**Returns:** `i64`

*Defined at line 193*

---

## bme280_temp_c100

Temperature in 0.01 C ("5123" = 51.23 C).

```fides
pub fn bme280_temp_c100(t_fine: i64) -> i64 {
```

**Parameters:**

- `t_fine`: `i64`

**Returns:** `i64`

*Defined at line 201*

---

## bme280_pressure_q24_8

Pressure in Pa as Q24.8 (divide by 256), the 64-bit formula.

```fides
pub fn bme280_pressure_q24_8(c: &Bme280Cal, t_fine: i64, adc_p: i64) -> i64 {
```

**Parameters:**

- `c`: `&Bme280Cal`
- `t_fine`: `i64`
- `adc_p`: `i64`

**Returns:** `i64`

*Defined at line 206*

---

## bme280_humidity_q22_10

Relative humidity in %RH as Q22.10 (divide by 1024).

```fides
pub fn bme280_humidity_q22_10(c: &Bme280Cal, t_fine: i64, adc_h: i64) -> i64 {
```

**Parameters:**

- `c`: `&Bme280Cal`
- `t_fine`: `i64`
- `adc_h`: `i64`

**Returns:** `i64`

*Defined at line 224*

---

## bme280_compensate_f64

The datasheet's double-precision formulas: [temp C, pressure Pa, %RH].

```fides
pub fn bme280_compensate_f64(c: &Bme280Cal, adc_t: i64, adc_p: i64, adc_h: i64) -> Vec<f64> {
```

**Parameters:**

- `c`: `&Bme280Cal`
- `adc_t`: `i64`
- `adc_p`: `i64`
- `adc_h`: `i64`

**Returns:** `Vec<f64>`

*Defined at line 240*

---

## bme280_parse_data

Raw ADC values from the 8 data bytes at 0xF7: [adc_p, adc_t, adc_h].

```fides
pub fn bme280_parse_data(d: &Vec<i64>) -> Vec<i64> {
```

**Parameters:**

- `d`: `&Vec<i64>`

**Returns:** `Vec<i64>`

*Defined at line 276*

---

## bme280_osrs

Oversampling setting code for 0,1,2,4,8,16 samples (0 skips).

```fides
pub fn bme280_osrs(n: i64) -> i64 {
```

**Parameters:**

- `n`: `i64`

**Returns:** `i64`

*Defined at line 289*

---

## Bme280

```fides
pub struct Bme280 {
```

**Fields:**

| Name | Type |
|------|------|
| `link` | `SensorBus` |
| `cal` | `Bme280Cal` |
| `has_humidity` | `bool` |

*Defined at line 308*

---

## bme280_init

Check the chip id, read calibration and set oversampling (x1 each),
filter off and normal mode.

```fides
pub fn bme280_init(link: SensorBus) -> Result<Bme280, str> {
```

**Parameters:**

- `link`: `SensorBus`

**Returns:** `Result<Bme280, str>`

*Defined at line 316*

---

## bme280_read

One reading: [temp 0.01 C, pressure Pa Q24.8, humidity Q22.10].

```fides
pub fn bme280_read(dev: &mut Bme280) -> Result<Vec<i64>, str> {
```

**Parameters:**

- `dev`: `&mut Bme280`

**Returns:** `Result<Vec<i64>, str>`

*Defined at line 352*

---

## MPU6050_ADDR:

```fides
pub const MPU6050_ADDR: i64 = 0x68;
```

*Defined at line 379*

---

## MPU6050_WHO_AM_I_VALUE:

```fides
pub const MPU6050_WHO_AM_I_VALUE: i64 = 0x68;
```

*Defined at line 380*

---

## MPU6050_REG_SMPLRT_DIV:

```fides
pub const MPU6050_REG_SMPLRT_DIV: i64 = 0x19;
```

*Defined at line 381*

---

## MPU6050_REG_CONFIG:

```fides
pub const MPU6050_REG_CONFIG: i64 = 0x1A;
```

*Defined at line 382*

---

## MPU6050_REG_GYRO_CONFIG:

```fides
pub const MPU6050_REG_GYRO_CONFIG: i64 = 0x1B;
```

*Defined at line 383*

---

## MPU6050_REG_ACCEL_CONFIG:

```fides
pub const MPU6050_REG_ACCEL_CONFIG: i64 = 0x1C;
```

*Defined at line 384*

---

## MPU6050_REG_ACCEL_XOUT_H:

```fides
pub const MPU6050_REG_ACCEL_XOUT_H: i64 = 0x3B;
```

*Defined at line 385*

---

## MPU6050_REG_PWR_MGMT_1:

```fides
pub const MPU6050_REG_PWR_MGMT_1: i64 = 0x6B;
```

*Defined at line 386*

---

## MPU6050_REG_WHO_AM_I:

```fides
pub const MPU6050_REG_WHO_AM_I: i64 = 0x75;
```

*Defined at line 387*

---

## mpu6050_accel_lsb_per_g

LSB per g for AFS_SEL 0..3 (+-2, 4, 8, 16 g).

```fides
pub fn mpu6050_accel_lsb_per_g(afs: i64) -> i64 {
```

**Parameters:**

- `afs`: `i64`

**Returns:** `i64`

*Defined at line 390*

---

## mpu6050_gyro_lsb10

10 x LSB per deg/s for FS_SEL 0..3 (+-250, 500, 1000, 2000): 1310,
655, 328, 164 (the datasheet's 131, 65.5, 32.8, 16.4).

```fides
pub fn mpu6050_gyro_lsb10(fs: i64) -> i64 {
```

**Parameters:**

- `fs`: `i64`

**Returns:** `i64`

*Defined at line 396*

---

## Mpu6050

```fides
pub struct Mpu6050 {
```

**Fields:**

| Name | Type |
|------|------|
| `link` | `SensorBus` |
| `afs` | `i64` |
| `fs` | `i64` |

*Defined at line 410*

---

## mpu6050_init

Wake the device (clock from the X gyro PLL) and set the ranges.

```fides
pub fn mpu6050_init(link: SensorBus, afs: i64, fs: i64) -> Result<Mpu6050, str> {
```

**Parameters:**

- `link`: `SensorBus`
- `afs`: `i64`
- `fs`: `i64`

**Returns:** `Result<Mpu6050, str>`

*Defined at line 417*

---

## mpu6050_scale

Scale a 14-byte burst from 0x3B: [ax, ay, az] in mg, temperature in
0.001 C, [gx, gy, gz] in 0.001 deg/s.

```fides
pub fn mpu6050_scale(raw: &Vec<i64>, afs: i64, fs: i64) -> Vec<i64> {
```

**Parameters:**

- `raw`: `&Vec<i64>`
- `afs`: `i64`
- `fs`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 433*

---

## mpu6050_read

```fides
pub fn mpu6050_read(dev: &mut Mpu6050) -> Result<Vec<i64>, str> {
```

**Parameters:**

- `dev`: `&mut Mpu6050`

**Returns:** `Result<Vec<i64>, str>`

*Defined at line 452*

---

## VL53L0X_ADDR:

```fides
pub const VL53L0X_ADDR: i64 = 0x29;
```

*Defined at line 465*

---

## VL53L0X_REG_SYSRANGE_START:

```fides
pub const VL53L0X_REG_SYSRANGE_START: i64 = 0x00;
```

*Defined at line 466*

---

## VL53L0X_REG_SYSTEM_INTERRUPT_CLEAR:

```fides
pub const VL53L0X_REG_SYSTEM_INTERRUPT_CLEAR: i64 = 0x0B;
```

*Defined at line 467*

---

## VL53L0X_REG_RESULT_INTERRUPT_STATUS:

```fides
pub const VL53L0X_REG_RESULT_INTERRUPT_STATUS: i64 = 0x13;
```

*Defined at line 468*

---

## VL53L0X_REG_RESULT_RANGE_STATUS:

```fides
pub const VL53L0X_REG_RESULT_RANGE_STATUS: i64 = 0x14;
```

*Defined at line 469*

---

## VL53L0X_REG_MODEL_ID:

```fides
pub const VL53L0X_REG_MODEL_ID: i64 = 0xC0;
```

*Defined at line 470*

---

## VL53L0X_REG_REVISION_ID:

```fides
pub const VL53L0X_REG_REVISION_ID: i64 = 0xC2;
```

*Defined at line 471*

---

## VL53L0X_MODEL_ID_VALUE:

```fides
pub const VL53L0X_MODEL_ID_VALUE: i64 = 0xEE;
```

*Defined at line 472*

---

## vl53l0x_range_status

The device range status (bits 6..3 of RESULT_RANGE_STATUS); 11 means
the range is valid.

```fides
pub fn vl53l0x_range_status(status_reg: i64) -> i64 {
```

**Parameters:**

- `status_reg`: `i64`

**Returns:** `i64`

*Defined at line 476*

---

## vl53l0x_present

```fides
pub fn vl53l0x_present(s: &mut SensorBus) -> bool {
```

**Parameters:**

- `s`: `&mut SensorBus`

**Returns:** `bool`

*Defined at line 480*

---

## vl53l0x_start_single

Start a single measurement (the stop variable read at init time goes
through the 0x80/0xFF/0x00 private page as in the ST API).

```fides
pub fn vl53l0x_start_single(s: &mut SensorBus, stop_variable: i64) -> bool {
```

**Parameters:**

- `s`: `&mut SensorBus`
- `stop_variable`: `i64`

**Returns:** `bool`

*Defined at line 486*

---

## vl53l0x_read_result

Poll for the result (up to max_polls reads of the interrupt status),
then read range and status and clear the interrupt. [range mm, device
range status]; Err on timeout.

```fides
pub fn vl53l0x_read_result(s: &mut SensorBus, max_polls: i64) -> Result<Vec<i64>, str> {
```

**Parameters:**

- `s`: `&mut SensorBus`
- `max_polls`: `i64`

**Returns:** `Result<Vec<i64>, str>`

*Defined at line 500*

---

## SHT3X_ADDR:

```fides
pub const SHT3X_ADDR: i64 = 0x44;
```

*Defined at line 531*

---

## SHT3X_ADDR_ALT:

```fides
pub const SHT3X_ADDR_ALT: i64 = 0x45;
```

*Defined at line 532*

---

## SHT3X_MEAS_HIGH_STRETCH:

```fides
pub const SHT3X_MEAS_HIGH_STRETCH: i64 = 0x2C06;
```

*Defined at line 533*

---

## SHT3X_MEAS_MED_STRETCH:

```fides
pub const SHT3X_MEAS_MED_STRETCH: i64 = 0x2C0D;
```

*Defined at line 534*

---

## SHT3X_MEAS_LOW_STRETCH:

```fides
pub const SHT3X_MEAS_LOW_STRETCH: i64 = 0x2C10;
```

*Defined at line 535*

---

## SHT3X_MEAS_HIGH:

```fides
pub const SHT3X_MEAS_HIGH: i64 = 0x2400;
```

*Defined at line 536*

---

## SHT3X_MEAS_MED:

```fides
pub const SHT3X_MEAS_MED: i64 = 0x240B;
```

*Defined at line 537*

---

## SHT3X_MEAS_LOW:

```fides
pub const SHT3X_MEAS_LOW: i64 = 0x2416;
```

*Defined at line 538*

---

## SHT3X_SOFT_RESET:

```fides
pub const SHT3X_SOFT_RESET: i64 = 0x30A2;
```

*Defined at line 539*

---

## SHT3X_HEATER_ON:

```fides
pub const SHT3X_HEATER_ON: i64 = 0x306D;
```

*Defined at line 540*

---

## SHT3X_HEATER_OFF:

```fides
pub const SHT3X_HEATER_OFF: i64 = 0x3066;
```

*Defined at line 541*

---

## SHT3X_READ_STATUS:

```fides
pub const SHT3X_READ_STATUS: i64 = 0xF32D;
```

*Defined at line 542*

---

## SHT3X_CLEAR_STATUS:

```fides
pub const SHT3X_CLEAR_STATUS: i64 = 0x3041;
```

*Defined at line 543*

---

## sht3x_crc8

Sensirion CRC-8: polynomial 0x31, initial 0xFF, no reflection.

```fides
pub fn sht3x_crc8(bytes: &Vec<i64>) -> i64 {
```

**Parameters:**

- `bytes`: `&Vec<i64>`

**Returns:** `i64`

*Defined at line 546*

---

## sht3x_command_bytes

```fides
pub fn sht3x_command_bytes(cmd: i64) -> Vec<i64> {
```

**Parameters:**

- `cmd`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 565*

---

## sht3x_temp_c100

Temperature in 0.01 C: -45 + 175 * raw / 65535, rounded.

```fides
pub fn sht3x_temp_c100(raw: i64) -> i64 {
```

**Parameters:**

- `raw`: `i64`

**Returns:** `i64`

*Defined at line 573*

---

## sht3x_rh100

Relative humidity in 0.01 %: 100 * raw / 65535, rounded.

```fides
pub fn sht3x_rh100(raw: i64) -> i64 {
```

**Parameters:**

- `raw`: `i64`

**Returns:** `i64`

*Defined at line 578*

---

## sht3x_decode

Decode the 6-byte answer: [temp 0.01 C, RH 0.01 %], Err on a CRC error.

```fides
pub fn sht3x_decode(b: &Vec<i64>) -> Result<Vec<i64>, str> {
```

**Parameters:**

- `b`: `&Vec<i64>`

**Returns:** `Result<Vec<i64>, str>`

*Defined at line 583*

---

## sht3x_measure

Single-shot high repeatability with clock stretching, then the answer.

```fides
pub fn sht3x_measure(s: &mut SensorBus) -> Result<Vec<i64>, str> {
```

**Parameters:**

- `s`: `&mut SensorBus`

**Returns:** `Result<Vec<i64>, str>`

*Defined at line 606*

---

## BH1750_ADDR:

```fides
pub const BH1750_ADDR: i64 = 0x23;
```

*Defined at line 622*

---

## BH1750_ADDR_ALT:

```fides
pub const BH1750_ADDR_ALT: i64 = 0x5C;
```

*Defined at line 623*

---

## BH1750_POWER_DOWN:

```fides
pub const BH1750_POWER_DOWN: i64 = 0x00;
```

*Defined at line 624*

---

## BH1750_POWER_ON:

```fides
pub const BH1750_POWER_ON: i64 = 0x01;
```

*Defined at line 625*

---

## BH1750_RESET:

```fides
pub const BH1750_RESET: i64 = 0x07;
```

*Defined at line 626*

---

## BH1750_CONT_H:

```fides
pub const BH1750_CONT_H: i64 = 0x10;
```

*Defined at line 627*

---

## BH1750_CONT_H2:

```fides
pub const BH1750_CONT_H2: i64 = 0x11;
```

*Defined at line 628*

---

## BH1750_CONT_L:

```fides
pub const BH1750_CONT_L: i64 = 0x13;
```

*Defined at line 629*

---

## BH1750_ONE_H:

```fides
pub const BH1750_ONE_H: i64 = 0x20;
```

*Defined at line 630*

---

## BH1750_ONE_H2:

```fides
pub const BH1750_ONE_H2: i64 = 0x21;
```

*Defined at line 631*

---

## BH1750_ONE_L:

```fides
pub const BH1750_ONE_L: i64 = 0x23;
```

*Defined at line 632*

---

## BH1750_MTREG_DEFAULT:

```fides
pub const BH1750_MTREG_DEFAULT: i64 = 69;
```

*Defined at line 633*

---

## bh1750_mtreg_commands

The two commands that set the measurement time register (31..254).

```fides
pub fn bh1750_mtreg_commands(mt: i64) -> Vec<i64> {
```

**Parameters:**

- `mt`: `i64`

**Returns:** `Vec<i64>`

*Defined at line 636*

---

## bh1750_lux100

Illuminance in 0.01 lx: raw / 1.2 * (69 / mtreg), halved in the
H-resolution mode 2.

```fides
pub fn bh1750_lux100(raw: i64, mtreg: i64, mode: i64) -> i64 {
```

**Parameters:**

- `raw`: `i64`
- `mtreg`: `i64`
- `mode`: `i64`

**Returns:** `i64`

*Defined at line 645*

---

## bh1750_read_lux100

```fides
pub fn bh1750_read_lux100(s: &mut SensorBus, mtreg: i64, mode: i64) -> Result<i64, str> {
```

**Parameters:**

- `s`: `&mut SensorBus`
- `mtreg`: `i64`
- `mode`: `i64`

**Returns:** `Result<i64, str>`

*Defined at line 653*

---

## INA219_ADDR:

```fides
pub const INA219_ADDR: i64 = 0x40;
```

*Defined at line 666*

---

## INA219_REG_CONFIG:

```fides
pub const INA219_REG_CONFIG: i64 = 0x00;
```

*Defined at line 667*

---

## INA219_REG_SHUNT:

```fides
pub const INA219_REG_SHUNT: i64 = 0x01;
```

*Defined at line 668*

---

## INA219_REG_BUS:

```fides
pub const INA219_REG_BUS: i64 = 0x02;
```

*Defined at line 669*

---

## INA219_REG_POWER:

```fides
pub const INA219_REG_POWER: i64 = 0x03;
```

*Defined at line 670*

---

## INA219_REG_CURRENT:

```fides
pub const INA219_REG_CURRENT: i64 = 0x04;
```

*Defined at line 671*

---

## INA219_REG_CAL:

```fides
pub const INA219_REG_CAL: i64 = 0x05;
```

*Defined at line 672*

---

## INA219_CONFIG_DEFAULT:

```fides
pub const INA219_CONFIG_DEFAULT: i64 = 0x399F;
```

*Defined at line 673*

---

## ina219_config

Configuration word: bus range 16 or 32 V, PGA gain 1/2/4/8 (40, 80,
160, 320 mV), ADC settings (4-bit codes), mode (0..7).

```fides
pub fn ina219_config(brng_32v: bool, pga: i64, badc: i64, sadc: i64, mode: i64) -> i64 {
```

**Parameters:**

- `brng_32v`: `bool`
- `pga`: `i64`
- `badc`: `i64`
- `sadc`: `i64`
- `mode`: `i64`

**Returns:** `i64`

*Defined at line 677*

---

## ina219_calibration

Calibration register: trunc(0.04096 / (current_lsb * r_shunt)), with
current LSB in uA and shunt in milliohms (integers, no rounding drift).

```fides
pub fn ina219_calibration(current_lsb_ua: i64, shunt_mohm: i64) -> i64 {
```

**Parameters:**

- `current_lsb_ua`: `i64`
- `shunt_mohm`: `i64`

**Returns:** `i64`

*Defined at line 695*

---

## ina219_shunt_uv

Shunt voltage in uV (signed register, 10 uV per bit).

```fides
pub fn ina219_shunt_uv(reg: i64) -> i64 {
```

**Parameters:**

- `reg`: `i64`

**Returns:** `i64`

*Defined at line 700*

---

## ina219_bus_mv

Bus voltage in mV (bits 15..3, 4 mV per bit).

```fides
pub fn ina219_bus_mv(reg: i64) -> i64 {
```

**Parameters:**

- `reg`: `i64`

**Returns:** `i64`

*Defined at line 709*

---

## ina219_conversion_ready

```fides
pub fn ina219_conversion_ready(reg: i64) -> bool {
```

**Parameters:**

- `reg`: `i64`

**Returns:** `bool`

*Defined at line 713*

---

## ina219_overflow

```fides
pub fn ina219_overflow(reg: i64) -> bool {
```

**Parameters:**

- `reg`: `i64`

**Returns:** `bool`

*Defined at line 717*

---

## ina219_current_ua

Current in uA from the current register.

```fides
pub fn ina219_current_ua(reg: i64, current_lsb_ua: i64) -> i64 {
```

**Parameters:**

- `reg`: `i64`
- `current_lsb_ua`: `i64`

**Returns:** `i64`

*Defined at line 722*

---

## ina219_power_uw

Power in uW from the power register (power LSB = 20 x current LSB).

```fides
pub fn ina219_power_uw(reg: i64, current_lsb_ua: i64) -> i64 {
```

**Parameters:**

- `reg`: `i64`
- `current_lsb_ua`: `i64`

**Returns:** `i64`

*Defined at line 731*

---

## ina219_current_reg

What the device computes: the current register for a shunt register.

```fides
pub fn ina219_current_reg(shunt_reg: i64, cal: i64) -> i64 {
```

**Parameters:**

- `shunt_reg`: `i64`
- `cal`: `i64`

**Returns:** `i64`

*Defined at line 736*

---

## ina219_power_reg

And the power register from current and bus registers.

```fides
pub fn ina219_power_reg(current_reg: i64, bus_reg: i64) -> i64 {
```

**Parameters:**

- `current_reg`: `i64`
- `bus_reg`: `i64`

**Returns:** `i64`

*Defined at line 745*

---

## Ina219

```fides
pub struct Ina219 {
```

**Fields:**

| Name | Type |
|------|------|
| `link` | `SensorBus` |
| `current_lsb_ua` | `i64` |
| `cal` | `i64` |

*Defined at line 749*

---

## ina219_init

```fides
pub fn ina219_init(link: SensorBus, current_lsb_ua: i64, shunt_mohm: i64, config: i64) -> Result<Ina219, str> {
```

**Parameters:**

- `link`: `SensorBus`
- `current_lsb_ua`: `i64`
- `shunt_mohm`: `i64`
- `config`: `i64`

**Returns:** `Result<Ina219, str>`

*Defined at line 763*

---

## ina219_read

[bus mV, shunt uV, current uA, power uW].

```fides
pub fn ina219_read(dev: &mut Ina219) -> Vec<i64> {
```

**Parameters:**

- `dev`: `&mut Ina219`

**Returns:** `Vec<i64>`

*Defined at line 779*

---

*Generated by [scribe](https://fides.dev) -- Fides documentation generator*
