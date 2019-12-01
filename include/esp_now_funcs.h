#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

namespace ESPNow {
void init() {
  WiFi.disconnect();
  if(esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  } else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}

std::unique_ptr<esp_now_peer_info_t> scan_for_slave(const char *slave_SSID) {
  std::unique_ptr<esp_now_peer_info_t> slave = nullptr;
  int8_t scanResults = WiFi.scanNetworks();

  Serial.println("");
  if(scanResults == 0) {
    Serial.println("No WiFi devices in AP Mode found");
  } else {
    Serial.print("Found ");
    Serial.print(scanResults);
    Serial.println(" devices ");
    for(int i = 0; i < scanResults; ++i) {
      // Print SSID and RSSI for each device found
      String SSID = WiFi.SSID(i);
      int32_t RSSI = WiFi.RSSI(i);
      String BSSIDstr = WiFi.BSSIDstr(i);

      delay(10);
      // Check if the current device starts with `Slave`
      if(SSID.equals(slave_SSID)) {
        slave.reset(new esp_now_peer_info_t);
        // SSID of interest
        Serial.println("Found a Slave.");
        Serial.print(i + 1);
        Serial.print(": ");
        Serial.print(SSID);
        Serial.print(" [");
        Serial.print(BSSIDstr);
        Serial.print("]");
        Serial.print(" (");
        Serial.print(RSSI);
        Serial.print(")");
        Serial.println("");
        // Get BSSID => Mac Address of the Slave
        int mac[6];
        if(6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x%c", &mac[0],
                       &mac[1], &mac[2], &mac[3], &mac[4], &mac[5])) {
          for(int ii = 0; ii < 6; ++ii) {
            slave->peer_addr[ii] = (uint8_t)mac[ii];
          }
        }
        // we are planning to have only one slave in this example;
        // Hence, break after we find one, to be a bit efficient
        break;
      }
    }
  }

  if(slave) {
    Serial.println("Slave Found, processing..");
  } else {
    Serial.println("Slave Not Found, trying again.");
  }

  // clean up ram
  WiFi.scanDelete();

  return slave;
}

bool peer(std::unique_ptr<esp_now_peer_info_t> &peer_info) {
  Serial.print("Peer Status: ");
  if(esp_now_is_peer_exist(peer_info->peer_addr)) {
    Serial.println("Already Paired");
    return true;
  } else {
    esp_err_t addStatus = esp_now_add_peer(peer_info.get());
    if(addStatus == ESP_OK) {
      Serial.println("Pair success");
      return true;
    } else if(addStatus == ESP_ERR_ESPNOW_NOT_INIT) {
      Serial.println("ESPNOW Not Init");
      return false;
    } else if(addStatus == ESP_ERR_ESPNOW_ARG) {
      Serial.println("Invalid Argument");
      return false;
    } else if(addStatus == ESP_ERR_ESPNOW_FULL) {
      Serial.println("Peer list full");
      return false;
    } else if(addStatus == ESP_ERR_ESPNOW_NO_MEM) {
      Serial.println("Out of memory");
      return false;
    } else if(addStatus == ESP_ERR_ESPNOW_EXIST) {
      Serial.println("Peer Exists");
      return true;
    } else {
      Serial.println("Not sure what happened");
      return false;
    }
  }
}
}; // namespace ESPNow