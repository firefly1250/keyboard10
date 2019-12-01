
#include "BLEHIDDevice.h"
#include <memory>
#include "Task.h"

constexpr auto LOG_TAG = "SampleHIDDevice";

#define GRASPING_KEYBOARD_LEFT

#ifdef GRASPING_KEYBOARD_LEFT
#define ESPNOW_SLAVE
constexpr auto hid_type = HID_KEYBOARD;
constexpr auto ble_name = "grasping-keyboard";
#endif
#ifdef GRASPING_KEYBOARD_RIGHT
#define ESPNOW_MASTER
constexpr auto hid_type = HID_MOUSE;
constexpr auto ble_name = "grasping-mouse";
#endif

class ESPNowTask : public Task {
public:
  ESPNowTask() : Task("ESPNowTask", 8092, 5, 0) {}
  esp_err_t send(uint8_t data);

private:
  void run(void *) override;
};

extern BLEHIDDevice *hid;
extern BLECharacteristic *inputKbd;
extern BLECharacteristic *inputMouse;
extern BLECharacteristic *outputKbd;

extern std::unique_ptr<Task> keyboard_task;
extern std::unique_ptr<Task> mouse_task;
extern std::unique_ptr<ESPNowTask> espnow_task;

extern uint8_t my_data;
extern uint8_t recieved_data;
extern xSemaphoreHandle mutex_my_data;
extern xSemaphoreHandle mutex_recieved_data;