#ifndef ceammc_rust_hw_h
#define ceammc_rust_hw_h

/* Warning, this file is autogenerated by cbindgen. Don't modify this manually. */

#include <cstdint>
#include <cstddef>


constexpr static const int32_t ceammc_JOB_ERROR = -1;

enum class ceammc_hw_gamepad_btn {
    South,
    East,
    North,
    West,
    C,
    Z,
    LeftTrigger,
    LeftTrigger2,
    RightTrigger,
    RightTrigger2,
    Select,
    Start,
    Mode,
    LeftThumb,
    RightThumb,
    DPadUp,
    DPadDown,
    DPadLeft,
    DPadRight,
    Unknown,
};

enum class ceammc_hw_gamepad_event_axis {
    LeftStickX,
    LeftStickY,
    LeftZ,
    RightStickX,
    RightStickY,
    RightZ,
    DPadX,
    DPadY,
    Unknown,
};

enum class ceammc_hw_gamepad_event_type {
    ButtonPressed,
    ButtonRepeated,
    ButtonReleased,
    ButtonChanged,
    AxisChanged,
    Connected,
    Disconnected,
    Dropped,
};

enum class ceammc_hw_gamepad_powerstate {
    Unknown = 0,
    Wired,
    Discharging,
    Charging,
    Charged,
};

enum class ceammc_hw_printer_state {
    READY,
    PAUSED,
    PRINTING,
    UNKNOWN,
};

/// gamepad opaque type
struct ceammc_hw_gamepad;

struct ceammc_gamepad_err_cb {
    /// pointer to user data
    void *user;
    void (*cb)(void*, const char*);
};

struct ceammc_hw_gamepad_event {
    /// gamepad id
    size_t id;
    ceammc_hw_gamepad_btn button;
    ceammc_hw_gamepad_event_type event;
    ceammc_hw_gamepad_event_axis axis;
    float value;
};

struct ceammc_gamepad_event_cb {
    /// pointer to user data
    void *user;
    void (*cb)(void *user, const ceammc_hw_gamepad_event *event);
};

struct ceammc_hw_gamepad_powerinfo {
    ceammc_hw_gamepad_powerstate state;
    uint8_t data;
};

struct ceammc_gamepad_dev_info {
    /// gamepad name
    const char *name;
    /// gamepad os_name
    const char *os_name;
    /// gamepad id
    size_t id;
    /// gamepad power info
    ceammc_hw_gamepad_powerinfo power;
    /// gamepad vendor id
    uint16_t vid;
    /// gamepad product id
    uint16_t pid;
    /// gamepad is connected
    bool is_connected;
    /// gamepad has force feedback capability
    bool has_ff;
};

struct ceammc_gamepad_listdev_cb {
    void *user;
    void (*cb)(void *user, const ceammc_gamepad_dev_info *info);
};

struct ceammc_hw_notify_cb {
    /// dispatcher ID
    size_t id;
    /// dispatcher callback (not NULL!)
    void (*f)(size_t id);
};

struct ceammc_hw_printer_info {
    const char *name;
    const char *system_name;
    const char *driver_name;
    const char *uri;
    const char *location;
    bool is_default;
    bool is_shared;
    ceammc_hw_printer_state state;
};

struct ceammc_hw_printer_info_cb {
    void *user;
    void (*cb)(void *user, const ceammc_hw_printer_info *info);
};

struct ceammc_hw_print_options {
    bool landscape;
};

/// error callback
struct ceammc_hw_error_cb {
    /// pointer to user data (can be NULL)
    void *user;
    /// can be NULL
    void (*cb)(void*, const char*);
};


extern "C" {

/// free gamepad
/// @param gp - pointer to gp
void ceammc_hw_gamepad_free(ceammc_hw_gamepad *gp);

/// list connected devices
bool ceammc_hw_gamepad_list_devices(ceammc_hw_gamepad *gp);

/// create new gamepad
/// @param on_err - error callback
/// @param on_event - gamepad event callback
/// @param on_devinfo - gamepad list connected devices callback
/// @param cb_notify - dispatcher notification
/// @return pointer to new gamepad or NULL on error
ceammc_hw_gamepad *ceammc_hw_gamepad_new(ceammc_gamepad_err_cb on_err,
                                         ceammc_gamepad_event_cb on_event,
                                         ceammc_gamepad_listdev_cb on_devinfo,
                                         ceammc_hw_notify_cb cb_notify,
                                         uint64_t poll_time_ms);

/// process events
/// @param gp - pointer to gp
void ceammc_hw_gamepad_process_events(ceammc_hw_gamepad *gp);

/// get printers info via specified callback
/// @param info_cb - called on every found printer
/// @return number of printers found
size_t ceammc_hw_get_printers(ceammc_hw_printer_info_cb info_cb);

int32_t ceammc_hw_print_file(const char *printer,
                             const char *path,
                             const ceammc_hw_print_options *opts,
                             ceammc_hw_error_cb on_err,
                             ceammc_hw_error_cb on_debug);

bool ceammc_hw_printer_default(ceammc_hw_printer_info_cb info_cb);

} // extern "C"

#endif // ceammc_rust_hw_h