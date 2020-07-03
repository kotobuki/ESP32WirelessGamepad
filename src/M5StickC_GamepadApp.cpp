/**
    M5StickC_GamepadApp:
    This application has been developed to use an M5StickC device (ESP32)
    as bluetooth gamepad input device. It reads a joystick position and
    button status and provides these data via Bluetooth Low Energy (BLE)
    using the Human Interface Device (HID) via GATT protocol.

    Copyright (C) 2020 by Ernst Sikora
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Arduino.h>
#include <M5StickC.h>
#include <Wire.h>

#include "GamepadBLE.h"
#include "M5StickC_GamepadIO.h"

#include "AXP192_BLEService.h"
#include "M5StickC_PowerManagement.h"

// Bluetooth icon in RGB565 format and 16x24 size
static const uint16_t image_data_icon_bluetooth[384] = {
    0x0000, 0x0000, 0x0000, 0x0193, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x0193, 0x0000, 0x0000, 0x0000, 
    0x0000, 0x0000, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x0000, 0x0000, 
    0x0000, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x1252, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x0000, 
    0x0212, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0xadfb, 0x2273, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x0231, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0xcebd, 0xcebd, 0x1232, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0xcebd, 0xffff, 0xbe3b, 0x0a12, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0xcebd, 0xbe5c, 0xf7bf, 0x9d9a, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x2ab3, 0x09f2, 0x09f2, 0xcebd, 0x8d39, 0x53b6, 0xffff, 0x7cd8, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x53b6, 0xf7df, 0x4bb6, 0x09f2, 0xcebd, 0x8d39, 0x09f2, 0x9d9a, 0xffff, 0x4355, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x9d7a, 0xffdf, 0x53d6, 0xcebd, 0x8d39, 0x6416, 0xffff, 0x9539, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x9d9a, 0xffdf, 0xef9e, 0xdf1d, 0xffff, 0x8519, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x9d7a, 0xffff, 0xffff, 0x7cd8, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x7cd8, 0xffff, 0xffff, 0x53d6, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x84f9, 0xffff, 0xf7bf, 0xef9e, 0xffdf, 0x5bd6, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x8519, 0xffff, 0x6437, 0xcebd, 0x8d39, 0x8d19, 0xffff, 0x6417, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x53b6, 0xffdf, 0x5bf6, 0x09f2, 0xcebd, 0x8d39, 0x09f2, 0xa5ba, 0xffff, 0x3b35, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x32f4, 0x09f2, 0x09f2, 0xcebd, 0x8d39, 0x2293, 0xe77e, 0xb63b, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0xcebd, 0x957a, 0xcebd, 0xd6fd, 0x1a52, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0xcebd, 0xffdf, 0xef7e, 0x2ab3, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0xcebd, 0xf7df, 0x4355, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 
    0x0212, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0xcebc, 0x6416, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x0231, 
    0x0000, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x53d6, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x0000, 
    0x0000, 0x0000, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f1, 0x0000, 0x0000, 
    0x0000, 0x0000, 0x0000, 0x01d1, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x09f2, 0x01d1, 0x0000, 0x0000, 0x0000
};

// Type definition: Struct for image data
typedef struct {
    const uint16_t *data;
    uint16_t width;
    uint16_t height;
    uint8_t dataSize;
} tImage;

// Struct with image data of the bluetooth icon
static const tImage icon_bluetooth = { image_data_icon_bluetooth, 16, 24, 16 };

// Object providing the gamepad service via BLE
GamepadBLE gamepadBle;

// Object providing convenient access to gamepad peripherals
M5StickC_GamepadIO *pGamepadIO = nullptr;

// Object providing power management information via BLE
AXP192_BLEService axp192Ble;

// Object providing utility functions for accessing power management data
M5StickC_PowerManagement axp192PowMan;


/**
 * The execution of the loop function is structured into cycles where each cycle comprises a fixed number of slots.
 * The numner of the slot determines which sub functions are executed.
 * Some sub functions are executed in every slot, others are executed e.g. only in the last slot of each cycle.
 */

// Number of slots in each cycle
static const uint16_t kNumSlots = 200;

// Number of the current slot of the current cycle
uint16_t curSlotNr = 0;

// Nominal duration of each slot in microseconds. Time that is not used up in a slot is spent by calling the delay function.
static const uint32_t slotTimeMicros = 25000;

/**
 * Statistical data the helps to analyse computation times of the loop function.
 */

// Start time of loop function in microseconds
uint64_t timeStartMicros = 0;

// End time of loop function in microseconds
uint64_t timeEndMicros = 0;

// Duration of loop function in microseconds
uint32_t timeDeltaMicros = 0;

// Maximum duration of a slot within the current cycle
uint32_t timeDeltaMaxInCycleMicros = 0;

// Overall maximum duration of a slot throughout all cycles
uint32_t timeDeltaMaxMicros = 0;


// Buffer to store debug output for printing to Serial.
char strOut[200];


// BLE server object of this device
BLEServer *pServer = nullptr;

/**
 * Initializes the BLE device and BLE server. 
 */
void setupBLE()
{
    // Initialize bluetooth device
    std::string deviceStr = "ESP32 BLE Controller";
    BLEDevice::init(deviceStr);
    BLEDevice::setPower(ESP_PWR_LVL_P9);

    // Create BLE GATT server
    pServer = BLEDevice::createServer();
}

void setup()
{
    M5.begin();

    axp192PowMan.start();

    pGamepadIO = M5StickC_GamepadIO::getInstance();
    pGamepadIO->start();

    M5.Lcd.setTextFont(4);
    M5.Lcd.setCursor(70, 0, 4);
    M5.Lcd.println(("Joy"));

    setupBLE();

    gamepadBle.start(pServer);
    axp192Ble.start(pServer);
}

/**
 * Processes power management data.
 * Among other things determines the battery level of the gamepad.
 */
void processAxp()
{
    // Read and process AXP192 data
    axp192PowMan.readAndProcessData();
    
    // Debug output
    axp192PowMan.printStatusToString(strOut);
    Serial.println(strOut);

    // Set battery level of gamepad
    uint8_t batLvlPerc = (uint8_t) axp192PowMan.getBatteryLevelPercent();
    gamepadBle.updateBatteryLevel(batLvlPerc);

    // Update AXP192 BLE service data
    axp192Ble.setBatVoltage( axp192PowMan.getBatVoltage() );
    axp192Ble.setBatPower( axp192PowMan.getBatPower() );
    axp192Ble.setBatChargeCurrent( axp192PowMan.getBatChargeCurrent() );
    axp192Ble.setCoulombData( axp192PowMan.getCoulombData() );
    axp192Ble.setCurrentDirection( axp192PowMan.getCurrentDirection() );
    axp192Ble.setACInPresent( axp192PowMan.isACInPresent() );
    axp192Ble.setACInAvailable( axp192PowMan.isACInPresent() );
    axp192Ble.setVBusPresent( axp192PowMan.isVBusPresent() );
    axp192Ble.setVBusAvailable( axp192PowMan.isVBusAvailable() );
}

/**
 * Prints the current date and time from the RTC (real time clock) to Serial.
 */
void printRtcTimestamp()
{
    RTC_TimeTypeDef RTC_TimeStruct;
    RTC_DateTypeDef RTC_DateStruct;

    M5.Rtc.GetTime(&RTC_TimeStruct);
    M5.Rtc.GetData(&RTC_DateStruct);
    
    sprintf(strOut, "%04d-%02d-%02d, %02d:%02d:%02d: ",
        RTC_DateStruct.Year, RTC_DateStruct.Month, RTC_DateStruct.Date,
        RTC_TimeStruct.Hours, RTC_TimeStruct.Minutes, RTC_TimeStruct.Seconds);

    Serial.print(strOut);
}

void processGamepadControls()
{
    pGamepadIO->process();

    // Transform positions to output range
    GamepadBLE::StickAxis_t joyScaledX = pGamepadIO->getJoyNormX() << 8;
    GamepadBLE::StickAxis_t joyScaledY = pGamepadIO->getJoyNormY() << 8;

    // Set left stick axis values
    gamepadBle.setLeftStick(joyScaledX, joyScaledY);

    // Set stick button state
    gamepadBle.setLeftStickButton( pGamepadIO->isJoyPressed() );

    // Set A and B button
    gamepadBle.setButtonA( pGamepadIO->isBtnBluePressed() );
    gamepadBle.setButtonB( pGamepadIO->isBtnRedPressed() );

    // Send data to host device
    gamepadBle.updateInputReport();
}

void loop()
{
    timeStartMicros = micros();

    //Serial.println("[V][M5StickC_GamepadApp.cpp] loop(): >> loop");
    //Serial.flush();

    /* ----- Read gamepad controls and send to host ------ */
    
    // Do every second slot
    processGamepadControls();

    /* ----- Update display ----- */

    // Do every second slot
    if (curSlotNr % 2 == 0)
    {
        M5.Lcd.setCursor(100, 50, 4);
        M5.Lcd.printf("X:%d      ", pGamepadIO->getJoyNormX());
        M5.Lcd.setCursor(100, 80, 4);
        M5.Lcd.printf("Y:%d      ", pGamepadIO->getJoyNormY());
        M5.Lcd.setCursor(100, 110, 4);
        M5.Lcd.printf("%d%d%d", pGamepadIO->isJoyPressed(), pGamepadIO->isBtnBluePressed(), pGamepadIO->isBtnRedPressed());
    }

    // Do in slot 1, 21 etc.
    if (curSlotNr % 20 == 1)
    {
        // If there is a bluetooth connection show the bluetooth icon on the display
        if (gamepadBle.isConnected()) {
            
            uint16_t i = 0;

            for (uint16_t imgY = 0; imgY < icon_bluetooth.height; ++imgY)
            {
                for (uint16_t imgX = 0; imgX < icon_bluetooth.width; ++imgX)
                {
                    M5.Lcd.drawPixel(imgX, imgY, icon_bluetooth.data[i]);
                    ++i;
                }
            }
        }
        else {
            // Draw rectangle with background colour
            M5.Lcd.fillRect(0, 0, 16, 24, 0);
        }
    }

    /* ----- Update battery status ----- */

    // Do in slot 3 every 100 slots
    if (curSlotNr % 100 == 3)
    {
        printRtcTimestamp();
        processAxp();
    }

    /* ----- Print statistics about computation time ----- */
    
    // Do in last slot of each cycle
    if (curSlotNr == kNumSlots - 1)
    {
        sprintf(strOut, "Duration of loop execution in microseconds: %d (last), %d (max in cylce), %d (max overall)",
                timeDeltaMicros,
                timeDeltaMaxInCycleMicros,
                timeDeltaMaxMicros);

        Serial.println(strOut);
    }
    
    /* ----- Update slot number and compute duration stats ----- */
    
    if (curSlotNr == 0) {
        timeDeltaMaxInCycleMicros = 0;
    }

    curSlotNr = (curSlotNr + 1) % kNumSlots;

    //Serial.println("[V][M5StickC_GamepadApp.cpp] loop(): << loop");
    //Serial.flush();

    /* ----- Wait until next cycle ----- */
    
    timeEndMicros = micros();

    // Compute duration of loop
    timeDeltaMicros = timeEndMicros - timeStartMicros;

    // Update maximum slot duration within cycle
    if (timeDeltaMicros > timeDeltaMaxInCycleMicros)
    {
        timeDeltaMaxInCycleMicros = timeDeltaMicros;

        // Update overall maximum slot duration
        if (timeDeltaMicros > timeDeltaMaxMicros)
        {
            timeDeltaMaxMicros = timeDeltaMicros;
        }
    }

    // Print warning when slot time has been exceed
    if (timeDeltaMicros < slotTimeMicros) {
        delayMicroseconds(slotTimeMicros - timeDeltaMicros);
    }
    else {
        sprintf(strOut, "[W][M5StickC_GamepadApp.cpp] loop(): Duration of loop greater than cycle time: %d microseconds.", timeDeltaMicros);
        Serial.println(strOut);
    }
    
}