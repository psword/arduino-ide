/***************************************************************************
* Example sketch for the ADS1115_WE library
*
* This sketch shows how to use the ADS1115 in continuous mode. 
*  
* Further information can be found on:
* https://wolles-elektronikkiste.de/ads1115 (German)
* https://wolles-elektronikkiste.de/en/ads1115-a-d-converter-with-amplifier (English)
* 
***************************************************************************/

#include<ADS1115_WE.h> 
#include<Wire.h>
#define I2C_ADDRESS_1 0x4A
#define I2C_ADDRESS_2 0x4B

float acidVoltage    = 1022.25;    //buffer solution 4.0 at 25C
float neutralVoltage = 755.25;     //buffer solution 7.0 at 25C
float baseVoltage    = 502.50;      //buffer solution 10.0 at 25C
float voltage = 0.0;

/* There are several ways to create your ADS1115_WE object:
 * ADS1115_WE adc = ADS1115_WE(); -> uses Wire / I2C Address = 0x48
 * ADS1115_WE adc = ADS1115_WE(I2C_ADDRESS); -> uses Wire / I2C_ADDRESS
 * ADS1115_WE adc = ADS1115_WE(&Wire); -> you can pass any TwoWire object / I2C Address = 0x48
 * ADS1115_WE adc = ADS1115_WE(&Wire, I2C_ADDRESS); -> all together
 */
ADS1115_WE adc_1 = ADS1115_WE(I2C_ADDRESS_1);
ADS1115_WE adc_2 = ADS1115_WE(I2C_ADDRESS_2);

void setup() {
  Wire.begin();
  Serial.begin(9600);
  if(!adc_1.init()){
    Serial.println("ADS1115 not connected!");
  }
  if(!adc_2.init()){
    Serial.println("ADS1115 not connected!");
  }
  pinMode(23, OUTPUT);
  digitalWrite(23, HIGH);
  pinMode(19, OUTPUT);
  digitalWrite(19, HIGH);
  /* Set the voltage range of the ADC to adjust the gain
   * Please note that you must not apply more than VDD + 0.3V to the input pins!
   * 
   * ADS1115_RANGE_6144  ->  +/- 6144 mV
   * ADS1115_RANGE_4096  ->  +/- 4096 mV
   * ADS1115_RANGE_2048  ->  +/- 2048 mV (default)
   * ADS1115_RANGE_1024  ->  +/- 1024 mV
   * ADS1115_RANGE_0512  ->  +/- 512 mV
   * ADS1115_RANGE_0256  ->  +/- 256 mV
   */
  adc_1.setVoltageRange_mV(ADS1115_RANGE_4096); //comment line/change parameter to change range
  adc_2.setVoltageRange_mV(ADS1115_RANGE_4096);
  /* Set the inputs to be compared
   *  
   *  ADS1115_COMP_0_1    ->  compares 0 with 1 (default)
   *  ADS1115_COMP_0_3    ->  compares 0 with 3
   *  ADS1115_COMP_1_3    ->  compares 1 with 3
   *  ADS1115_COMP_2_3    ->  compares 2 with 3
   *  ADS1115_COMP_0_GND  ->  compares 0 with GND
   *  ADS1115_COMP_1_GND  ->  compares 1 with GND
   *  ADS1115_COMP_2_GND  ->  compares 2 with GND
   *  ADS1115_COMP_3_GND  ->  compares 3 with GND
   */
  adc_1.setCompareChannels(ADS1115_COMP_0_GND); //comment line/change parameter to change channel
  adc_2.setCompareChannels(ADS1115_COMP_0_GND);
  /* Set number of conversions after which the alert pin asserts
   * - or you can disable the alert 
   *  
   *  ADS1115_ASSERT_AFTER_1  -> after 1 conversion
   *  ADS1115_ASSERT_AFTER_2  -> after 2 conversions
   *  ADS1115_ASSERT_AFTER_4  -> after 4 conversions
   *  ADS1115_DISABLE_ALERT   -> disable comparator / alert pin (default) 
   */
  //adc.setAlertPinMode(ADS1115_ASSERT_AFTER_1); //uncomment if you want to change the default

  /* Set the conversion rate in SPS (samples per second)
   * Options should be self-explaining: 
   * 
   *  ADS1115_8_SPS 
   *  ADS1115_16_SPS  
   *  ADS1115_32_SPS 
   *  ADS1115_64_SPS  
   *  ADS1115_128_SPS (default)
   *  ADS1115_250_SPS 
   *  ADS1115_475_SPS 
   *  ADS1115_860_SPS 
   */
  // adc.setConvRate(ADS1115_8_SPS); //uncomment if you want to change the default

  /* Set continuous or single shot mode:
   * 
   *  ADS1115_CONTINUOUS  ->  continuous mode
   *  ADS1115_SINGLE     ->  single shot mode (default)
   */
  adc_1.setMeasureMode(ADS1115_CONTINUOUS); //comment line/change parameter to change mode
  adc_2.setMeasureMode(ADS1115_CONTINUOUS);

   /* Choose maximum limit or maximum and minimum alert limit (window) in Volt - alert pin will 
   *  assert when measured values are beyond the maximum limit or outside the window 
   *  Upper limit first: setAlertLimit_V(MODE, maximum, minimum)
   *  In max limit mode the minimum value is the limit where the alert pin assertion will be  
   *  cleared (if not latched)  
   * 
   *  ADS1115_MAX_LIMIT
   *  ADS1115_WINDOW
   * 
   */
  //adc.setAlertModeAndLimit_V(ADS1115_MAX_LIMIT, 3.0, 1.5); //uncomment if you want to change the default
  
  /* Enable or disable latch. If latch is enabled the alert pin will assert until the
   * conversion register is read (getResult functions). If disabled the alert pin assertion will be
   * cleared with next value within limits. 
   *  
   *  ADS1115_LATCH_DISABLED (default)
   *  ADS1115_LATCH_ENABLED
   */
  //adc.setAlertLatch(ADS1115_LATCH_ENABLED); //uncomment if you want to change the default

  /* Sets the alert pin polarity if active:
   *  
   * ADS1115_ACT_LOW  ->  active low (default)   
   * ADS1115_ACT_HIGH ->  active high
   */
  //adc.setAlertPol(ADS1115_ACT_LOW); //uncomment if you want to change the default
 
  /* With this function the alert pin will assert, when a conversion is ready.
   * In order to deactivate, use the setAlertLimit_V function  
   */
  //adc.setAlertPinToConversionReady(); //uncomment if you want to change the default

  Serial.println("ADS1115 Example Sketch - Continuous Mode");
  Serial.println("All values in volts");
  Serial.println();
}

  /* If you change the compare channels you can immediately read values from the conversion 
   * register, although they might belong to the former channel if no precautions are taken. 
   * It takes about the time needed for two conversions to get the correct data. In single 
   * shot mode you can use the isBusy() function to wait for data from the new channel. This 
   * does not work in continuous mode. 
   * To solve this issue the library adds a delay after change of channels if you are in contunuous
   * mode. The length of the delay is adjusted to the conversion rate. But be aware that the output 
   * rate will be much lower that the conversion rate if you change channels frequently. 
   */

void loop() {
  float voltage_1 = 0.0;
  float voltage_2 = 0.0;

  Serial.print("ADC2, 0: ");
  voltage_2 = adc_2.getResult_mV(); // alternative: getResult_mV for Millivolt
  Serial.println(voltage_1);

  // Serial.print(",   1: ");
  // voltage = readChannel(ADS1115_COMP_1_GND);
  // Serial.print(voltage);
  
  // Serial.print(",   2: ");
  // voltage = readChannel(ADS1115_COMP_2_GND);
  // Serial.print(voltage);

  // Serial.print(",   3: ");
  // voltage = readChannel(ADS1115_COMP_3_GND);
  // Serial.println(voltage);
  float slope = ((7.0-4.0)/(neutralVoltage - acidVoltage) + (7-10.0)/(neutralVoltage - baseVoltage))/2;  // three point: (neutralVoltage,7.0),(acidVoltage,4.0),(baseVoltage,10.0)
  float intercept =  7.0 - (slope*neutralVoltage);
  Serial.print("slope:");
  Serial.print(slope);
  Serial.print(",intercept:");
  Serial.println(intercept);
  float phValue = (slope*voltage_2)+intercept;  //y = k*x + b
  
  Serial.println(phValue);
  delay(1000);

  Serial.print("ADC1, 0: ");
  voltage_1 = adc_1.getResult_mV(); // alternative: getResult_mV for Millivolt
  Serial.println(voltage_1);
}


