#include <Base.h>

#define DEBUG

GyverBME280 bme_slave;  //  bme-280 object
File sd_file;  //  microSD card object
Servo svp;  // servomotor object

String data_str = "";
bool apogee_state = false;
bool setup_state = 1;
int accumulate_start = 0;
unsigned long tmr1 = 0;
unsigned long tmr2 = 0;
unsigned long tmr3 = 0;
float buff_alt = -100;
float bme_alt = 0;
float mpu_data[MPU_REGC] = {0, 0, 0, 0, 0, 0, 0};  //  [accX, accY, accZ, temp, gyrX, gyrY, gyrZ] from mpu-6050


bool Periphery_Setup()  // setting up mpu-6050 data transmission
{
  Serial.begin(9600);
  pinMode(ENDSTOP, INPUT_PULLUP);
  pinMode(SWITCH_TOGGLE, INPUT_PULLUP);
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);
  svp.attach(SERVO_PWM);
  svp.write(SERVO_RESCUE_ANGLE);
  #ifdef DEBUG
    Serial.println("***MPU-6050 SETUP - DONE***");
  #endif
  bme_slave.begin();
  #ifdef DEBUG
    Serial.println("***BME-280 SETUP - DONE***");
  #endif
  if(SD.begin(SD_CHIP_SELECT))
  {
    SD.remove("logs.txt");
    sd_file = SD.open("logs.txt", FILE_WRITE);
    tone(BUZZER, 1000, 1000);
    delay(500);
    #ifdef DEBUG
      Serial.println("**SD CARD SETUP - DONE***");
    #endif
    return 0;
  }
  else
  {
    tone(BUZZER, 200, 200);
    delay(500);
    tone(BUZZER, 200, 200);
    #ifdef DEBUG
      Serial.println("**SD CARD SETUP - FAILED***");
    #endif
    return 1;
  }
}

void Start_wait()
{
  while (!digitalRead(ENDSTOP))
  {
    tone(BUZZER, 1000, 200);
    delay(500);
    sd_file.print(float(millis()/1000));
    sd_file.println("WAIT FOR START");
    #ifdef DEBUG
      Serial.println("WAIT FOR START");
    #endif
  }
  delay(500);
  svp.write(SERVO_WORKING_ANGLE);
  #ifdef DEBUG
    Serial.println("RESCUE SYSTEM READY");
  #endif
  while(accumulate_start < 5000)
  {
    if (analogRead(SWITCH_TOGGLE)<= 0)
      accumulate_start = 0;
    else
    {
      accumulate_start += analogRead(SWITCH_TOGGLE);
      tone(2000, 100);
      delay(100);
      tone(2000, 100);
      delay(1000);    
    }
  }

}

void Get_data()  //  reads mpu-6050 data and returns mpu_data array pointer
{
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);  //  starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);  
  Wire.requestFrom(MPU_ADDR, 14, true); // request a total of 14 registers
  for (byte i = 0; i < MPU_REGC; i++)  // read data from registers
  {
    mpu_data[i] = Wire.read() << 8 | Wire.read();
    if (i < 3)
    mpu_data[i] = mpu_data[i] / 32768 * 2;
    else if(i > 3)
    mpu_data[i] = mpu_data[i] / 32768 * 250; 
  }
  bme_alt = pressureToAltitude(bme_slave.readPressure());
}





void WriteData()  //  writes data to the microSD 
{
  if (sd_file)
  {
    data_str= String(float(tmr1)/1000);
    data_str+= "|\ttmp: ";
    data_str+= String(bme_slave.readTemperature());
    data_str+= "|\thum: ";
    data_str+= String(bme_slave.readHumidity());  
    data_str+= "|\talt: ";
    data_str+= String(bme_alt);
    data_str+= "|\tacX: ";
    data_str+= String(mpu_data[0]);
    data_str+= "|\tacY: ";
    data_str+= String(mpu_data[1]);
    data_str+= "|\tacz: ";
    data_str+= String(mpu_data[2]);
    sd_file.print(data_str);
    data_str= "|\tgX: ";
    data_str+= String(mpu_data[4]);
    data_str+= "|\tgY: "; 
    data_str+= String(mpu_data[5]);
    data_str+= "|\tgZ: ";
    data_str+= String(mpu_data[6]);
    sd_file.print(data_str);
    sd_file.print("\n");
    #ifdef DEBUG
      Serial.println(data_str);
      Serial.println("LOGGED");
    #endif
  }
  else
  { 
    #ifdef DEBUG
      Serial.println("SD CARD FAILED OPENING FILE");
    #endif
  }
}

void Rescue_deploy()
{
 svp.write(SERVO_RESCUE_ANGLE);
  // sd_file.println();
  // sd_file.print(float(millis())/1000);
  // sd_file.print(":\t");
  // sd_file.println("RESCUE SYSTEM DEPLOYED!");
  // #ifdef DEBUG
  // Serial.print(float(millis())/1000);
  // Serial.print(":\t");
  // Serial.println("RESCUE SYSTEM DEPLOYED!");
//   #endif
}

void File_reopen()
  {
    sd_file.close();
    sd_file = SD.open("logs.txt", FILE_WRITE);
    #ifdef DEBUG
      Serial.println("FILE REOPENED...");
    #endif
  }

void Apogee_check()
{
  if (!apogee_state)
  {
    data_str= String(float(tmr2)/1000);
    data_str += "|\t";
    if (buff_alt - bme_alt >= ALTITUDE_TRESHOLD)
    {
      Rescue_deploy();
      apogee_state = true;
      data_str+= "*********************APOGEEE REACHED!*********************\t";
      #ifdef DEBUG
      Serial.println(data_str);
      #endif 
    }
    else
    {
      if (buff_alt < bme_alt)buff_alt = bme_alt;
      data_str+="APOGEE IS NOT REACHED YET";
    #ifdef DEBUG
      Serial.print("APOGEE IS NOT REACHED YET|\t");
      Serial.println(buff_alt);
    #endif 
    }
    sd_file.println(data_str);
  }

}