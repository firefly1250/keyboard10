#include <Task.h>

class MyOutputCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *me) override;
};

class MainBLEServer : public Task, public BLEServerCallbacks {
public:
  MainBLEServer() : Task("BLEServerTask", 8092, 6, 1) {}
  bool isConected() { return is_connected; }

private:
  bool is_connected{false};
  void onConnect(BLEServer *pServer) override;

  void onDisconnect(BLEServer *pServer) override;
  void run(void *data) override;
};
