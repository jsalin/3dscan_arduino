// Simple program for rotating a stepper motor and taking photos with a
// IR remote controlled system camera.
// Copyright (C) 2016 Jussi Salin <salinjus@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "multiCameraIrControl.h"

#define STEPS_FOR_360 4096     // number of steps for the motor to make a full 360 degrees rotation
#define ENABLE_PIN A1          // pin for enabling stepper driver
#define DIR_PIN A2             // pin for direction signal
#define STEP_PIN A0            // pin for step signal
#define IR_PIN 2               // pin for IR LED
#define PHOTOS_TO_TAKE 60      // number of photos or frames to take during a full rotation
#define PRE_PHOTO_DELAY 1000   // ms to sleep before taking a photo (object stabilizes)
#define POST_PHOTO_DELAY 1000  // ms to sleep after taking a photo (camera lag)
#define STEP_DELAY 10          // ms to sleep between steps (to rotate at good speed)
#define STARTUP_DELAY 3000     // ms to sleep at the beginning
                               // (it might take time for user to remove hands after power up)
#define LED_PIN 13             // On while taking a photo, off when rotating
#define DUMMY_PHOTOS 3         // How many "dummy" photos in the beginning before rotation

void setup()
{
  Serial.begin(38400);
  
  // Configure stepper motor for operation
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, LOW);
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(DIR_PIN, LOW);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("Booted up");
}
 
void loop()
{
  Canon Camera(IR_PIN);

  delay(STARTUP_DELAY);
  Serial.println("Taking few photos to calibrate camera");
  for (int i=0; i<DUMMY_PHOTOS; i++)
  {
    delay(PRE_PHOTO_DELAY);
    Camera.shutterNow();
    delay(POST_PHOTO_DELAY);
  }

  delay(STARTUP_DELAY);
  Serial.println("Begin scan");

  // Loop for full rotation of taking photos
  for (int i=0; i<PHOTOS_TO_TAKE; i++)
  {
    digitalWrite(LED_PIN, HIGH);
    Serial.print("Taking photo #");
    Serial.println(i);
    // Take photo
    delay(PRE_PHOTO_DELAY);
    Camera.shutterNow();
    delay(POST_PHOTO_DELAY);
    digitalWrite(LED_PIN, LOW);

    // Rotate
    unsigned long steps_to_take = STEPS_FOR_360/2/PHOTOS_TO_TAKE;
    for (unsigned long j=0; j<steps_to_take; j++)
    {
      //Serial.print(".");
      digitalWrite(STEP_PIN, LOW);
      delay(STEP_DELAY);
      digitalWrite(STEP_PIN, HIGH);
      delay(STEP_DELAY);
    }
    //Serial.println();
  }

  Serial.println("Done!");

  // Completed - disable motor driver
  digitalWrite(ENABLE_PIN, HIGH);

  // Don't do anything till user powers off or presses reset for a new rotation
  for(;;);
}
