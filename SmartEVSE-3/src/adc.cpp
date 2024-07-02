#include "adc.h"

static esp_adc_cal_characteristics_t * adc_chars_CP;
static esp_adc_cal_characteristics_t * adc_chars_PP;
static esp_adc_cal_characteristics_t * adc_chars_Temperature;

volatile uint16_t adcsample = 0;
volatile uint8_t sampleidx = 0;
volatile uint16_t ADCsamples[ADCBUFFERSIZE];       // declared volatile, as they are used in a ISR


uint32_t getvoltage(uint32_t sample)
{
  return esp_adc_cal_raw_to_voltage(sample, adc_chars_CP);
}

uint32_t getADCsampleMin(void)
{
  uint32_t voltage, Min;
  for (uint8_t n = 0; n < ADCBUFFERSIZE; n++)
  {
    voltage = getvoltage(ADCsamples[n]);   // convert adc reading to voltage
    if (voltage < Min)
        Min = voltage; // store lowest value
  }
}

uint32_t getADCsampleMax(void)
{
  uint32_t voltage, Max;
  for (uint8_t n = 0; n < ADCBUFFERSIZE; n++)
  {
    voltage = getvoltage(ADCsamples[n]);   // convert adc reading to voltage
    if (voltage > Max)
        Max = voltage; // store highest value
  }
}
   

void sampleADC(void)
{
  RTC_ENTER_CRITICAL();
  adcsample = local_adc1_read(ADC1_CHANNEL_3);

  RTC_EXIT_CRITICAL();

  ADCsamples[sampleidx++] = adcsample;
  if (sampleidx == ADCBUFFERSIZE) sampleidx = 0;
}

// Some low level stuff here to setup the ADC, and perform the conversion.
//
//
uint16_t IRAM_ATTR local_adc1_read(int channel) {
    uint16_t adc_value;

    SENS.sar_read_ctrl.sar1_dig_force = 0;                      // switch SARADC into RTC channel 
    SENS.sar_meas_wait2.force_xpd_sar = SENS_FORCE_XPD_SAR_PU;  // adc_power_on
    RTCIO.hall_sens.xpd_hall = false;                           // disable other peripherals
    
    //adc_ll_amp_disable()  // Close ADC AMP module if don't use it for power save.
    SENS.sar_meas_wait2.force_xpd_amp = SENS_FORCE_XPD_AMP_PD;  // channel is set in the convert function
    // disable FSM, it's only used by the LNA.
    SENS.sar_meas_ctrl.amp_rst_fb_fsm = 0; 
    SENS.sar_meas_ctrl.amp_short_ref_fsm = 0;
    SENS.sar_meas_ctrl.amp_short_ref_gnd_fsm = 0;
    SENS.sar_meas_wait1.sar_amp_wait1 = 1;
    SENS.sar_meas_wait1.sar_amp_wait2 = 1;
    SENS.sar_meas_wait2.sar_amp_wait3 = 1; 

    // adc_hal_set_controller(ADC_NUM_1, ADC_CTRL_RTC);         //Set controller
    // see esp-idf/components/hal/esp32/include/hal/adc_ll.h
    SENS.sar_read_ctrl.sar1_dig_force       = 0;                // 1: Select digital control;       0: Select RTC control.
    SENS.sar_meas_start1.meas1_start_force  = 1;                // 1: SW control RTC ADC start;     0: ULP control RTC ADC start.
    SENS.sar_meas_start1.sar1_en_pad_force  = 1;                // 1: SW control RTC ADC bit map;   0: ULP control RTC ADC bit map;
    SENS.sar_touch_ctrl1.xpd_hall_force     = 1;                // 1: SW control HALL power;        0: ULP FSM control HALL power.
    SENS.sar_touch_ctrl1.hall_phase_force   = 1;                // 1: SW control HALL phase;        0: ULP FSM control HALL phase.

    // adc_hal_convert(ADC_NUM_1, channel, &adc_value);
    // see esp-idf/components/hal/esp32/include/hal/adc_ll.h
    SENS.sar_meas_start1.sar1_en_pad = (1 << channel);          // select ADC channel to sample on
    while (SENS.sar_slave_addr1.meas_status != 0);              // wait for conversion to be idle (blocking)
    SENS.sar_meas_start1.meas1_start_sar = 0;         
    SENS.sar_meas_start1.meas1_start_sar = 1;                   // start ADC conversion
    while (SENS.sar_meas_start1.meas1_done_sar == 0);           // wait (blocking) for conversion to finish
    adc_value = SENS.sar_meas_start1.meas1_data_sar;            // read ADC value from register

    return adc_value;
}


// Sample the Proximity Pin, and determine the maximum current the cable can handle.
//
void ProximityPin() 
{
    uint32_t sample, voltage;

    RTC_ENTER_CRITICAL();
    // Sample Proximity Pilot (PP)
    sample = local_adc1_read(ADC1_CHANNEL_6);
    RTC_EXIT_CRITICAL();

    voltage = esp_adc_cal_raw_to_voltage(sample, adc_chars_PP);

    if (!Config) {                                                          // Configuration (0:Socket / 1:Fixed Cable)
        //socket
        _LOG_A("PP pin: %u (%u mV)\n", sample, voltage);
    } else {
        //fixed cable
        _LOG_A("PP pin: %u (%u mV) (warning: fixed cable configured so PP probably disconnected, making this reading void)\n", sample, voltage);
    }

    MaxCapacity = 13;                                                       // No resistor, Max cable current = 13A
    if ((voltage > 1200) && (voltage < 1400)) MaxCapacity = 16;             // Max cable current = 16A	680R -> should be around 1.3V
    if ((voltage > 500) && (voltage < 700)) MaxCapacity = 32;               // Max cable current = 32A	220R -> should be around 0.6V
    if ((voltage > 200) && (voltage < 400)) MaxCapacity = 63;               // Max cable current = 63A	100R -> should be around 0.3V

    if (Config) MaxCapacity = MaxCurrent;                                   // Override with MaxCurrent when Fixed Cable is used.
}