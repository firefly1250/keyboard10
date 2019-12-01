/*
   based on
   https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLETests/SampleHIDKeyboard.cpp
 */

#include "BLEDevice.h"
#include "BLEServer.h"
#include "BLEUtils.h"
#include "BLE2902.h"
#include "BLEHIDDevice.h"
#include <esp_log.h>
#include <string>
#include "MainBLEServer.h"
#include "global.h"
#include "ReportMap.h"

#include <Arduino.h>

BLEHIDDevice *hid;
BLECharacteristic *inputKbd;
BLECharacteristic *inputMouse;
BLECharacteristic *outputKbd;

uint32_t passKey = 1307;
/** @brief security callback
 *
 * This class is passed to the BLEServer as callbacks for security
 * related actions. Depending on IO_CAP configuration & host, different
 * types of security actions are required for bonding this device to a
 * host. */
class CB_Security : public BLESecurityCallbacks {

  // Request a pass key to be typed in on the host
  uint32_t onPassKeyRequest() {
    ESP_LOGE(LOG_TAG, "The passkey request %d", passKey);
    vTaskDelay(25000);
    return passKey;
  }

  // The host sends a pass key to the ESP32 which needs to be displayed
  // and typed into the host PC
  void onPassKeyNotify(uint32_t pass_key) {
    ESP_LOGE(LOG_TAG, "The passkey Notify number:%d", pass_key);
    passKey = pass_key;
  }

  // CB for testing if a host is allowed to connect, in our case always yes.
  bool onSecurityRequest() { return true; }

  // CB on a completed authentication (not depending on status)
  void onAuthenticationComplete(esp_ble_auth_cmpl_t auth_cmpl) {
    if(auth_cmpl.success) {
      ESP_LOGI(LOG_TAG, "remote BD_ADDR:");
      esp_log_buffer_hex(LOG_TAG, auth_cmpl.bd_addr, sizeof(auth_cmpl.bd_addr));
      ESP_LOGI(LOG_TAG, "address type = %d", auth_cmpl.addr_type);
    }
    ESP_LOGI(LOG_TAG, "pair status = %s",
             auth_cmpl.success ? "success" : "fail");
  }

  // You need to confirm the given pin
  bool onConfirmPIN(uint32_t pin) {
    ESP_LOGE(LOG_TAG, "Confirm pin: %d", pin);
    return true;
  }
};

void MainBLEServer::run(void *data) {
  ESP_LOGI(LOG_TAG, "Starting BLE work!");

  BLEDevice::init(ble_name);
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(this);
  BLEDevice::setSecurityCallbacks(new CB_Security());
  /*
   * Instantiate hid device
   */
  hid = new BLEHIDDevice(pServer);

  /*
   * Set manufacturer name (OPTIONAL)
   * https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.manufacturer_name_string.xml
   */
  std::string name = "esp-community";
  hid->manufacturer()->setValue(name);

  /*
   * Set pnp parameters (MANDATORY)
   * https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.pnp_id.xml
   */

  hid->pnp(0x02, 0xe502, 0xa111, 0x0210);

  /*
   * Set hid informations (MANDATORY)
   * https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.hid_information.xml
   */
  hid->hidInfo(0x00, 0x01);

  switch(hid_type) {
  case HID_MOUSE:
    inputMouse = hid->inputReport(1);
    hid->reportMap((uint8_t *)reportMapMouse, sizeof(reportMapMouse));
    break;
  case HID_KEYBOARD:
    inputKbd = hid->inputReport(1);
    outputKbd = hid->outputReport(1);
    hid->reportMap((uint8_t *)reportMapKeyboard, sizeof(reportMapKeyboard));
    break;
  }

  hid->startServices();

  /*
   * Its good to setup advertising by providing appearance and advertised
   * service. This will let clients find our device by type
   */
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->setAppearance(hid_type);
  // pAdvertising->setMinPreferred(6);  // 6*1.25ms minimum
  // pAdvertising->setMaxPreferred(12); // 12*1.25ms maximum
  pAdvertising->addServiceUUID(hid->hidService()->getUUID());
  pAdvertising->start();

  BLESecurity *pSecurity = new BLESecurity();
  // pSecurity->setAuthenticationMode(ESP_LE_AUTH_BOND);
  pSecurity->setCapability(ESP_IO_CAP_NONE);

  ESP_LOGI(LOG_TAG, "Advertising started!");
  while(1) {
    delay(1000000);
  }
}

void MainBLEServer::onConnect(BLEServer *pServer) {
  BLE2902 *desc;

  is_connected = true;

  // enable notifications for input service, suggested by chegewara to support
  // iOS/Win10
  // https://github.com/asterics/esp32_mouse_keyboard/issues/4#issuecomment-386558158

  switch(hid_type) {
  case HID_MOUSE:
    desc =
        (BLE2902 *)inputMouse->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    desc->setNotifications(true);
    mouse_task->start();
    break;
  case HID_KEYBOARD:
    desc = (BLE2902 *)inputKbd->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    desc->setNotifications(true);
    keyboard_task->start();
    break;
  }

  ESP_LOGI(LOG_TAG, "Client connected ! ");
}

void MainBLEServer::onDisconnect(BLEServer *pServer) {
  BLE2902 *desc;

  is_connected = false;

  // disable notifications for input service, suggested by chegewara to
  // reduce power & memory usage
  // https://github.com/asterics/esp32_mouse_keyboard/commit/a1796ce91155ec7db62af4a53dbdef32bc4adf08#commitcomment-28888676

  switch(hid_type) {
  case HID_MOUSE:
    desc =
        (BLE2902 *)inputMouse->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    desc->setNotifications(false);
    mouse_task->stop();
    break;
  case HID_KEYBOARD:
    desc = (BLE2902 *)inputKbd->getDescriptorByUUID(BLEUUID((uint16_t)0x2902));
    desc->setNotifications(false);
    keyboard_task->stop();
    break;
  }

  // restart advertising
  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
  ESP_LOGI(LOG_TAG, "Client disconnected, restarting advertising");
}