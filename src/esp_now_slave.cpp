#include "global.h"
#ifdef ESPNOW_SLAVE

#include "esp_now_funcs.h"

static constexpr auto CHANNEL = 1;
static constexpr auto SSID = "grasping-keyboard-wifi";
static std::unique_ptr<esp_now_peer_info_t> master{nullptr};

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

  if(!master) {
    master.reset(new esp_now_peer_info_t);
    for(int ii = 0; ii < 6; ++ii) {
      master->peer_addr[ii] = mac_addr[ii];
    }
    master->channel = CHANNEL; // pick a channel
    master->encrypt = 0;       // no encryption
    master->ifidx = ESP_IF_WIFI_AP;
    ESPNow::peer(master);
  }
}

void ESPNowTask::run(void *) {
  WiFi.mode(WIFI_AP);
  // configure device AP mode
  WiFi.softAP(SSID, "soeituv05jeoislgj9", CHANNEL, 0);
  // This is the mac address of the Slave in AP Mode
  Serial.print("AP MAC: ");
  Serial.println(WiFi.softAPmacAddress());
  // Init ESPNow with a fallback logic
  ESPNow::init();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(onRecieve);
  // esp_now_register_send_cb(onSent);
}

esp_err_t ESPNowTask::send(uint8_t data) {
  if(master && esp_now_is_peer_exist(master->peer_addr))
    return esp_now_send(master->peer_addr, &data, 1);
  else
    return ESP_FAIL;
}

#endif