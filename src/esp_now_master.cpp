#include "global.h"
#ifdef ESPNOW_MASTER

#include "esp_now_funcs.h"

static constexpr auto CHANNEL = 3;
static constexpr auto SSID = "grasping-keyboard-wifi";
static std::unique_ptr<esp_now_peer_info_t> slave;

void onSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x", mac_addr[0],
           mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("Last Packet Sent to: ");
  Serial.println(macStr);
  Serial.print("Last Packet Send Status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success"
                                                : "Delivery Fail");
}

void onRecieve(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  // char macStr[18];
  // snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
  // mac_addr[0],
  //         mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  // Serial.print("Last Packet Recv from: ");
  // Serial.println(macStr);
  // Serial.print("Last Packet Recv Data: ");
  // Serial.println(*data);
  // Serial.println("");
  xSemaphoreTake(mutex_recieved_data, portMAX_DELAY);
  recieved_data = data[0];
  xSemaphoreGive(mutex_recieved_data);
}

void ESPNowTask::run(void *) {
  WiFi.mode(WIFI_STA);
  ESPNow::init();
  esp_now_register_recv_cb(onRecieve);
  // esp_now_register_send_cb(onSent);
  while(true) {
    slave = ESPNow::scan_for_slave(SSID);
    if(!slave) continue;
    slave->channel = CHANNEL; // pick a channel
    slave->encrypt = 0;       // no encryption
    slave->ifidx = ESP_IF_WIFI_STA;
    if(ESPNow::peer(slave)) break;
  }
}

esp_err_t ESPNowTask::send(uint8_t data) {
  return esp_now_send(slave->peer_addr, &data, 1);
}

#endif