# clinical — Fides Documentation

## Contents

- [clinical_bmi](#clinical_bmi) (function)
- [clinical_bmi_category](#clinical_bmi_category) (function)
- [clinical_bmi_imperial](#clinical_bmi_imperial) (function)
- [clinical_gfr_egfr](#clinical_gfr_egfr) (function)
- [clinical_ckd_stage](#clinical_ckd_stage) (function)
- [clinical_chads2vasc](#clinical_chads2vasc) (function)
- [clinical_chads2vasc_risk](#clinical_chads2vasc_risk) (function)
- [clinical_meld](#clinical_meld) (function)
- [clinical_dose_by_weight](#clinical_dose_by_weight) (function)
- [clinical_dose_young](#clinical_dose_young) (function)
- [clinical_dose_clark](#clinical_dose_clark) (function)
- [clinical_creatinine_clearance](#clinical_creatinine_clearance) (function)
- [clinical_c_to_f](#clinical_c_to_f) (function)
- [clinical_f_to_c](#clinical_f_to_c) (function)
- [clinical_max_hr](#clinical_max_hr) (function)
- [clinical_target_hr_zone](#clinical_target_hr_zone) (function)
- [clinical_anion_gap](#clinical_anion_gap) (function)
- [clinical_anion_gap_normal](#clinical_anion_gap_normal) (function)
- [clinical_news2](#clinical_news2) (function)
- [clinical_news2_category](#clinical_news2_category) (function)

## clinical_bmi

Calculate BMI from weight in kg and height in meters.

```fides
pub fn clinical_bmi(weight_kg: f64, height_m: f64) -> f64 {
```

**Parameters:**

- `weight_kg`: `f64`
- `height_m`: `f64`

**Returns:** `f64`

*Defined at line 33*

---

## clinical_bmi_category

Get BMI category string.

```fides
pub fn clinical_bmi_category(bmi: f64) -> str {
```

**Parameters:**

- `bmi`: `f64`

**Returns:** `str`

*Defined at line 42*

---

## clinical_bmi_imperial

Calculate BMI from weight in pounds and height in inches.

```fides
pub fn clinical_bmi_imperial(weight_lb: f64, height_in: f64) -> f64 {
```

**Parameters:**

- `weight_lb`: `f64`
- `height_in`: `f64`

**Returns:** `f64`

*Defined at line 51*

---

## clinical_gfr_egfr

Calculate eGFR using the simplified CKD-EPI 2021 formula.
serum_creatinine: in mg/dL (e.g. 1.0 = normal)
age: in years
is_female: 1 = female, 0 = male
Returns eGFR in mL/min/1.73m²

```fides
pub fn clinical_gfr_egfr(serum_creatinine: f64, age: i64, is_female: i64) -> f64 {
```

**Parameters:**

- `serum_creatinine`: `f64`
- `age`: `i64`
- `is_female`: `i64`

**Returns:** `f64`

*Defined at line 72*

---

## clinical_ckd_stage

Get CKD stage from eGFR value.
Stage 1: >=90, Stage 2: 60-89, Stage 3a: 45-59, Stage 3b: 30-44,
Stage 4: 15-29, Stage 5: <15

```fides
pub fn clinical_ckd_stage(egfr: f64) -> i64 {
```

**Parameters:**

- `egfr`: `f64`

**Returns:** `i64`

*Defined at line 102*

---

## clinical_chads2vasc

Calculate CHA₂DS₂-VASc score.
Parameters are 0 or 1 (presence of condition), age in years, is_female.

```fides
pub fn clinical_chads2vasc(chf: i64, htn: i64, age: i64, diabetes: i64,
```

**Parameters:**

- `chf`: `i64`
- `htn`: `i64`
- `age`: `i64`
- `diabetes`: `i64`

*Defined at line 120*

---

## clinical_chads2vasc_risk

Get stroke risk category from CHA₂DS₂-VASc score.

```fides
pub fn clinical_chads2vasc_risk(score: i64) -> str {
```

**Parameters:**

- `score`: `i64`

**Returns:** `str`

*Defined at line 143*

---

## clinical_meld

Calculate MELD score (simplified, without sodium adjustment).
creatinine: mg/dL, bilirubin: mg/dL, inr: INR value

```fides
pub fn clinical_meld(creatinine: f64, bilirubin: f64, inr: f64) -> f64 {
```

**Parameters:**

- `creatinine`: `f64`
- `bilirubin`: `f64`
- `inr`: `f64`

**Returns:** `f64`

*Defined at line 158*

---

## clinical_dose_by_weight

Calculate drug dosage based on weight.
dose_mg_per_kg: mg of drug per kg of body weight
weight_kg: patient weight in kg
Returns total dose in mg

```fides
pub fn clinical_dose_by_weight(dose_mg_per_kg: f64, weight_kg: f64) -> f64 {
```

**Parameters:**

- `dose_mg_per_kg`: `f64`
- `weight_kg`: `f64`

**Returns:** `f64`

*Defined at line 188*

---

## clinical_dose_young

Calculate pediatric dose using Young's rule:
dose = (adult_dose × age) / (age + 12)

```fides
pub fn clinical_dose_young(adult_dose: f64, age_years: i64) -> f64 {
```

**Parameters:**

- `adult_dose`: `f64`
- `age_years`: `i64`

**Returns:** `f64`

*Defined at line 195*

---

## clinical_dose_clark

Calculate pediatric dose using Clark's rule:
dose = (adult_dose × weight_lb) / 150

```fides
pub fn clinical_dose_clark(adult_dose: f64, weight_lb: f64) -> f64 {
```

**Parameters:**

- `adult_dose`: `f64`
- `weight_lb`: `f64`

**Returns:** `f64`

*Defined at line 206*

---

## clinical_creatinine_clearance

Calculate creatinine clearance (Cockcroft-Gault equation).
Used for drug dosing in patients with renal impairment.
CrCl = ((140 - age) × weight_kg) / (72 × serum_creatinine)
For females: multiply by 0.85

```fides
pub fn clinical_creatinine_clearance(age: i64, weight_kg: f64,
```

**Parameters:**

- `age`: `i64`
- `weight_kg`: `f64`

*Defined at line 215*

---

## clinical_c_to_f

Convert Celsius to Fahrenheit.

```fides
pub fn clinical_c_to_f(celsius: f64) -> f64 {
```

**Parameters:**

- `celsius`: `f64`

**Returns:** `f64`

*Defined at line 230*

---

## clinical_f_to_c

Convert Fahrenheit to Celsius.

```fides
pub fn clinical_f_to_c(fahrenheit: f64) -> f64 {
```

**Parameters:**

- `fahrenheit`: `f64`

**Returns:** `f64`

*Defined at line 235*

---

## clinical_max_hr

Calculate maximum heart rate (Fox formula: 220 - age).

```fides
pub fn clinical_max_hr(age: i64) -> i64 {
```

**Parameters:**

- `age`: `i64`

**Returns:** `i64`

*Defined at line 242*

---

## clinical_target_hr_zone

Calculate target heart rate zone (50-85% of max HR).
Returns (lower, upper) as a Vec [lower, upper].

```fides
pub fn clinical_target_hr_zone(age: i64) -> Vec {
```

**Parameters:**

- `age`: `i64`

**Returns:** `Vec`

*Defined at line 248*

---

## clinical_anion_gap

Calculate anion gap: Na - (Cl + HCO3)
Normal range: 8-16 mEq/L

```fides
pub fn clinical_anion_gap(sodium: i64, chloride: i64, bicarbonate: i64) -> i64 {
```

**Parameters:**

- `sodium`: `i64`
- `chloride`: `i64`
- `bicarbonate`: `i64`

**Returns:** `i64`

*Defined at line 260*

---

## clinical_anion_gap_normal

Check if anion gap is normal (8-16).

```fides
pub fn clinical_anion_gap_normal(gap: i64) -> bool {
```

**Parameters:**

- `gap`: `i64`

**Returns:** `bool`

*Defined at line 265*

---

## clinical_news2

Calculate NEWS2 score from vital signs.
resp_rate: breaths/min, spo2: oxygen saturation %, air_or_o2: 0=air 1=oxygen
systolic_bp: mmHg, pulse: bpm, temp_c: temperature in Celsius

```fides
pub fn clinical_news2(resp_rate: i64, spo2: i64, air_or_o2: i64,
```

**Parameters:**

- `resp_rate`: `i64`
- `spo2`: `i64`
- `air_or_o2`: `i64`

*Defined at line 278*

---

## clinical_news2_category

Get NEWS2 urgency category.

```fides
pub fn clinical_news2_category(score: i64) -> str {
```

**Parameters:**

- `score`: `i64`

**Returns:** `str`

*Defined at line 325*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
