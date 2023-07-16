#include <Arduino.h>
#include <SPI.h>
#include <GyverBME280.h>
#include <Wire.h>
#include <SD.h>
#include <Servo.h>


#define FLIGHT_TIME 120000  // milliseconds
#define FILE_SAVE 5000 // file will be closed and reopened every 5 seconds in case of troubles with logging
#define MPU_ADDR 0x68  //  mpu-6050 i2c address
#define BME_ADDR 0x76  // bme-280 i2c address
#define MPU_REGC 7  // mumber of registers to read from mpu-6050 through i2c
#define SD_CHIP_SELECT 10  //  CS pin for SD-card on arduino
#define SERVO_PWM 2  // servomotor PWM pin on arduino
#define SERVO_WORKING_ANGLE 0  //  put real parameter!
#define SERVO_RESCUE_ANGLE 180  //  put real parameter!
#define ALTITUDE_TRESHOLD 0.2  // put real parameter!
#define TAB "\t|"
#define ENDSTOP A3  // endstop switch pin
#define SWITCH_TOGGLE A7  //  switch sensor pin
#define BUZZER 3  //  buzzer pin



extern String data_str;
extern bool apogee_state;
extern unsigned long tmr1;  // data writer funtion timer
extern unsigned long tmr2;  //  apogee check function timer
extern unsigned long tmr3;  //  file save timer
extern Servo svp;  // servomotor object
extern File sd_file;  //  microSD card object
extern GyverBME280 bme_slave;
extern float buff_alt;  // buffer altitude variable for apogee check function 
extern float bme_alt;
extern bool setup_state;  // periphery setup state
extern int accumulate_start;  // switch sensor accumulator


bool Periphery_Setup();  // setting up mpu-6050 data transmission
void Get_data();  // reads mpu-6050 data and returns mpu_data array pointer
void WriteData();  //  writes data to the microSD 
void Apogee_check();  // checks for apogee state every second
void Rescue_deploy();  // deploy rescue system of rocket
void File_reopen();  // reopening logs file every 5 seconds to prevent from deleting
void Start_wait();  

