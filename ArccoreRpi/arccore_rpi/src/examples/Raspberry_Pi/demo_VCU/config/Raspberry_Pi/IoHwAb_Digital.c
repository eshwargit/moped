/*
 * This code is not automatically generated.
 * Because Pi dose not have the IoHw layer
 * in the ArcCore tools.
 *
 *  Created on: 28 okt 2014
 *      Author: sse
 */
#include <stdio.h>

#include "Platform_Types.h"
#include "IoHwAb_Digital.h"
#include "Sensors.h"

#include "MOPED_DEBUG.h"

#if defined(USE_DIO)
#include "Dio.h"
#endif

#if defined(USE_PWM)
#include "Pwm.h"
#endif

#include "bcm2835.h"

#define PI 	   			3.1415926f

//TODO Should this conversion be done in IoHwAb or in SensorSw-C?
/**
 * Convert wheel speed from pulse counter to cm/s
 *
 * @param wheel				----- wheel type (front or rear)
 * @param startTime			----- pointer to the time since the counter started from zero
 */
static uint32 IoHwAb_Digital_CalcWheelSpeed(enum Wheel wheel, uint64 *startTime) {
	uint32 time;
	float distance, speed;
	uint32 nrPulses = Sensors_GetWheelPulse(wheel);

	distance = PI * WHEEL_DIAMETER * nrPulses / PULSES_PER_WHEEL_TURN;
	time = (uint32)(CURRENT_TIME - *startTime);
	speed = 1e6 * distance / time; //speed in cm/s

	Sensors_ResetWheelPulse(wheel);
	*startTime = CURRENT_TIME;

	return (uint32)speed;
}

//TODO: Maybe another name, since MCP3008 has 8 channels and this one specifically reads battery,
// 		e.g. IoHwAb_BatterySensorRead(0, Data) or even IoHwAb_Digital_Get(BATTERY_SENSOR, Data)
/**
 * Get the battery voltage value
 *
 * @param portDefArg1		----- not used
 * @param Data				----- pointer for data storage
 * @return status			----- read status (e.g. OK or not OK)
 */
Std_ReturnType IoHw_Read_AdcSensor(/*IN*/uint32 portDefArg1, /*IN*/uint32* Data){
	Mcp3008_Read(MCP3008_BATTERY_CH, Data);
#if ADC_PRINT_DEBUG
	printf("infor: IoHW adc = %lu\r\n", *Data);
#endif
    return E_OK;
}

//TODO: Prefix the function name with IoHwAb_ (or even convert it to IoHwAb_Digital_Get(FRONT_WHEEL_SENSOR, Data))
//		However, this requires regeneration of the project
/**
 * Get the front wheel speed
 *
 * @param portDefArg1		----- not used
 * @param Data				----- pointer for data storage
 * @return status			----- read status (e.g. OK or not OK)
 */
Std_ReturnType IoHw_Read_FrontWheelSensor(/*IN*/uint32 portDefArg1, /*IN*/uint32* Data){
	static uint64 startTime;
	*Data = IoHwAb_Digital_CalcWheelSpeed(FRONT_WHEEL, &startTime);

//	printf("In IoHw_Read_RearWheelSensor, data = %lu cm\r\n", *Data);

#if WHEEL_PRINT_DEBUG
		printf("infor: IoHW front wheel speed = %lucm/s\r\n", *Data);
#endif

    return E_OK;
}

//TODO: As above
/**
 * Get the rear wheel speed
 *
 * @param portDefArg1		----- not used
 * @param Data				----- pointer for data storage
 * @return status			----- read status (e.g. OK or not OK)
 */
Std_ReturnType IoHw_Read_RearWheelSensor(/*IN*/uint32 portDefArg1, /*IN*/uint32* Data){
	static uint64 startTime;
	*Data = IoHwAb_Digital_CalcWheelSpeed(REAR_WHEEL, &startTime);

//	printf("In IoHw_Read_RearWheelSensor, data = %lu cm\r\n", *Data);

#if  WHEEL_PRINT_DEBUG
		printf("infor: IoHW rear speed = %lucm/s\r\n", *Data);
#endif

	return E_OK;
}


Std_ReturnType IoHw_WriteSpeed_DutyCycle(/*IN*/uint32 portDefArg1, /*OUT*/uint8 * DutyCycle){

	uint32 speed = (int) ((100 + (0.55556 * ((signed char)*DutyCycle + 100) * 0.9)) * 16.38);

	Pwm_SetPeriodAndDuty(1, 20000, speed);

#if SPEED_PRINT_DEBUG
    printf("infor: write speed data %d to PWM driver: \r\n", speed);
#endif

	return E_OK;
}

Std_ReturnType IoHw_WriteServo_DutyCycle(/*IN*/uint32 portDefArg1, /*OUT*/uint8 * DutyCycle){
	uint32 steer = (int) ((100 + (0.55556 * ((signed char)*DutyCycle + 100) * 0.9)) * 16.38);

	Pwm_SetPeriodAndDuty(0, 20000, steer);

#if SERVO_PRINT_DEBUG
	printf("infor: write servo data %d to PWM driver: \r\n", steer);
#endif

	return E_OK;
}

Std_ReturnType IoHw_Write_Led(/*IN*/uint32 portDefArg1, /*IN*/uint8 portDefArg2, /*OUT*/uint32 * Pin, /*OUT*/uint8 * Level){

	uint8 LED_State = *Level;
	uint32 LED_Pin = *Pin;
	printf("infor: IoHw pin %d, level %d \r\n", LED_Pin, LED_State);
	switch (LED_Pin) {

	case GPIO_LED_RED:
		if (LED_State == 0) {
//			bcm2835_SetPinInTwinGpioReg(&GPCLR0, LED_RED);
		} else {
//			bcm2835_SetPinInTwinGpioReg(&GPSET0, LED_RED);
		}
		break;

	case GPIO_LED_YELLOW1:
		if (LED_State == 0) {
//			bcm2835_SetPinInTwinGpioReg(&GPCLR0, LED_YELLOW1);
		} else {
//			bcm2835_SetPinInTwinGpioReg(&GPSET0, LED_YELLOW1);
		}
		break;

	case GPIO_LED_YELLOW2:
		if (LED_State == 0) {
//			bcm2835_SetPinInTwinGpioReg(&GPCLR0, LED_YELLOW2);
		} else {
//			bcm2835_SetPinInTwinGpioReg(&GPSET0, LED_YELLOW2);
		}
		break;
	default:
		break;
	}

	return E_OK;
}