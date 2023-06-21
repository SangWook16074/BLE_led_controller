# BLE_led_controller
--------------------------------------------
아두이노 IDE로 제작한 ESP32 전등 제어 스위치코드입니다.


# 📊 ESP32 소스 코드 분석

ESP32 코드의 동작은 characteristic 값을 확인하여 스위치 동작을 수행하는 것입니다. 

- setup()

```cpp
void setup() {
  Serial.begin(115200);
	// BLEDevice 이름 정의
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
	
	//초기 characteristic 값
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
```

BLE 장치는 초기의 characteristic 값을 “0”으로 세팅합니다. “0”은 스위치를 켜지 않는 행위를 의미합니다.  

- loop()

```cpp
void loop() {
	//characteristic값 가져오기
  std::string dataFromFlutter = pCharacteristic -> getValue();
  Serial.println(String(dataFromFlutter.c_str()));
  if (dataFromFlutter == "0") {//값이 "0"이면

    digitalWrite(ledPin, LOW); // LED를 끄는 코드

  } else if (dataFromFlutter == "1") {//값이 "1"이면
    
    digitalWrite(ledPin, HIGH); // LED를 켜는 코드

  }
  pCharacteristic -> setValue(dataFromFlutter);
  pCharacteristic -> notify();
  
  delay(1000);
}
```

loop()함수는 지속적으로 자신의 characteristic 값을 확인합니다. 그리고 확인된 값을 통해서 LED를 On/Off 함으로써, 스위치 동작을 나타냅니다.

- BLE 장치 연결 및 해제 CallBack

```cpp
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
		// 연결 상태 함수
    isConnected = true;

    std::string value = pCharacteristic->getValue();
    
		// 장치와 연결시 불이 꺼져있다면, 불을 킬 수 있도록 characteristic 값을 갱신.
    if (String(value.c_str()) == "0") {

    pCharacteristic->setValue("1");
    pCharacteristic->notify();
    }

  }

  void onDisconnect(BLEServer* pServer) {
		//해제 상태 함수
    isConnected = false;
		//연결이 해제되어 있다면 다시 advertising을 수행해서 클라이언트가 검색가능.
    BLEDevice::startAdvertising();
   
    std::string value = pCharacteristic->getValue();
		//연결이 해제될 때, 불이 켜져있다면 불일 끄도록 값을 갱신.
    if (String(value.c_str()) == "1") {
      pCharacteristic->setValue(0);
      pCharacteristic->notify();
    }
  }
};
```

위 콜백을 통해 BLE 장치가 연결 및 해제되었을 때 수행하는 기능을 정의합니다. 장치가 연결상태라면, characteristic 값을 가져와서 현재의 상태를 확인합니다. 만약, 불이 꺼져 있다면 불을 킬 수 있도록 characteristic 값을 갱신합니다. 장치가 연결 해제가 되면, advertising을 다시 수행하여 클라이언트가 검색할 수 있도록 하고, 현재의 characteristic값을 가져와서 현재의 상태가 불이 켜져 있다면, 불을 끄도록 characteristic값을 갱신합니다.
