# medical — Fides Documentation

## Contents

- [HL7_ADT_A01:](#hl7_adt_a01:) (const)
- [HL7_ADT_A02:](#hl7_adt_a02:) (const)
- [HL7_ADT_A03:](#hl7_adt_a03:) (const)
- [HL7_ADT_A04:](#hl7_adt_a04:) (const)
- [HL7_ADT_A08:](#hl7_adt_a08:) (const)
- [HL7_ORU_R01:](#hl7_oru_r01:) (const)
- [HL7_OUL_R22:](#hl7_oul_r22:) (const)
- [HL7_MDM_T02:](#hl7_mdm_t02:) (const)
- [hl7_msg_type_code](#hl7_msg_type_code) (function)
- [hl7_msg_type_name](#hl7_msg_type_name) (function)
- [HL7Message](#hl7message) (struct)
- [hl7_new](#hl7_new) (function)
- [hl7_set_receiver](#hl7_set_receiver) (function)
- [hl7_add_segment](#hl7_add_segment) (function)
- [hl7_segment_count](#hl7_segment_count) (function)
- [hl7_msh_segment](#hl7_msh_segment) (function)
- [hl7_pid_segment](#hl7_pid_segment) (function)
- [hl7_pv1_segment](#hl7_pv1_segment) (function)
- [hl7_obx_segment](#hl7_obx_segment) (function)
- [hl7_adt_a01](#hl7_adt_a01) (function)
- [hl7_adt_a03](#hl7_adt_a03) (function)
- [hl7_serialize](#hl7_serialize) (function)
- [hl7_parse](#hl7_parse) (function)
- [hl7_get_field](#hl7_get_field) (function)
- [hl7_segment_type](#hl7_segment_type) (function)
- [FHIR_PATIENT:](#fhir_patient:) (const)
- [FHIR_OBSERVATION:](#fhir_observation:) (const)
- [FHIR_CONDITION:](#fhir_condition:) (const)
- [FHIR_MEDICATION:](#fhir_medication:) (const)
- [FHIR_ENCOUNTER:](#fhir_encounter:) (const)
- [FHIR_PRACTITIONER:](#fhir_practitioner:) (const)
- [fhir_resource_name](#fhir_resource_name) (function)
- [icd10_validate](#icd10_validate) (function)
- [LOINC_GLUCOSE:](#loinc_glucose:) (const)
- [LOINC_HEMOGLOBIN_A1C:](#loinc_hemoglobin_a1c:) (const)
- [LOINC_CHOLESTEROL:](#loinc_cholesterol:) (const)
- [LOINC_CREATININE:](#loinc_creatinine:) (const)
- [LOINC_WBC:](#loinc_wbc:) (const)
- [LOINC_RBC:](#loinc_rbc:) (const)
- [LOINC_PLATELETS:](#loinc_platelets:) (const)
- [LOINC_SODIUM:](#loinc_sodium:) (const)
- [LOINC_POTASSIUM:](#loinc_potassium:) (const)
- [LOINC_TEMPERATURE:](#loinc_temperature:) (const)
- [LOINC_HEART_RATE:](#loinc_heart_rate:) (const)
- [LOINC_BP_SYSTOLIC:](#loinc_bp_systolic:) (const)
- [LOINC_BP_DIASTOLIC:](#loinc_bp_diastolic:) (const)
- [LOINC_SPO2:](#loinc_spo2:) (const)

## HL7_ADT_A01:

```fides
pub const HL7_ADT_A01: i64 = 1;  // Admit a patient
```

*Defined at line 20*

---

## HL7_ADT_A02:

```fides
pub const HL7_ADT_A02: i64 = 2;  // Transfer a patient
```

*Defined at line 21*

---

## HL7_ADT_A03:

```fides
pub const HL7_ADT_A03: i64 = 3;  // Discharge a patient
```

*Defined at line 22*

---

## HL7_ADT_A04:

```fides
pub const HL7_ADT_A04: i64 = 4;  // Register a patient
```

*Defined at line 23*

---

## HL7_ADT_A08:

```fides
pub const HL7_ADT_A08: i64 = 8;  // Update patient information
```

*Defined at line 24*

---

## HL7_ORU_R01:

```fides
pub const HL7_ORU_R01: i64 = 10; // Observation result
```

*Defined at line 25*

---

## HL7_OUL_R22:

```fides
pub const HL7_OUL_R22: i64 = 11; // Unsolicited lab observation
```

*Defined at line 26*

---

## HL7_MDM_T02:

```fides
pub const HL7_MDM_T02: i64 = 12; // Medical document notification
```

*Defined at line 27*

---

## hl7_msg_type_code

Get HL7 message type code.

```fides
pub fn hl7_msg_type_code(msg_type: i64) -> str {
```

**Parameters:**

- `msg_type`: `i64`

**Returns:** `str`

*Defined at line 30*

---

## hl7_msg_type_name

Get HL7 message type name.

```fides
pub fn hl7_msg_type_name(msg_type: i64) -> str {
```

**Parameters:**

- `msg_type`: `i64`

**Returns:** `str`

*Defined at line 43*

---

## HL7Message

An HL7 v2 message (pipe-delimited segments).

```fides
pub struct HL7Message {
```

**Fields:**

| Name | Type |
|------|------|
| `msg_type` | `i64` |
| `msg_id` | `str` |
| `sending_app` | `str` |
| `sending_facility` | `str` |
| `receiving_app` | `str` |
| `receiving_facility` | `str` |
| `timestamp` | `str,    // YYYYMMDDHHMMSS` |
| `segments` | `Vec,     // Vec of segment strings` |

*Defined at line 58*

---

## hl7_new

Create a new HL7 message.

```fides
pub fn hl7_new(msg_type: i64, msg_id: str, sending_app: str,
```

**Parameters:**

- `msg_type`: `i64`
- `msg_id`: `str`
- `sending_app`: `str`

*Defined at line 70*

---

## hl7_set_receiver

Set receiving application and facility.

```fides
pub fn hl7_set_receiver(msg: HL7Message, app: str, facility: str) -> void {
```

**Parameters:**

- `msg`: `HL7Message`
- `app`: `str`
- `facility`: `str`

**Returns:** `void`

*Defined at line 85*

---

## hl7_add_segment

Add a segment to the message.

```fides
pub fn hl7_add_segment(msg: HL7Message, segment: str) -> void {
```

**Parameters:**

- `msg`: `HL7Message`
- `segment`: `str`

**Returns:** `void`

*Defined at line 91*

---

## hl7_segment_count

Get segment count.

```fides
pub fn hl7_segment_count(msg: HL7Message) -> i64 {
```

**Parameters:**

- `msg`: `HL7Message`

**Returns:** `i64`

*Defined at line 96*

---

## hl7_msh_segment

Build MSH (Message Header) segment.

```fides
pub fn hl7_msh_segment(msg: HL7Message) -> str {
```

**Parameters:**

- `msg`: `HL7Message`

**Returns:** `str`

*Defined at line 103*

---

## hl7_pid_segment

Build PID (Patient Identification) segment.

```fides
pub fn hl7_pid_segment(patient_id: str, patient_name: str,
```

**Parameters:**

- `patient_id`: `str`
- `patient_name`: `str`

*Defined at line 113*

---

## hl7_pv1_segment

Build PV1 (Patient Visit) segment.

```fides
pub fn hl7_pv1_segment(patient_class: str, assigned_location: str,
```

**Parameters:**

- `patient_class`: `str`
- `assigned_location`: `str`

*Defined at line 121*

---

## hl7_obx_segment

Build OBX (Observation Result) segment.

```fides
pub fn hl7_obx_segment(set_id: i64, value_type: str, observation_id: str,
```

**Parameters:**

- `set_id`: `i64`
- `value_type`: `str`
- `observation_id`: `str`

*Defined at line 129*

---

## hl7_adt_a01

Build an ADT^A01 (Admit Patient) message.

```fides
pub fn hl7_adt_a01(sending_facility: str, patient_id: str,
```

**Parameters:**

- `sending_facility`: `str`
- `patient_id`: `str`

*Defined at line 139*

---

## hl7_adt_a03

Build an ADT^A03 (Discharge Patient) message.

```fides
pub fn hl7_adt_a03(sending_facility: str, patient_id: str) -> HL7Message {
```

**Parameters:**

- `sending_facility`: `str`
- `patient_id`: `str`

**Returns:** `HL7Message`

*Defined at line 155*

---

## hl7_serialize

Serialize an HL7 message to text (pipe-delimited, newline-separated).

```fides
pub fn hl7_serialize(msg: HL7Message) -> str {
```

**Parameters:**

- `msg`: `HL7Message`

**Returns:** `str`

*Defined at line 168*

---

## hl7_parse

Parse an HL7 message from text. Returns an HL7Message.
Segments are split by CR or LF.

```fides
pub fn hl7_parse(text: str) -> HL7Message {
```

**Parameters:**

- `text`: `str`

**Returns:** `HL7Message`

*Defined at line 183*

---

## hl7_get_field

Extract a field from a segment (pipe-separated).

```fides
pub fn hl7_get_field(segment: str, field_num: i64) -> str {
```

**Parameters:**

- `segment`: `str`
- `field_num`: `i64`

**Returns:** `str`

*Defined at line 216*

---

## hl7_segment_type

Get the segment type (first 3 characters).

```fides
pub fn hl7_segment_type(segment: str) -> str {
```

**Parameters:**

- `segment`: `str`

**Returns:** `str`

*Defined at line 225*

---

## FHIR_PATIENT:

```fides
pub const FHIR_PATIENT: i64    = 0;
```

*Defined at line 231*

---

## FHIR_OBSERVATION:

```fides
pub const FHIR_OBSERVATION: i64 = 1;
```

*Defined at line 232*

---

## FHIR_CONDITION:

```fides
pub const FHIR_CONDITION: i64  = 2;
```

*Defined at line 233*

---

## FHIR_MEDICATION:

```fides
pub const FHIR_MEDICATION: i64 = 3;
```

*Defined at line 234*

---

## FHIR_ENCOUNTER:

```fides
pub const FHIR_ENCOUNTER: i64  = 4;
```

*Defined at line 235*

---

## FHIR_PRACTITIONER:

```fides
pub const FHIR_PRACTITIONER: i64 = 5;
```

*Defined at line 236*

---

## fhir_resource_name

Get FHIR resource type name.

```fides
pub fn fhir_resource_name(resource_type: i64) -> str {
```

**Parameters:**

- `resource_type`: `i64`

**Returns:** `str`

*Defined at line 239*

---

## icd10_validate

Validate ICD-10 code format (e.g. "E11.9", "I10", "J45.909").

```fides
pub fn icd10_validate(code: str) -> bool {
```

**Parameters:**

- `code`: `str`

**Returns:** `bool`

*Defined at line 252*

---

## LOINC_GLUCOSE:

```fides
pub const LOINC_GLUCOSE: str       = "2339-0";  // Glucose
```

*Defined at line 273*

---

## LOINC_HEMOGLOBIN_A1C:

```fides
pub const LOINC_HEMOGLOBIN_A1C: str = "4548-4"; // HbA1c
```

*Defined at line 274*

---

## LOINC_CHOLESTEROL:

```fides
pub const LOINC_CHOLESTEROL: str    = "2093-3";  // Cholesterol
```

*Defined at line 275*

---

## LOINC_CREATININE:

```fides
pub const LOINC_CREATININE: str     = "2160-0";  // Creatinine
```

*Defined at line 276*

---

## LOINC_WBC:

```fides
pub const LOINC_WBC: str            = "6690-2";  // White Blood Cell
```

*Defined at line 277*

---

## LOINC_RBC:

```fides
pub const LOINC_RBC: str            = "789-8";   // Red Blood Cell
```

*Defined at line 278*

---

## LOINC_PLATELETS:

```fides
pub const LOINC_PLATELETS: str      = "777-3";   // Platelets
```

*Defined at line 279*

---

## LOINC_SODIUM:

```fides
pub const LOINC_SODIUM: str         = "2951-2";  // Sodium
```

*Defined at line 280*

---

## LOINC_POTASSIUM:

```fides
pub const LOINC_POTASSIUM: str      = "2823-3";  // Potassium
```

*Defined at line 281*

---

## LOINC_TEMPERATURE:

```fides
pub const LOINC_TEMPERATURE: str    = "8310-5";  // Body Temperature
```

*Defined at line 282*

---

## LOINC_HEART_RATE:

```fides
pub const LOINC_HEART_RATE: str     = "8867-4";  // Heart Rate
```

*Defined at line 283*

---

## LOINC_BP_SYSTOLIC:

```fides
pub const LOINC_BP_SYSTOLIC: str    = "8480-6";  // BP Systolic
```

*Defined at line 284*

---

## LOINC_BP_DIASTOLIC:

```fides
pub const LOINC_BP_DIASTOLIC: str   = "8462-4";  // BP Diastolic
```

*Defined at line 285*

---

## LOINC_SPO2:

```fides
pub const LOINC_SPO2: str           = "59408-5"; // Oxygen Saturation
```

*Defined at line 286*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
