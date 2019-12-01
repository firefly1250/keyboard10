#define US_KEYBOARD

#include <Arduino.h>
#include <memory>
#include "BLECharacteristic.h"
//#include "HIDKeyboardTypes.h"
#include <esp_log.h>
#include <string>
#include <Task.h>

#include "global.h"
#include "MainBLEServer.h"

#include "keymap.h"

namespace Pins {
#ifdef GRASPING_KEYBOARD_LEFT
constexpr uint8_t key1 = 13;
constexpr uint8_t key2 = 14;
constexpr uint8_t key3 = 25;
constexpr uint8_t key4 = 27;
constexpr uint8_t key5 = 26;
#endif
#ifdef GRASPING_KEYBOARD_RIGHT
constexpr uint8_t key1 = 14;
constexpr uint8_t key2 = 27;
constexpr uint8_t key3 = 13;
constexpr uint8_t key4 = 26;
constexpr uint8_t key5 = 25;
#endif

constexpr uint8_t x = 32;
constexpr uint8_t y = 33;
constexpr uint8_t battery = 0; // TODO
constexpr uint8_t led = 0;     // TODO

}; // namespace Pins

uint8_t my_data;
uint8_t recieved_data;
xSemaphoreHandle mutex_my_data;
xSemaphoreHandle mutex_recieved_data;

auto main_ble = new MainBLEServer();

int sign(int x) { return x > 0 ? 1 : x < 0 ? -1 : 0; }

class ButtonReadTask : public Task {
public:
  ButtonReadTask() : Task("ButtonReadTask", 4096, 5, 0) {}

private:
  uint8_t pre_data{0};
  void run(void *) override {
    portTickType xLastWakeTime = xTaskGetTickCount();
    while(1) {
      uint8_t data = 0;
      data |= ((!digitalRead(Pins::key1)) << 0);
      data |= ((!digitalRead(Pins::key2)) << 1);
      data |= ((!digitalRead(Pins::key3)) << 2);
      data |= ((!digitalRead(Pins::key4)) << 3);
      data |= ((!digitalRead(Pins::key5)) << 4);
      xSemaphoreTake(mutex_my_data, portMAX_DELAY);
      my_data = data;
      xSemaphoreGive(mutex_my_data);
      if(pre_data != my_data) {
        if(espnow_task->send(my_data) == ESP_OK)
          pre_data = my_data;
      }
      vTaskDelayUntil(&xLastWakeTime, 2);
    }
  }
};
struct KeyState {
  uint16_t data;
  uint8_t key;
  uint8_t modifier;
  uint8_t fn;

  KeyState(uint16_t data) {
    this->data = data;
    key = (data & 0b111) | ((data & (0b111 << 5)) >> 2);
    const auto modifier_key_state =
        ((data & 0b11000) >> 3) | ((data & (0b11000 << 5)) >> 6);

    modifier = 0;
    for(int i = 0; i < 4; i++)
      if(modifier_key_state & (1 << i))
        modifier |= modifiermap[i];

    fn = (modifier & key_alt) && (modifier & key_gui);
    if(fn)
      modifier &= ~(key_alt | key_gui);
  }

  void print_data() const {
    for(auto i = 9; i >= 0; i--)
      Serial.print((data & (1 << i)) >> i);
  }
};

KeyState key_update() {
  static KeyState key_pre{0};
  static KeyState key{0};
  static uint16_t count{0};
  static bool released{false};

  xSemaphoreTake(mutex_my_data, portMAX_DELAY);
  xSemaphoreTake(mutex_recieved_data, portMAX_DELAY);
#ifdef GRASPING_KEYBOARD_LEFT
  const uint16_t data_now = (my_data << 5) | recieved_data;
#endif
#ifdef GRASPING_KEYBOARD_RIGHT
  const uint16_t data_now = my_data | (recieved_data << 5);
#endif
  xSemaphoreGive(mutex_recieved_data);
  xSemaphoreGive(mutex_my_data);

  KeyState key_now(data_now);

  for(int i = 0; i < 6; i++) {
    if((key_pre.key & (1 << i)) && !(key_now.key & (1 << i))) {
      released = true;
      break;
    }
  }
  if(key_now.key == 0)
    released = false;

  if(key_pre.data == key_now.data)
    count++;
  else
    count = 0;
  if(count > 15)
    key = key_now;

  if(released)
    key = KeyState(key.data & 0b1100011000);

  key_pre = key_now;
  return key;
}

class KeybordTask : public Task {
public:
  KeybordTask() : Task("KeybordTask", 8092, 5, 1) {}

private:
  void run(void *) override {
    delay(3000); // wait 3 seconds before send first message
    portTickType xLastWakeTime = xTaskGetTickCount();

    int count = 0;
    while(1) {
      const auto key_state = key_update();
      if(count == 16) {
        count = 0;

        uint8_t a[] = {key_state.modifier,
                       0x0,
                       keymap[key_state.key | key_state.fn * 64],
                       0x0,
                       0x0,
                       0x0,
                       0x0,
                       0x0};
        inputKbd->setValue(a, sizeof(a));
        inputKbd->notify();

        key_state.print_data();
        Serial.print("\t,");
        Serial.print((int)key_state.modifier);
        Serial.print("\t,");
        Serial.print((int)key_state.key);
        Serial.println("");
      }
      count += 2;
      vTaskDelayUntil(&xLastWakeTime, 2);
    }
  }
};

class MouseTask : public Task {
public:
  MouseTask() : Task("MouseTask", 8092, 5, 1) {}

private:
  void run(void *) override {
    delay(3000); // wait 3 seconds before send first message
    portTickType xLastWakeTime = xTaskGetTickCount();

    int count = 0;
    while(1) {
      const auto key_state = key_update();
      if(count == 16) { // 60fps
        count = 0;
        uint8_t a[] = {0, 0, 0, 0, 0};
        if(keymap[key_state.key] == key_left_click)
          a[0] |= 1;
        if(keymap[key_state.key] == key_right_click)
          a[0] |= 2;
        if(keymap[key_state.key] == key_middle_click)
          a[0] |= 4;

        auto x_raw = -(analogRead(Pins::x) - 1950);
        auto y_raw = +(analogRead(Pins::y) - 1950);
        if(abs(x_raw) < 150)
          x_raw = 0;
        if(abs(y_raw) < 150)
          y_raw = 0;
        if(!key_state.fn) {
          const auto x =
              static_cast<uint8_t>(0.01 * (x_raw - 150 * sign(x_raw)));
          const auto y =
              static_cast<uint8_t>(0.01 * (y_raw - 150 * sign(y_raw)));
          a[1] = y;
          a[2] = -x;
          a[3] = 0;
          a[4] = 0;
        } else {
          const auto x =
              static_cast<uint8_t>(0.002 * (x_raw - 150 * sign(x_raw)));
          const auto y =
              static_cast<uint8_t>(0.002 * (y_raw - 150 * sign(y_raw)));
          a[1] = 0;
          a[2] = 0;
          a[3] = x;
          a[4] = y;
        }
        inputMouse->setValue(a, sizeof(a));
        inputMouse->notify();

        key_state.print_data();
        Serial.print("\t,");
        for(int i = 0; i < sizeof(a); i++) {
          Serial.print(a[i]);
          Serial.print("\t,");
        }
        Serial.println("");
      }
      count += 2;
      vTaskDelayUntil(&xLastWakeTime, 2);
    }
  }
};

std::unique_ptr<Task> keyboard_task(new KeybordTask());
std::unique_ptr<Task> mouse_task(new MouseTask());
std::unique_ptr<ESPNowTask> espnow_task(new ESPNowTask());

/*
 * This callback is connect with output report. In keyboard output report
 * report special keys changes, like CAPSLOCK, NUMLOCK We can add digital pins
 * with LED to show status bit 1 - NUM LOCK bit 2 - CAPS LOCK bit 3 - SCROLL
 * LOCK
 */

void MyOutputCallbacks::onWrite(BLECharacteristic *me) {
  auto value = reinterpret_cast<const uint8_t *>(me->getValue().c_str());
  ESP_LOGI(LOG_TAG, "special keys: %d", *value);
}

void setup(void) {
  Serial.begin(115200);
  pinMode(Pins::key1, INPUT_PULLUP);
  pinMode(Pins::key2, INPUT_PULLUP);
  pinMode(Pins::key3, INPUT_PULLUP);
  pinMode(Pins::key4, INPUT_PULLUP);
  pinMode(Pins::key5, INPUT_PULLUP);

  init_keymap();

  mutex_my_data = xSemaphoreCreateMutex();
  mutex_recieved_data = xSemaphoreCreateMutex();

  main_ble->start();
  espnow_task->start();

  auto button_task = new ButtonReadTask();
  button_task->start();
}

void loop() {}