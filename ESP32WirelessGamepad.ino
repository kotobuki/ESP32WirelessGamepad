#include <M5Stack.h>
#include "GamepadBLE.h"
#include "Wire.h"

// Pointer to object providing the gamepad service via BLE
GamepadBLE *pGamepadBle = nullptr;

// BLE server object of this device
BLEServer *pServer = nullptr;

int redButtonPin = 26;
int blueButtonPin = 36;

#define JOYSTICK_I2C_ADDR 0x52

void setup() {
  M5.begin();
  M5.Power.begin();

  dacWrite(25, 0);

  pinMode(redButtonPin, INPUT);
  pinMode(blueButtonPin, INPUT);

  Wire.begin(21, 22, 400000);

  // Initialize bluetooth device
  BLEDevice::init(kGamepadDeviceInfo.deviceName);
  BLEDevice::setPower(ESP_PWR_LVL_P9);

  // Create BLE GATT server
  pServer = BLEDevice::createServer();

  pGamepadBle = GamepadBLE::getInstance();
  pGamepadBle->start(pServer, kGamepadDeviceInfo);

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(GREEN , BLACK);
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(0, 110);
  M5.Lcd.printf("Not connected");
}

bool wasConnected = false;

void loop() {
  M5.update();

  bool isConnected = pGamepadBle->isConnected();

  if (!wasConnected && isConnected) {
    M5.Lcd.setCursor(0, 110);
    M5.Lcd.print("Connected    ");
  } else if (wasConnected && !isConnected) {
    M5.Lcd.setCursor(0, 110);
    M5.Lcd.print("Disconnected ");
  }

  int isRedButtonPressed = (digitalRead(redButtonPin) == LOW);
  int isBlueButtonPressed = (digitalRead(blueButtonPin) == LOW);

  M5.Lcd.setCursor(0, 0);
  M5.Lcd.printf("Red  (as BTN_0): %d\nBlue (as BTN_1): %d",
                isRedButtonPressed,
                isBlueButtonPressed);

  uint8_t joystickA0;
  uint8_t joystickA1;
  uint8_t joystickA2;

  Wire.requestFrom(JOYSTICK_I2C_ADDR, 3);
  if (Wire.available()) {
    joystickA0 = Wire.read(); // X
    joystickA1 = Wire.read(); // Y
    joystickA2 = Wire.read(); // Button
  }

  // Convert from uint8 to int16
  int joystickPosX = map(joystickA0, 0, 255, -32768, 32767);
  int joystickPosY = map(joystickA1, 0, 255, -32768, 32767);

  M5.Lcd.setCursor(0, 200);
  M5.Lcd.printf("X: %+06d\nY: %+06d", joystickPosX, joystickPosY);

  M5.Lcd.setCursor(140, 200);
  M5.Lcd.print("BTN_2:");
  M5.Lcd.setCursor(140, 220);
  M5.Lcd.printf("%d", M5.BtnB.isPressed());

  M5.Lcd.setCursor(230, 200);
  M5.Lcd.print("BTN_3:");
  M5.Lcd.setCursor(230, 220);
  M5.Lcd.printf("%d", M5.BtnC.isPressed());

  if (isConnected) {
    // BTN_0
    pGamepadBle->setButtonA(isRedButtonPressed);

    // BTN_1
    pGamepadBle->setButtonB(isBlueButtonPressed);

    // BTN_2
//    pGamepadBle->setButtonY(M5.BtnB.isPressed());
    pGamepadBle->setButtonX(M5.BtnB.isPressed());

    // BTN_3
//    pGamepadBle->setButtonLB(M5.BtnC.isPressed());
    pGamepadBle->setButtonY(M5.BtnC.isPressed());

    pGamepadBle->setLeftStick(joystickPosX, joystickPosY);
    pGamepadBle->updateInputReport();
  }

  wasConnected = isConnected;

  delay(25);
}
