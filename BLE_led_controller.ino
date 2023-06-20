#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


BLECharacteristic *pCharacteristic;
BLEServer *pServer;
int ledPin = 2; // LED가 연결된 GPIO 핀 번호
int ledValue = 0; // LED 상태 값
bool isConnected = false; // Flutter 앱과의 연결 상태



class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    isConnected = true;

    std::string value = pCharacteristic->getValue();
    
    if (String(value.c_str()) == "0") {

    pCharacteristic->setValue("1");
    pCharacteristic->notify();
    }

  }

  void onDisconnect(BLEServer* pServer) {
    isConnected = false;
    BLEDevice::startAdvertising();
   
    std::string value = pCharacteristic->getValue();

    if (String(value.c_str()) == "1") {
      pCharacteristic->setValue(0);
      pCharacteristic->notify();
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");

  BLEDevice::init("LED BLE MESH SERVER");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE |
    BLECharacteristic::PROPERTY_NOTIFY
  );

  pCharacteristic->setValue("0");
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);

  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it on your phone!");

  pinMode(ledPin, OUTPUT);
}

void loop() {
  std::string dataFromFlutter = pCharacteristic -> getValue();
  Serial.println(String(dataFromFlutter.c_str()));
  if (dataFromFlutter == "0") {
    digitalWrite(ledPin, LOW); // LED를 끄는 코드
  pCharacteristic -> setValue("0");
  pCharacteristic -> notify();
  

  } else if (dataFromFlutter == "1") {
    digitalWrite(ledPin, HIGH); // LED를 켜는 코드
  pCharacteristic -> setValue("1");
  pCharacteristic -> notify();


  }
  
  delay(1000);
}