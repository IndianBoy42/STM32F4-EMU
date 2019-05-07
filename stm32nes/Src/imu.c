#include "imu.h"
#include <math.h>
#define IMU_SPI SPI2

typedef enum {
	CHANNEL_COMMAND,
	CHANNEL_EXECUTABLE,
	CHANNEL_CONTROL,
	CHANNEL_REPORTS,
	CHANNEL_WAKE_REPORTS,
	CHANNEL_GYRO,
}CHANNEL_ENUM;


uint8_t shtpHeader[4]; //Each packet has a header of 4 bytes
uint8_t shtpData[MAX_PACKET_SIZE];
uint8_t sequenceNumber[6] = {0, 0, 0, 0, 0, 0}; //There are 6 com channels. Each channel has its own seqnum
uint8_t commandSequenceNumber = 0;				//Commands have a seqNum as well. These are inside command packet, the header uses its own seqNum per channel
uint32_t metaData[MAX_METADATA_SIZE];			//There is more than 10 words in a metadata record but we'll stop at Q point 3


//These are the raw sensor values pulled from the user requested Input Report
uint16_t rawAccelX, rawAccelY, rawAccelZ, accelAccuracy;
uint16_t rawLinAccelX, rawLinAccelY, rawLinAccelZ, accelLinAccuracy;
uint16_t rawGyroX, rawGyroY, rawGyroZ, gyroAccuracy;
uint16_t rawMagX, rawMagY, rawMagZ, magAccuracy;
uint16_t rawQuatI, rawQuatJ, rawQuatK, rawQuatReal, rawQuatRadianAccuracy, quatAccuracy;
uint16_t stepCount;
uint32_t timeStamp;
uint8_t stabilityClassifier;
uint8_t activityClassifier;
uint8_t *_activityConfidences; //Array that store the confidences of the 9 possible activities
uint8_t calibrationStatus;	 //Byte R0 of ME Calibration Response

//These Q values are defined in the datasheet but can also be obtained by querying the meta data records
//See the read metadata example for more info
int16_t rotationVector_Q1 = 14;
int16_t accelerometer_Q1 = 8;
int16_t linear_accelerometer_Q1 = 8;
int16_t gyro_Q1 = 9;
int16_t magnetometer_Q1 = 4;

uint8_t IMU_transfer(uint8_t byte) {
	while (!(IMU_SPI->SR & SPI_SR_TXE));
	LL_SPI_TransmitData8(IMU_SPI,byte);
	while (!(IMU_SPI->SR & SPI_SR_RXNE));
	return LL_SPI_ReceiveData8(IMU_SPI);
}

uint8_t IMU_receivePacket(void) {
	if (gpio_read(APU_IRQ)) return 0; //Data is not available

	//Old way: if (waitForSPI() == false) return (false); //Something went wrong

	//Get first four bytes to find out how much data we need to read
	gpio_reset(APU_NSS);

	//Get the first four bytes, aka the packet header
	uint8_t packetLSB = IMU_transfer(0);
	uint8_t packetMSB = IMU_transfer(0);
	uint8_t channelNumber = IMU_transfer(0);
	uint8_t sequenceNumber = IMU_transfer(0); //Not sure if we need to store this or not

	//Store the header info
	shtpHeader[0] = packetLSB;
	shtpHeader[1] = packetMSB;
	shtpHeader[2] = channelNumber;
	shtpHeader[3] = sequenceNumber;

	//Calculate the number of data bytes in this packet
	int16_t dataLength = ((uint16_t)packetMSB << 8 | packetLSB);
	dataLength &= ~(1 << 15); //Clear the MSbit.
	//This bit indicates if this package is a continuation of the last. Ignore it for now.
	//TODO catch this as an error and exit
	if (dataLength == 0) return 0; //Packet is empty
	dataLength -= 4; //Remove the header bytes from the data count

	//Read incoming data into the shtpData array
	for (uint16_t dataSpot = 0; dataSpot < dataLength; dataSpot++)
	{
		uint8_t incoming = IMU_transfer(0xFF);
		if (dataSpot < MAX_PACKET_SIZE)	//BNO080 can respond with upto 270 bytes, avoid overflow
			shtpData[dataSpot] = incoming; //Store data into the shtpData array
	}
	gpio_set(APU_NSS);

	return 1;
}

uint8_t IMU_waitForSPI(void){
	for (uint8_t i=0; i<125;i++){
		if (!gpio_read(APU_IRQ)) return 1;
		delay(1);
	}
	return 0;
}

uint8_t IMU_sendPacket(uint8_t channelNumber, uint8_t dataLength)
{
	uint8_t packetLength = dataLength + 4; //Add four bytes for the header
	//Wait for BNO080 to indicate it is available for communication
	if (IMU_waitForSPI() == 0) return 0; //Something went wrong

	gpio_reset(APU_NSS);

	//Send the 4 byte packet header
	IMU_transfer(packetLength & 0xFF);			 //Packet length LSB
	IMU_transfer(packetLength >> 8);				 //Packet length MSB
	IMU_transfer(channelNumber);					 //Channel number
	IMU_transfer(sequenceNumber[channelNumber]++); //Send the sequence number, increments with each packet sent, different counter for each channel

	//Send the user's data packet
	for (uint8_t i = 0; i < dataLength; i++)
	{
		IMU_transfer(shtpData[i]);
	}
	gpio_set(APU_NSS);
 	return 1;
}

uint8_t IMU_dataAvailable(void){
	//If we have an interrupt pin connection available, check if data is available.
	//If int pin is NULL, then we'll rely on receivePacket() to timeout
	//See issue 13: https://github.com/sparkfun/SparkFun_BNO080_Arduino_Library/issues/13
	if (gpio_read(APU_IRQ)) return 2;

	if (IMU_receivePacket()){
		//Check to see if this packet is a sensor reporting its data to us
		if (shtpHeader[2] == CHANNEL_REPORTS && shtpData[0] == SHTP_REPORT_BASE_TIMESTAMP)
		{
			IMU_parseInputReport(); //This will update the rawAccelX, etc variables depending on which feature report is found
			return 1;
		}
		else if (shtpHeader[2] == CHANNEL_CONTROL)
		{
			IMU_parseCommandReport(); //This will update responses to commands, calibrationStatus, etc.
			return 1;
		}
	}
	return 0;
}

uint8_t IMU_init(void) {
	LL_SPI_Enable(IMU_SPI);

	gpio_set(APU_NSS);
	gpio_set(APU_IO1);
	gpio_set(APU_IO2);

	gpio_reset(APU_RST);
	delay(2);
	gpio_set(APU_RST);

	if (!IMU_waitForSPI()) return 0;
	// shtpData[0] = 1; //Reset

	// //Attempt to start communication with sensor
	// IMU_sendPacket(CHANNEL_EXECUTABLE, 1); //Transmit packet on channel 1, 1 byte

	// //Read all incoming data and flush it
	// delay(50);
	// while (IMU_receivePacket());
	// delay(50);
	// while (IMU_receivePacket());
	
	IMU_waitForSPI();
	IMU_receivePacket();

	IMU_waitForSPI();
	IMU_receivePacket();

	shtpData[0] = SHTP_REPORT_PRODUCT_ID_REQUEST; //Request the product ID and reset info
	shtpData[1] = 0;							  //Reserved

	IMU_sendPacket(CHANNEL_CONTROL, 2);
	IMU_waitForSPI();
	if (IMU_receivePacket())
	{
		if (shtpData[0] == SHTP_REPORT_PRODUCT_ID_RESPONSE)
			return 1;
	}
	return 0;
}

void IMU_parseCommandReport(void) {
	if (shtpData[0] == SHTP_REPORT_COMMAND_RESPONSE)
	{
		//The BNO080 responds with this report to command requests. It's up to use to remember which command we issued.
		uint8_t command = shtpData[2]; //This is the Command byte of the response

		if (command == COMMAND_ME_CALIBRATE)
		{
			calibrationStatus = shtpData[5 + 0]; //R0 - Status (0 = success, non-zero = fail)
		}
	}
}


void IMU_parseInputReport(void) {
	//Calculate the number of data bytes in this packet
	int16_t dataLength = ((uint16_t)shtpHeader[1] << 8 | shtpHeader[0]);
	dataLength &= ~(1 << 15); //Clear the MSbit. This bit indicates if this package is a continuation of the last.
	//Ignore it for now. TODO catch this as an error and exit

	dataLength -= 4; //Remove the header bytes from the data count

	timeStamp = ((uint32_t)shtpData[4] << (8 * 3)) | (shtpData[3] << (8 * 2)) | (shtpData[2] << (8 * 1)) | (shtpData[1] << (8 * 0));

	uint8_t status = shtpData[5 + 2] & 0x03; //Get status bits
	uint16_t data1 = (uint16_t)shtpData[5 + 5] << 8 | shtpData[5 + 4];
	uint16_t data2 = (uint16_t)shtpData[5 + 7] << 8 | shtpData[5 + 6];
	uint16_t data3 = (uint16_t)shtpData[5 + 9] << 8 | shtpData[5 + 8];
	uint16_t data4 = 0;
	uint16_t data5 = 0;

	if (dataLength - 5 > 9)
	{
		data4 = (uint16_t)shtpData[5 + 11] << 8 | shtpData[5 + 10];
	}
	if (dataLength - 5 > 11)
	{
		data5 = (uint16_t)shtpData[5 + 13] << 8 | shtpData[5 + 12];
	}

	//Store these generic values to their proper global variable
	if (shtpData[5] == SENSOR_REPORTID_ACCELEROMETER)
	{
		accelAccuracy = status;
		rawAccelX = data1;
		rawAccelY = data2;
		rawAccelZ = data3;
	}
	else if (shtpData[5] == SENSOR_REPORTID_LINEAR_ACCELERATION)
	{
		accelLinAccuracy = status;
		rawLinAccelX = data1;
		rawLinAccelY = data2;
		rawLinAccelZ = data3;
	}
	else if (shtpData[5] == SENSOR_REPORTID_GYROSCOPE)
	{
		gyroAccuracy = status;
		rawGyroX = data1;
		rawGyroY = data2;
		rawGyroZ = data3;
	}
	else if (shtpData[5] == SENSOR_REPORTID_MAGNETIC_FIELD)
	{
		magAccuracy = status;
		rawMagX = data1;
		rawMagY = data2;
		rawMagZ = data3;
	}
	else if (shtpData[5] == SENSOR_REPORTID_ROTATION_VECTOR || shtpData[5] == SENSOR_REPORTID_GAME_ROTATION_VECTOR)
	{
		quatAccuracy = status;
		rawQuatI = data1;
		rawQuatJ = data2;
		rawQuatK = data3;
		rawQuatReal = data4;
		rawQuatRadianAccuracy = data5; //Only available on rotation vector, not game rot vector
	}
	else if (shtpData[5] == SENSOR_REPORTID_STEP_COUNTER)
	{
		stepCount = data3; //Bytes 8/9
	}
	else if (shtpData[5] == SENSOR_REPORTID_STABILITY_CLASSIFIER)
	{
		stabilityClassifier = shtpData[5 + 4]; //Byte 4 only
	}
	else if (shtpData[5] == SENSOR_REPORTID_PERSONAL_ACTIVITY_CLASSIFIER)
	{
		activityClassifier = shtpData[5 + 5]; //Most likely state

		//Load activity classification confidences into the array
		for (uint8_t x = 0; x < 9; x++)					   //Hardcoded to max of 9. TODO - bring in array size
			_activityConfidences[x] = shtpData[5 + 6 + x]; //5 bytes of timestamp, byte 6 is first confidence byte
	}
	else if (shtpData[5] == SHTP_REPORT_COMMAND_RESPONSE)
	{
		// Serial.println("!");
		//The BNO080 responds with this report to command requests. It's up to use to remember which command we issued.
		uint8_t command = shtpData[5 + 2]; //This is the Command byte of the response

		if (command == COMMAND_ME_CALIBRATE)
		{
			// Serial.println("ME Cal report found!");
			calibrationStatus = shtpData[5 + 5]; //R0 - Status (0 = success, non-zero = fail)
		}
	}
	else
	{
		//This sensor report ID is unhandled.
		//See reference manual to add additional feature reports as needed
	}
}

float qToFloat(int16_t fixedPointValue, uint8_t qPoint)
{
	float qFloat = fixedPointValue;
	qFloat *= pow(2, qPoint * -1);
	return (qFloat);
}

//Given a sensor's report ID, this tells the BNO080 to begin reporting the values
//Also sets the specific config word. Useful for personal activity classifier
void IMU_setFeatureCommand(uint8_t reportID, uint16_t freq, uint32_t specificConfig)
{
	int64_t microsBetweenReports = (int64_t)freq * 1000L;

	shtpData[0] = SHTP_REPORT_SET_FEATURE_COMMAND;	 //Set feature command. Reference page 55
	shtpData[1] = reportID;							   //Feature Report ID. 0x01 = Accelerometer, 0x05 = Rotation vector
	shtpData[2] = 0;								   //Feature flags
	shtpData[3] = 0;								   //Change sensitivity (LSB)
	shtpData[4] = 0;								   //Change sensitivity (MSB)
	shtpData[5] = (microsBetweenReports >> 0) & 0xFF;  //Report interval (LSB) in microseconds. 0x7A120 = 500ms
	shtpData[6] = (microsBetweenReports >> 8) & 0xFF;  //Report interval
	shtpData[7] = (microsBetweenReports >> 16) & 0xFF; //Report interval
	shtpData[8] = (microsBetweenReports >> 24) & 0xFF; //Report interval (MSB)
	shtpData[9] = 0;								   //Batch Interval (LSB)
	shtpData[10] = 0;								   //Batch Interval
	shtpData[11] = 0;								   //Batch Interval
	shtpData[12] = 0;								   //Batch Interval (MSB)
	shtpData[13] = (specificConfig >> 0) & 0xFF;	   //Sensor-specific config (LSB)
	shtpData[14] = (specificConfig >> 8) & 0xFF;	   //Sensor-specific config
	shtpData[15] = (specificConfig >> 16) & 0xFF;	  //Sensor-specific config
	shtpData[16] = (specificConfig >> 24) & 0xFF;	  //Sensor-specific config (MSB)

	//Transmit packet on channel 2, 17 bytes
	IMU_sendPacket(CHANNEL_CONTROL, 17);
}

// //Tell the sensor to do a command
// //See 6.3.8 page 41, Command request
// //The caller is expected to set P0 through P8 prior to calling
// void IMU_sendCommand(uint8_t command)
// {
// 	shtpData[0] = SHTP_REPORT_COMMAND_REQUEST; //Command Request
// 	shtpData[1] = commandSequenceNumber++;	 //Increments automatically each function call
// 	shtpData[2] = command;					   //Command

// 	//Caller must set these
// 	/*shtpData[3] = 0; //P0
// 	shtpData[4] = 0; //P1
// 	shtpData[5] = 0; //P2
// 	shtpData[6] = 0;
// 	shtpData[7] = 0;
// 	shtpData[8] = 0;
// 	shtpData[9] = 0;
// 	shtpData[10] = 0;
// 	shtpData[11] = 0;*/

// 	//Transmit packet on channel 2, 12 bytes
// 	sendPacket(CHANNEL_CONTROL, 12);
// }

void IMU_enableGyro(uint16_t freq)
{
	IMU_setFeatureCommand(SENSOR_REPORTID_GYROSCOPE, freq,0);
}

void IMU_enableRotation(uint16_t freq)
{
	IMU_setFeatureCommand(SENSOR_REPORTID_ROTATION_VECTOR, freq,0);
}

void IMU_enableAccel(uint16_t freq)
{
	IMU_setFeatureCommand(SENSOR_REPORTID_ACCELEROMETER, freq,0);
}

//Return the gyro component
float getGyroX(void) {return qToFloat(rawGyroX, gyro_Q1); }
float getGyroY(void) {return qToFloat(rawGyroY, gyro_Q1); }
float getGyroZ(void) {return qToFloat(rawGyroZ, gyro_Q1); }
// //Return the gyro component
// uint8_t getGyroAccuracy(void) {return gyroAccuracy; }
float getQuatI(void) {return qToFloat(rawQuatI, rotationVector_Q1); }
float getQuatJ(void) {return  qToFloat(rawQuatJ, rotationVector_Q1); }
float getQuatK(void) {return qToFloat(rawQuatK, rotationVector_Q1); }
float getQuatReal(void) {return qToFloat(rawQuatReal, rotationVector_Q1); }

#define CONVERT_TO_DEGS 180.0f/3.1415926535897932384626f

always_inlined float square(float x) {
	return x*x;
}
float getRoll(void) {
	float q1 = getQuatReal(), q2 = getQuatI(), q3 = getQuatJ(), q4 = getQuatK();
	float q12_q32 = square(q1) - square(q3);
	float q22_q42 = square(q2) - square(q4);
	float sinr_cosp = +2.0f * (q1* q2+ q3* q4);
	float cosr_cosp = q12_q32 - q22_q42;
	return atan2(sinr_cosp, cosr_cosp)*CONVERT_TO_DEGS;

}
float getPitch(void) {
	float q1 = getQuatReal(), q2 = getQuatI(), q3 = getQuatJ(), q4 = getQuatK();
	float sinp = +2.0f * (q1* q3- q4 * q2);
	if (fabs(sinp) >= 1.0f)
		return copysign(3.1415926535897932384626f/2.0f, sinp)*CONVERT_TO_DEGS; // use 90 degrees if out of range
	else
		return asin(sinp)*CONVERT_TO_DEGS;
}
float getYaw(void){
	float q1 = getQuatReal(), q2 = getQuatI(), q3 = getQuatJ(), q4 = getQuatK();
	float q12_q32 = square(q1) - square(q3);
	float q22_q42 = square(q2) - square(q4);
	float siny_cosp = +2.0f * (q1* q4 + q2* q3);
	float cosy_cosp = q12_q32 - q22_q42;  
	return atan2(siny_cosp, cosy_cosp)*CONVERT_TO_DEGS;
}

float getAccelX(void) {return  qToFloat(rawAccelX, accelerometer_Q1); }
float getAccelY(void) {return qToFloat(rawAccelY, accelerometer_Q1); }
float getAccelZ(void) {return qToFloat(rawAccelZ, accelerometer_Q1); }

static uint8_t imu_control = 0;
uint8_t toggleIMUControl(void){
	imu_control = !imu_control;
	return imu_control;
}
uint8_t getIMUControl(void){ 
	return imu_control;
}
uint8_t getAccelLeft(void) {
    // if (imu_control && getAccelX()<-5.0f) return 1;
    if (imu_control && getPitch()>30.0f) return 1;
    return 0;
}
uint8_t getAccelRight(void) {
    // if (imu_control && getAccelX()>5.0f) return 1;
    if (imu_control && getPitch()<-30.0f) return 1;
    return 0;
}

uint8_t getAccelFlick(void) {
	// float x=getAccelX(),y=getAccelY(),z=getAccelZ();
	// float mag = square(x)+square(y)+square(z);
	// static uint32_t last_ticks = 0;
	// if (imu_control && mag > 110 && square(y)>65) {
	// 	last_ticks = get_ticks();
	// 	return 1;
	// }
	// if (get_ticks()-last_ticks<200) return 1;
	// if (getAccelY())
	if (imu_control && getRoll()<-75.0f && getRoll() > -115.0f) return 1;
	return 0;
}
