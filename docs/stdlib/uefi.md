# uefi — Fides Documentation

## Contents

- [EFI_SUCCESS:](#efi_success:) (const)
- [EFI_LOAD_ERROR:](#efi_load_error:) (const)
- [EFI_INVALID_PARAMETER:](#efi_invalid_parameter:) (const)
- [EFI_UNSUPPORTED:](#efi_unsupported:) (const)
- [EFI_BAD_BUFFER_SIZE:](#efi_bad_buffer_size:) (const)
- [EFI_BUFFER_TOO_SMALL:](#efi_buffer_too_small:) (const)
- [EFI_NOT_READY:](#efi_not_ready:) (const)
- [EFI_OUT_OF_RESOURCES:](#efi_out_of_resources:) (const)
- [EFI_NOT_FOUND:](#efi_not_found:) (const)
- [EFI_ACCESS_DENIED:](#efi_access_denied:) (const)
- [EFI_TIMEOUT:](#efi_timeout:) (const)
- [efi_reserved_memory:](#efi_reserved_memory:) (const)
- [efi_loader_code:](#efi_loader_code:) (const)
- [efi_loader_data:](#efi_loader_data:) (const)
- [efi_boot_services_code:](#efi_boot_services_code:) (const)
- [efi_boot_services_data:](#efi_boot_services_data:) (const)
- [efi_runtime_services_code:](#efi_runtime_services_code:) (const)
- [efi_runtime_services_data:](#efi_runtime_services_data:) (const)
- [efi_conventional_memory:](#efi_conventional_memory:) (const)
- [efi_unusable_memory:](#efi_unusable_memory:) (const)
- [efi_acpi_reclaim_memory:](#efi_acpi_reclaim_memory:) (const)
- [efi_acpi_nvs_memory:](#efi_acpi_nvs_memory:) (const)
- [efi_memory_mapped_io:](#efi_memory_mapped_io:) (const)
- [efi_memory_mapped_io_port_space:](#efi_memory_mapped_io_port_space:) (const)
- [efi_pal_code:](#efi_pal_code:) (const)
- [efi_persistent_memory:](#efi_persistent_memory:) (const)
- [uefi_pool_data:](#uefi_pool_data:) (const)
- [uefi_pool_realloc:](#uefi_pool_realloc:) (const)
- [uefi_pool_align:](#uefi_pool_align:) (const)
- [efi_timer_relative:](#efi_timer_relative:) (const)
- [efi_timer_periodic:](#efi_timer_periodic:) (const)
- [efi_timer_cancel:](#efi_timer_cancel:) (const)
- [efi_handle_is_valid](#efi_handle_is_valid) (function)
- [efi_bs_hdr_size:](#efi_bs_hdr_size:) (const)
- [efi_bs_raise_tpl:](#efi_bs_raise_tpl:) (const)
- [efi_bs_restore_tpl:](#efi_bs_restore_tpl:) (const)
- [efi_bs_allocate_pages:](#efi_bs_allocate_pages:) (const)
- [efi_bs_free_pages:](#efi_bs_free_pages:) (const)
- [efi_bs_get_memory_map:](#efi_bs_get_memory_map:) (const)
- [efi_bs_allocate_pool:](#efi_bs_allocate_pool:) (const)
- [efi_bs_free_pool:](#efi_bs_free_pool:) (const)
- [efi_bs_create_event:](#efi_bs_create_event:) (const)
- [efi_bs_set_timer:](#efi_bs_set_timer:) (const)
- [efi_bs_wait_for_event:](#efi_bs_wait_for_event:) (const)
- [efi_bs_signal_event:](#efi_bs_signal_event:) (const)
- [efi_bs_close_event:](#efi_bs_close_event:) (const)
- [efi_bs_check_event:](#efi_bs_check_event:) (const)
- [efi_bs_install_protocol_interface:](#efi_bs_install_protocol_interface:) (const)
- [efi_bs_reinstall_protocol_interface:](#efi_bs_reinstall_protocol_interface:) (const)
- [efi_bs_uninstall_protocol_interface:](#efi_bs_uninstall_protocol_interface:) (const)
- [efi_bs_handle_protocol:](#efi_bs_handle_protocol:) (const)
- [efi_bs_register_protocol_notify:](#efi_bs_register_protocol_notify:) (const)
- [efi_bs_locate_handle:](#efi_bs_locate_handle:) (const)
- [efi_bs_locate_protocol:](#efi_bs_locate_protocol:) (const)
- [efi_bs_install_configuration_table:](#efi_bs_install_configuration_table:) (const)
- [efi_bs_load_image:](#efi_bs_load_image:) (const)
- [efi_bs_start_image:](#efi_bs_start_image:) (const)
- [efi_bs_exit:](#efi_bs_exit:) (const)
- [efi_bs_unload_image:](#efi_bs_unload_image:) (const)
- [efi_bs_exit_boot_services:](#efi_bs_exit_boot_services:) (const)
- [efi_bs_get_next_monotonic_count:](#efi_bs_get_next_monotonic_count:) (const)
- [efi_bs_stall:](#efi_bs_stall:) (const)
- [efi_bs_set_watchdog_timer:](#efi_bs_set_watchdog_timer:) (const)
- [efi_bs_connect_controller:](#efi_bs_connect_controller:) (const)
- [efi_bs_disconnect_controller:](#efi_bs_disconnect_controller:) (const)
- [efi_bs_open_protocol:](#efi_bs_open_protocol:) (const)
- [efi_bs_close_protocol:](#efi_bs_close_protocol:) (const)
- [efi_bs_open_protocol_information:](#efi_bs_open_protocol_information:) (const)
- [efi_bs_protocols_per_handle:](#efi_bs_protocols_per_handle:) (const)
- [efi_bs_locate_handle_buffer:](#efi_bs_locate_handle_buffer:) (const)
- [efi_bs_locate_all_handles:](#efi_bs_locate_all_handles:) (const)
- [efi_bs_copy_mem:](#efi_bs_copy_mem:) (const)
- [efi_bs_set_mem:](#efi_bs_set_mem:) (const)
- [efi_bs_create_event_ex:](#efi_bs_create_event_ex:) (const)
- [efi_rs_hdr_size:](#efi_rs_hdr_size:) (const)
- [efi_rs_get_time:](#efi_rs_get_time:) (const)
- [efi_rs_set_time:](#efi_rs_set_time:) (const)
- [efi_rs_get_wakeup_time:](#efi_rs_get_wakeup_time:) (const)
- [efi_rs_set_wakeup_time:](#efi_rs_set_wakeup_time:) (const)
- [efi_rs_set_virtual_address_map:](#efi_rs_set_virtual_address_map:) (const)
- [efi_rs_convert_pointer:](#efi_rs_convert_pointer:) (const)
- [efi_rs_get_variable:](#efi_rs_get_variable:) (const)
- [efi_rs_set_variable:](#efi_rs_set_variable:) (const)
- [efi_rs_get_next_variable_name:](#efi_rs_get_next_variable_name:) (const)
- [efi_rs_get_next_high_monotonic_count:](#efi_rs_get_next_high_monotonic_count:) (const)
- [efi_rs_reset_system:](#efi_rs_reset_system:) (const)
- [efi_st_hdr_size:](#efi_st_hdr_size:) (const)
- [efi_st_firmware_vendor:](#efi_st_firmware_vendor:) (const)
- [efi_st_firmware_revision:](#efi_st_firmware_revision:) (const)
- [efi_st_console_in_handle:](#efi_st_console_in_handle:) (const)
- [efi_st_con_in:](#efi_st_con_in:) (const)
- [efi_st_console_out_handle:](#efi_st_console_out_handle:) (const)
- [efi_st_con_out:](#efi_st_con_out:) (const)
- [efi_st_standard_error_handle:](#efi_st_standard_error_handle:) (const)
- [efi_st_std_err:](#efi_st_std_err:) (const)
- [efi_st_runtime_services:](#efi_st_runtime_services:) (const)
- [efi_st_boot_services:](#efi_st_boot_services:) (const)
- [efi_st_number_of_table_entries:](#efi_st_number_of_table_entries:) (const)
- [efi_st_configuration_table:](#efi_st_configuration_table:) (const)
- [uefi_init](#uefi_init) (function)
- [uefi_get_boot_services](#uefi_get_boot_services) (function)
- [uefi_get_runtime_services](#uefi_get_runtime_services) (function)
- [uefi_get_con_out](#uefi_get_con_out) (function)
- [uefi_get_con_in](#uefi_get_con_in) (function)
- [efi_read_ptr](#efi_read_ptr) (function)
- [efi_write_ptr](#efi_write_ptr) (function)
- [uefi_allocate_pool](#uefi_allocate_pool) (function)
- [uefi_free_pool](#uefi_free_pool) (function)
- [uefi_allocate_pages](#uefi_allocate_pages) (function)
- [uefi_get_memory_map](#uefi_get_memory_map) (function)
- [uefi_exit_boot_services](#uefi_exit_boot_services) (function)
- [uefi_stall](#uefi_stall) (function)
- [uefi_print](#uefi_print) (function)
- [uefi_clear_screen](#uefi_clear_screen) (function)

## EFI_SUCCESS:

```fides
pub const EFI_SUCCESS: i64 = 0;
```

*Defined at line 25*

---

## EFI_LOAD_ERROR:

```fides
pub const EFI_LOAD_ERROR: i64 = 1;
```

*Defined at line 26*

---

## EFI_INVALID_PARAMETER:

```fides
pub const EFI_INVALID_PARAMETER: i64 = 2;
```

*Defined at line 27*

---

## EFI_UNSUPPORTED:

```fides
pub const EFI_UNSUPPORTED: i64 = 3;
```

*Defined at line 28*

---

## EFI_BAD_BUFFER_SIZE:

```fides
pub const EFI_BAD_BUFFER_SIZE: i64 = 4;
```

*Defined at line 29*

---

## EFI_BUFFER_TOO_SMALL:

```fides
pub const EFI_BUFFER_TOO_SMALL: i64 = 5;
```

*Defined at line 30*

---

## EFI_NOT_READY:

```fides
pub const EFI_NOT_READY: i64 = 6;
```

*Defined at line 31*

---

## EFI_OUT_OF_RESOURCES:

```fides
pub const EFI_OUT_OF_RESOURCES: i64 = 7;
```

*Defined at line 32*

---

## EFI_NOT_FOUND:

```fides
pub const EFI_NOT_FOUND: i64 = 14;
```

*Defined at line 33*

---

## EFI_ACCESS_DENIED:

```fides
pub const EFI_ACCESS_DENIED: i64 = 15;
```

*Defined at line 34*

---

## EFI_TIMEOUT:

```fides
pub const EFI_TIMEOUT: i64 = 18;
```

*Defined at line 35*

---

## efi_reserved_memory:

```fides
pub const efi_reserved_memory: i64 = 0;
```

*Defined at line 38*

---

## efi_loader_code:

```fides
pub const efi_loader_code: i64 = 1;
```

*Defined at line 39*

---

## efi_loader_data:

```fides
pub const efi_loader_data: i64 = 2;
```

*Defined at line 40*

---

## efi_boot_services_code:

```fides
pub const efi_boot_services_code: i64 = 3;
```

*Defined at line 41*

---

## efi_boot_services_data:

```fides
pub const efi_boot_services_data: i64 = 4;
```

*Defined at line 42*

---

## efi_runtime_services_code:

```fides
pub const efi_runtime_services_code: i64 = 5;
```

*Defined at line 43*

---

## efi_runtime_services_data:

```fides
pub const efi_runtime_services_data: i64 = 6;
```

*Defined at line 44*

---

## efi_conventional_memory:

```fides
pub const efi_conventional_memory: i64 = 7;
```

*Defined at line 45*

---

## efi_unusable_memory:

```fides
pub const efi_unusable_memory: i64 = 8;
```

*Defined at line 46*

---

## efi_acpi_reclaim_memory:

```fides
pub const efi_acpi_reclaim_memory: i64 = 9;
```

*Defined at line 47*

---

## efi_acpi_nvs_memory:

```fides
pub const efi_acpi_nvs_memory: i64 = 10;
```

*Defined at line 48*

---

## efi_memory_mapped_io:

```fides
pub const efi_memory_mapped_io: i64 = 11;
```

*Defined at line 49*

---

## efi_memory_mapped_io_port_space:

```fides
pub const efi_memory_mapped_io_port_space: i64 = 12;
```

*Defined at line 50*

---

## efi_pal_code:

```fides
pub const efi_pal_code: i64 = 13;
```

*Defined at line 51*

---

## efi_persistent_memory:

```fides
pub const efi_persistent_memory: i64 = 14;
```

*Defined at line 52*

---

## uefi_pool_data:

```fides
pub const uefi_pool_data: i64 = 0;
```

*Defined at line 55*

---

## uefi_pool_realloc:

```fides
pub const uefi_pool_realloc: i64 = 1;
```

*Defined at line 56*

---

## uefi_pool_align:

```fides
pub const uefi_pool_align: i64 = 2;
```

*Defined at line 57*

---

## efi_timer_relative:

```fides
pub const efi_timer_relative: i64 = 0;
```

*Defined at line 60*

---

## efi_timer_periodic:

```fides
pub const efi_timer_periodic: i64 = 1;
```

*Defined at line 61*

---

## efi_timer_cancel:

```fides
pub const efi_timer_cancel: i64 = 2;
```

*Defined at line 62*

---

## efi_handle_is_valid

```fides
pub fn efi_handle_is_valid(h: i64) -> bool {
```

**Parameters:**

- `h`: `i64`

**Returns:** `bool`

*Defined at line 70*

---

## efi_bs_hdr_size:

```fides
pub const efi_bs_hdr_size: i64 = 40;
```

*Defined at line 80*

---

## efi_bs_raise_tpl:

```fides
pub const efi_bs_raise_tpl: i64 = 40;
```

*Defined at line 83*

---

## efi_bs_restore_tpl:

```fides
pub const efi_bs_restore_tpl: i64 = 48;
```

*Defined at line 84*

---

## efi_bs_allocate_pages:

```fides
pub const efi_bs_allocate_pages: i64 = 56;
```

*Defined at line 85*

---

## efi_bs_free_pages:

```fides
pub const efi_bs_free_pages: i64 = 64;
```

*Defined at line 86*

---

## efi_bs_get_memory_map:

```fides
pub const efi_bs_get_memory_map: i64 = 72;
```

*Defined at line 87*

---

## efi_bs_allocate_pool:

```fides
pub const efi_bs_allocate_pool: i64 = 80;
```

*Defined at line 88*

---

## efi_bs_free_pool:

```fides
pub const efi_bs_free_pool: i64 = 88;
```

*Defined at line 89*

---

## efi_bs_create_event:

```fides
pub const efi_bs_create_event: i64 = 96;
```

*Defined at line 90*

---

## efi_bs_set_timer:

```fides
pub const efi_bs_set_timer: i64 = 104;
```

*Defined at line 91*

---

## efi_bs_wait_for_event:

```fides
pub const efi_bs_wait_for_event: i64 = 112;
```

*Defined at line 92*

---

## efi_bs_signal_event:

```fides
pub const efi_bs_signal_event: i64 = 120;
```

*Defined at line 93*

---

## efi_bs_close_event:

```fides
pub const efi_bs_close_event: i64 = 128;
```

*Defined at line 94*

---

## efi_bs_check_event:

```fides
pub const efi_bs_check_event: i64 = 136;
```

*Defined at line 95*

---

## efi_bs_install_protocol_interface:

```fides
pub const efi_bs_install_protocol_interface: i64 = 144;
```

*Defined at line 96*

---

## efi_bs_reinstall_protocol_interface:

```fides
pub const efi_bs_reinstall_protocol_interface: i64 = 152;
```

*Defined at line 97*

---

## efi_bs_uninstall_protocol_interface:

```fides
pub const efi_bs_uninstall_protocol_interface: i64 = 160;
```

*Defined at line 98*

---

## efi_bs_handle_protocol:

```fides
pub const efi_bs_handle_protocol: i64 = 168;
```

*Defined at line 99*

---

## efi_bs_register_protocol_notify:

```fides
pub const efi_bs_register_protocol_notify: i64 = 176;
```

*Defined at line 100*

---

## efi_bs_locate_handle:

```fides
pub const efi_bs_locate_handle: i64 = 184;
```

*Defined at line 101*

---

## efi_bs_locate_protocol:

```fides
pub const efi_bs_locate_protocol: i64 = 192;
```

*Defined at line 102*

---

## efi_bs_install_configuration_table:

```fides
pub const efi_bs_install_configuration_table: i64 = 200;
```

*Defined at line 103*

---

## efi_bs_load_image:

```fides
pub const efi_bs_load_image: i64 = 208;
```

*Defined at line 104*

---

## efi_bs_start_image:

```fides
pub const efi_bs_start_image: i64 = 216;
```

*Defined at line 105*

---

## efi_bs_exit:

```fides
pub const efi_bs_exit: i64 = 224;
```

*Defined at line 106*

---

## efi_bs_unload_image:

```fides
pub const efi_bs_unload_image: i64 = 232;
```

*Defined at line 107*

---

## efi_bs_exit_boot_services:

```fides
pub const efi_bs_exit_boot_services: i64 = 240;
```

*Defined at line 108*

---

## efi_bs_get_next_monotonic_count:

```fides
pub const efi_bs_get_next_monotonic_count: i64 = 248;
```

*Defined at line 109*

---

## efi_bs_stall:

```fides
pub const efi_bs_stall: i64 = 256;
```

*Defined at line 110*

---

## efi_bs_set_watchdog_timer:

```fides
pub const efi_bs_set_watchdog_timer: i64 = 264;
```

*Defined at line 111*

---

## efi_bs_connect_controller:

```fides
pub const efi_bs_connect_controller: i64 = 272;
```

*Defined at line 112*

---

## efi_bs_disconnect_controller:

```fides
pub const efi_bs_disconnect_controller: i64 = 280;
```

*Defined at line 113*

---

## efi_bs_open_protocol:

```fides
pub const efi_bs_open_protocol: i64 = 288;
```

*Defined at line 114*

---

## efi_bs_close_protocol:

```fides
pub const efi_bs_close_protocol: i64 = 296;
```

*Defined at line 115*

---

## efi_bs_open_protocol_information:

```fides
pub const efi_bs_open_protocol_information: i64 = 304;
```

*Defined at line 116*

---

## efi_bs_protocols_per_handle:

```fides
pub const efi_bs_protocols_per_handle: i64 = 312;
```

*Defined at line 117*

---

## efi_bs_locate_handle_buffer:

```fides
pub const efi_bs_locate_handle_buffer: i64 = 320;
```

*Defined at line 118*

---

## efi_bs_locate_all_handles:

```fides
pub const efi_bs_locate_all_handles: i64 = 328;
```

*Defined at line 119*

---

## efi_bs_copy_mem:

```fides
pub const efi_bs_copy_mem: i64 = 336;
```

*Defined at line 120*

---

## efi_bs_set_mem:

```fides
pub const efi_bs_set_mem: i64 = 344;
```

*Defined at line 121*

---

## efi_bs_create_event_ex:

```fides
pub const efi_bs_create_event_ex: i64 = 352;
```

*Defined at line 122*

---

## efi_rs_hdr_size:

```fides
pub const efi_rs_hdr_size: i64 = 40;
```

*Defined at line 125*

---

## efi_rs_get_time:

```fides
pub const efi_rs_get_time: i64 = 40;
```

*Defined at line 126*

---

## efi_rs_set_time:

```fides
pub const efi_rs_set_time: i64 = 48;
```

*Defined at line 127*

---

## efi_rs_get_wakeup_time:

```fides
pub const efi_rs_get_wakeup_time: i64 = 56;
```

*Defined at line 128*

---

## efi_rs_set_wakeup_time:

```fides
pub const efi_rs_set_wakeup_time: i64 = 64;
```

*Defined at line 129*

---

## efi_rs_set_virtual_address_map:

```fides
pub const efi_rs_set_virtual_address_map: i64 = 72;
```

*Defined at line 130*

---

## efi_rs_convert_pointer:

```fides
pub const efi_rs_convert_pointer: i64 = 80;
```

*Defined at line 131*

---

## efi_rs_get_variable:

```fides
pub const efi_rs_get_variable: i64 = 88;
```

*Defined at line 132*

---

## efi_rs_set_variable:

```fides
pub const efi_rs_set_variable: i64 = 96;
```

*Defined at line 133*

---

## efi_rs_get_next_variable_name:

```fides
pub const efi_rs_get_next_variable_name: i64 = 104;
```

*Defined at line 134*

---

## efi_rs_get_next_high_monotonic_count:

```fides
pub const efi_rs_get_next_high_monotonic_count: i64 = 112;
```

*Defined at line 135*

---

## efi_rs_reset_system:

```fides
pub const efi_rs_reset_system: i64 = 120;
```

*Defined at line 136*

---

## efi_st_hdr_size:

```fides
pub const efi_st_hdr_size: i64 = 24;  // Signature(8) + Revision(4) + HeaderSize(4) + CRC32(4) + Reserved(4)
```

*Defined at line 140*

---

## efi_st_firmware_vendor:

```fides
pub const efi_st_firmware_vendor: i64 = 24;  // ptr
```

*Defined at line 141*

---

## efi_st_firmware_revision:

```fides
pub const efi_st_firmware_revision: i64 = 32;  // u32
```

*Defined at line 142*

---

## efi_st_console_in_handle:

```fides
pub const efi_st_console_in_handle: i64 = 40;  // EFI_HANDLE
```

*Defined at line 143*

---

## efi_st_con_in:

```fides
pub const efi_st_con_in: i64 = 48;  // ptr to SIMPLE_TEXT_INPUT
```

*Defined at line 144*

---

## efi_st_console_out_handle:

```fides
pub const efi_st_console_out_handle: i64 = 56;  // EFI_HANDLE
```

*Defined at line 145*

---

## efi_st_con_out:

```fides
pub const efi_st_con_out: i64 = 64;  // ptr to SIMPLE_TEXT_OUTPUT
```

*Defined at line 146*

---

## efi_st_standard_error_handle:

```fides
pub const efi_st_standard_error_handle: i64 = 72;  // EFI_HANDLE
```

*Defined at line 147*

---

## efi_st_std_err:

```fides
pub const efi_st_std_err: i64 = 80;  // ptr to SIMPLE_TEXT_OUTPUT
```

*Defined at line 148*

---

## efi_st_runtime_services:

```fides
pub const efi_st_runtime_services: i64 = 88;  // ptr to EFI_RUNTIME_SERVICES
```

*Defined at line 149*

---

## efi_st_boot_services:

```fides
pub const efi_st_boot_services: i64 = 96;  // ptr to EFI_BOOT_SERVICES
```

*Defined at line 150*

---

## efi_st_number_of_table_entries:

```fides
pub const efi_st_number_of_table_entries: i64 = 104;  // usize
```

*Defined at line 151*

---

## efi_st_configuration_table:

```fides
pub const efi_st_configuration_table: i64 = 112;  // ptr
```

*Defined at line 152*

---

## uefi_init

```fides
pub fn uefi_init(image_handle: i64, system_table: i64) -> void {
```

**Parameters:**

- `image_handle`: `i64`
- `system_table`: `i64`

**Returns:** `void`

*Defined at line 163*

---

## uefi_get_boot_services

```fides
pub fn uefi_get_boot_services() -> i64 {
```

**Returns:** `i64`

*Defined at line 171*

---

## uefi_get_runtime_services

```fides
pub fn uefi_get_runtime_services() -> i64 {
```

**Returns:** `i64`

*Defined at line 176*

---

## uefi_get_con_out

```fides
pub fn uefi_get_con_out() -> i64 {
```

**Returns:** `i64`

*Defined at line 181*

---

## uefi_get_con_in

```fides
pub fn uefi_get_con_in() -> i64 {
```

**Returns:** `i64`

*Defined at line 186*

---

## efi_read_ptr

```fides
pub fn efi_read_ptr(addr: i64) -> i64 {
```

**Parameters:**

- `addr`: `i64`

**Returns:** `i64`

*Defined at line 195*

---

## efi_write_ptr

```fides
pub fn efi_write_ptr(addr: i64, val: i64) -> void {
```

**Parameters:**

- `addr`: `i64`
- `val`: `i64`

**Returns:** `void`

*Defined at line 200*

---

## uefi_allocate_pool

```fides
pub fn uefi_allocate_pool(pool_type: i64, size: i64) -> i64 {
```

**Parameters:**

- `pool_type`: `i64`
- `size`: `i64`

**Returns:** `i64`

*Defined at line 209*

---

## uefi_free_pool

```fides
pub fn uefi_free_pool(buffer: i64) -> i64 {
```

**Parameters:**

- `buffer`: `i64`

**Returns:** `i64`

*Defined at line 221*

---

## uefi_allocate_pages

```fides
pub fn uefi_allocate_pages(memory_type: i64, pages: i64) -> i64 {
```

**Parameters:**

- `memory_type`: `i64`
- `pages`: `i64`

**Returns:** `i64`

*Defined at line 230*

---

## uefi_get_memory_map

```fides
pub fn uefi_get_memory_map(map_size: i64, map: i64) -> i64 {
```

**Parameters:**

- `map_size`: `i64`
- `map`: `i64`

**Returns:** `i64`

*Defined at line 240*

---

## uefi_exit_boot_services

```fides
pub fn uefi_exit_boot_services(map_key: i64) -> i64 {
```

**Parameters:**

- `map_key`: `i64`

**Returns:** `i64`

*Defined at line 252*

---

## uefi_stall

```fides
pub fn uefi_stall(microseconds: i64) -> void {
```

**Parameters:**

- `microseconds`: `i64`

**Returns:** `void`

*Defined at line 261*

---

## uefi_print

```fides
pub fn uefi_print(msg: str) -> void {
```

**Parameters:**

- `msg`: `str`

**Returns:** `void`

*Defined at line 271*

---

## uefi_clear_screen

```fides
pub fn uefi_clear_screen() -> void {
```

**Returns:** `void`

*Defined at line 281*

---

*Generated by [scribe](https://fides.dev) — Fides documentation generator*
