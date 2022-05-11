# Data formatting

## Introduction

The Connected Islands Zanzibar project aims to collect data with remote sensors using a LoRaWAN network.
To be able to gather this data, send it, receive it and store it, a data model is needed.
First the measurements gathered need to be listed and decided how to represent them.
Second the data needs to be send over LoRaWAN. Therefore the data needs to be formatted into a bit stream to be able to be send out.
Thirdly this bit stream then needs to be reformatted again to a format a database can work with.
Outlined in this document are the ways it is done in this project.

<b><i>
	Note: at time of writing, the end of Zanzibar 2022, our current PoC PCB sensor only measures and send data from the Salinity sensor. The formatter in the TTN application then reformats the data and fills in 0 for all other measurements so no errors occur in the backend. There is also a problem with the location not being included in the payload msg from the PoC. This still needs to be addressed. A possible solution would be to manually add the location in the TTN application and retrieve if into the backend with the use of the TTN API currently used to build the nodes in Node-RED.
</i></b>


---
---


## Measurements

Below are all the measurements that the sensor will perform summed up.

<table style="width: 100%">
	<colgroup>
		<col span="1" style="width: 13%;">
		<col span="2" style="width: 30%;">
		<col span="3" style="width: 47%;">
		<col span="4" style="width: 20%;">
	</colgroup>
	<tr>
		<th>Sensor</th>
		<th>Read method</th>
		<th>Output</th>
		<th>Datatype</th>
	</tr>
	<tr>
		<td>
			Nova SDS011<br>
			Particulate matter
		</td>
		<td>
			<ul>
				<li>UART</li>
				<li>8-bit</li>
				<li>3.3 V</li>
				<li>9600 baudrate</li>
			</ul>
		</td>
		<td>
			<ul>
				<li><b>PM2.5:</b></li>
					<ul>
						<li>0 to 999 μg/m³ PWM Output</li>
						<li>byte 2 = DATA 1 = PM2.5 Low byte</li>
						<li>byte 3 = DATA 2 = PM2.5 High byte</li>
						<li>value = ((PM2.5 High byte *256) + PM2.5 Low byte) / 10</li>
					</ul>
				<li><b>PM10:</b></li>
					<ul>
						<li>0 to 999 μg/m³ PWM Output</li>
						<li>byte 4 = DATA 3 = PM10 Low byte</li>
						<li>byte 5 = DATA 4 = PM10 High byte</li>
						<li>value = ((PM10 High byte *256) + PM10 Low byte) / 10</li>
					</ul>
				<li>byte 8 = Check-sum = DATA1 + ... + DATA6 = byte 2 - 7</li>
			</ul>
		</td>
		<td>
			<ul>
				<li><b>PM2.5:</b></li>
					<ul>
						<li>unsigned int / uint16_t</li>
					</ul>
				<li><b>PM10:</b></li>
					<ul>
						<li>unsigned int / uint16_t</li>
					</ul>
			</ul>
		</td>
	</tr>
	<tr>
		<td>
			Bosch Sensortec BME280<br>
			Temperature, Pressure and Humidity.
		</td>
		<td>
			I²C<br>
			address 0x76 or 0x77<br>
			<a href="https://github.com/BoschSensortec/BME280_driver">API</a><br>
			Burst read all data at once: register 0xF7 to 0xFE = temperature + pressure + humidity
		</td>
		<td>
			<ul>
				<li><b>Temperature:</b></li>
					<ul>
						<li>-40 to 85°C</li>
					</ul>
				<li><b>Pressure:</b></li>
					<ul>
						<li>300 to 1100 hPa</li>
					</ul>
				<li><b>Humidity:</b></li>
					<ul>
						<li>0 to 100% relative humidity</li>
					</ul>
			</ul>
		</td>
		<td>
			<ul>
				<li><b>Temperature:</b></li>
					<ul>
						<li>byte</li>
					</ul>
				<li><b>Pressure:</b></li>
					<ul>
						<li>unsigned int / uint16_t</li>
					</ul>
				<li><b>Humidity:</b></li>
					<ul>
						<li>byte</li>
					</ul>
			</ul>
		</td>
	</tr>
	<tr>
		<td>
			ScioSense CCS811<br>
			eCO2 = equivalent CO2<br>
			and eTVOC = equivelant Total Volatile Organic Compound
		</td>
		<td>
			(Protocol)
		</td>
		<td>
			<ul>
				<li><b>eCO2:</b></li>
					<ul>
						<li>400 to 32768 ppm</li>
					</ul>
				<li><b>eTVOC:</b></li>
					<ul>
						<li>0 tot 29206 ppb</li>
					</ul>
			</ul>
		</td>
		<td>
			<ul>
				<li><b>eCO2:</b></li>
					<ul>
						<li>unsigned int / uint16_t</li>
					</ul>
				<li><b>eTVOC:</b></li>
					<ul>
						<li>unsigned int / uint16_t</li>
					</ul>
			</ul>
		</td>
	</tr>
	<tr>
		<td>
			Salinity sensor
			Own design
		</td>
		<td>
			Analog read value from 0 to 1023
		</td>
		<td>
			0 to 1023<br>
			(salinity is expressed as 0 to 28%)
		</td>
		<td>
			unsigned int / uint16_t
		</td>
	</tr>
	<tr>
		<td>
			Battery charge
		</td>
		<td>
			Analog read value from 0 to 1023
		</td>
		<td>
			High or low
		</td>
		<td>
			boolean
		</td>
	</tr>
</table>


---
---


## Node/Sensor Info

The network node or sensor will also need to be identifiable.
This info will come from The Things Network payload message through MQTT or another TTN API.
The following info is what is important.

<table style="width: 100%">
	<colgroup>
		<col span="1" style="width: 20%;">
		<col span="2" style="width: 40%;">
		<col span="3" style="width: 40%;">
	</colgroup>
	<tr>
		<th>Name</th>
		<th>Datatype</th>
		<th>Source</th>
		<th>Example</th>
	</tr>
	<tr>
		<td>
			id
		</td>
		<td>
			string
		</td>
		<td>
			Payload msg and API
		</td>
		<td>
			eui-0004a30b0020bb1b
		</td>
	<tr>
		<td>
			name
		</td>
		<td>
			string
		</td>
		<td>
			API
		</td>
		<td>
			Node 14
		</td>
	</tr>
	<tr>
		<td>
			date
		</td>
		<td>
			string ISO 8601
		</td>
		<td>
			Payload msg
		</td>
		<td>
			YYYY-MM-DDTHH:mm:ss.sssZ, 1996-10-13T08:35:32.000Z
		</td>
	</tr>
	<tr>
		<td>
			latitude
		</td>
		<td>
			float
		</td>
		<td>
			Payload msg (if triangulated) and API
		</td>
		<td>
			latitude: -6.220360548375914
		</td>
	</tr>
	<tr>
		<td>
			longitude
		</td>
		<td>
			float
		</td>
		<td>
			Payload msg (if triangulated) and API
		</td>
		<td>
			longitude: 39.21113847179748
		</td>
	</tr>
	</tr>
</table>

More info on TTN API can be found <a href="https://www.thethingsindustries.com/docs/reference/api/end_device/">here</a>.
	

---
---


## Data structure

This section shows how the data is structured.
First the formatting to form a bit stream to be send over LoRaWan and the steps involved, and if and how it needs to be reformatted afterwards.
Secondly how the data is then parsed into a JSON object to be stored in a NoSQL database, in this case MongoDb.

### LoRaWAN

The incoming data from a node/sensor.

<table style="width: 100%">
	<colgroup>
		<col span="1" style="width: 15%;">
		<col span="2" style="width: 20%;">
		<col span="3" style="width: 35%;">
		<col span="4" style="width: 15%;">
		<col span="5" style="width: 15%;">
	</colgroup>
	<tr>
		<th>Byte nr</th>
		<th>Name</th>
		<th>Sensor range</th>
		<th>On Node MCU</th>
		<th>Reformat</th>
	</tr>
	<tr>
		<td>
			0-1
		</td>
		<td>
			PM2.5
		</td>
		<td>
			0 to 999 μg/m³
		</td>
		<td>
			n/a
		</td>
		<td>
			n/a
		</td>
	</tr>
	<tr>
		<td>
			2-3
		</td>
		<td>
			PM10
		</td>
		<td>
			0 to 999 μg/m³
		</td>
		<td>
			n/a
		</td>
		<td>
			n/a
		</td>
	</tr>
	<tr>
		<td>
			4
		</td>
		<td>
			Temp
		</td>
		<td>
			-40 to 85°C
		</td>
		<td>
			+40
		</td>
		<td>
			-40
		</td>
	</tr>
	<tr>
		<td>
			5-6
		</td>
		<td>
			Pressure
		</td>
		<td>
			300 to 1100 hPa
		</td>
		<td>
			-300
		</td>
		<td>
			+300
		</td>
	</tr>
	<tr>
		<td>
			7
		</td>
		<td>
			Humidity
		</td>
		<td>
			0 to 100%
		</td>
		<td>
			n/a
		</td>
		<td>
			n/a
		</td>
	</tr>
	<tr>
		<td>
			8-9
		</td>
		<td>
			C02
		</td>
		<td>
			400 to 32768 ppm
		</td>
		<td>
			n/a
		</td>
		<td>
			n/a
		</td>
	</tr>
	<tr>
		<td>
			10-11
		</td>
		<td>
			TVOC
		</td>
		<td>
			0 tot 29206 ppb
		</td>
		<td>
			n/a
		</td>
		<td>
			n/a
		</td>
	</tr>
	<tr>
		<td>
			12
		</td>
		<td>
			Salinity
		</td>
		<td>
			0 to 1023
		</td>
		<td>
			n/a
		</td>
		<td>
			n/a
		</td>
	</tr>
	<tr>
		<td>
			13
		</td>
		<td>
			Battery charge
		</td>
		<td>
			0 to 1023
		</td>
		<td>
			0 or 1
		</td>
		<td>
			n/a
		</td>
	</tr>
</table>


### JSON for NoSQL database		

```javascript
	{
		id: string,
		name: string,
		latitude: float,
		longitude: float,
		datetime: string,
		pm25: integer,
		pm10: integer,
		temp: byte,
		pressure: integer,
		humidity: byte,
		co2: integer,
		tvoc: integer,
		salinity: byte,
		battery: boolean
	}
```


---
---


## Code

Lastly this are code examples used in the project.


### Arduino code

```c++
	// Testdata
	uint16_t pm25 = 999;
	uint16_t pm10 = 0;
	int tempRead = -40;
	uint16_t pressure = 300;
	byte humidity = 100;
	uint16_t co2 = 32768;
	uint16_t tvoc = 29206;
	uint16_t salinity = 1023;
	byte battery = 1; 
	
	// Reformat
	byte temp = tempRead + 40;
	pressure -= 300;
	
	byte payload[15];
	payload[0] = highByte(pm25);
	payload[1] = lowByte(pm25);
	payload[2] = highByte(pm10);
	payload[3] = lowByte(pm10);
	payload[4] = temp;
	payload[5] = highByte(pressure);
	payload[6] = lowByte(pressure);
	payload[7] = humidity;
	payload[8] = highByte(co2);
	payload[9] = lowByte(co2);
	payload[10] = highByte(tvoc);
	payload[11] = lowByte(tvoc);
	payload[12] = highByte(salinity);
	payload[13] = lowByte(salinity);
	payload[14] = battery;
	
	Serial.println("Data formatted. Sending test data ...");
	
	// Sending
	myLora.txBytes(payload, sizeof(payload));
	delay(200);
```

---


### PoC PCB code

The following is the code used in the current PoC prototype, at time of writing.
It is code written for the ATSAMD21G16B microcontroller and the RN2483 LoRa module.
It can be found in <a href="https://github.com/DaanDekoningKrekels/ci-poc-code/blob/main/RN2483.c">file RN2483.c in the ci-poc-code repository</a>.

```c
	void RN2483_send(uint8_t *data) {
		char msgBuffer[4*2 + 1];
		char buffer[3];
		for (unsigned i=0; i<4; i++) {
			sprintf(buffer, "%02X", data[i]);
			memcpy(&msgBuffer[i*2], &buffer, sizeof(buffer));
		}
		UART0_write("mac tx uncnf 1 ", 0);
		delay_ms(200);
		UART0_write(msgBuffer, 0);
		delay_ms(200);
		UART0_write("\r\n", 1);
		delay_ms(200);
	}
```

---

### TTN Formatter

```javascript
	function decodeUplink(input) {
	  return {
  		data: {
  		  pm25: (input.bytes[0] << 8) + input.bytes[1],
  		  pm10: (input.bytes[2] << 8) + input.bytes[3],
  		  temp: input.bytes[4] - 40,
  		  pressure: ((input.bytes[5] << 8) + input.bytes[6]) + 300,
  		  humidity: input.bytes[7],
  		  co2: (input.bytes[8] << 8) + input.bytes[9],
  		  tvoc: (input.bytes[10] << 8) + input.bytes[11],
  		  salinity: (input.bytes[12] << 8) + input.bytes[13],
  		  battery: input.bytes[14]
    		},
		warnings: [],
		errors: []
	  };
	}
```

<img src="./assets/TTN payload formatter.png" alt="TTN payload formatter in the console web interface" width="100%">
	
Version with added data for test purposes:

```javascript
	function decodeUplink(input) {
	  //let output = input;
	  
	  // Test
	  let output[15];
	  for (let i = 0; i < output.length; i++) {
        output.bytes[i] = 0;
      }
      output.bytes[12] = input.bytes[0];
      output.bytes[13] = input.bytes[1];
    
	  return {
  		data: {
  		  pm25: (output.bytes[0] << 8) + output.bytes[1],
  		  pm10: (output.bytes[2] << 8) + output.bytes[3],
  		  temp: output.bytes[4] - 40,
  		  pressure: ((output.bytes[5] << 8) + output.bytes[6]) + 300,
  		  humidity: output.bytes[7],
  		  co2: (output.bytes[8] << 8) + output.bytes[9],
  		  tvoc: (output.bytes[10] << 8) + output.bytes[11],
  		  salinity: (output.bytes[12] << 8) + output.bytes[13],
  		  battery: output.bytes[14]
    		},
		warnings: [],
		errors: []
	  };
	}
```

Now the msg in Node-RED should look like this:

<img src="./assets/Formatted message.png" alt="Formatted message received in Node-RED" width="50%">


---


### Node-RED

In the Node-RED backend the following will happen:
	1. Build the needed nodes by using the TTN API to extract the names of the end devices/nodes/sensors and their id as end point.
	2. Extract measurements formatted by the TTN application's formatter from the payload msg.
	3. Extract id, GPS location and timestamp/datetime from payload and add to the JSON.
		
#### Uplink message example
```javascript
	{
	   topic: "v3/zanzibar@ttn/devices/eui-0004a30b0020db41/up",
	   payload: {
		  end_device_ids: {
			 device_id: "eui-0004a30b0020db41",
			 ...
		  received_at: "2022-04-05T08:34:58.763123609Z"
		  uplink_message: {
			  ...
			  decoded_payload: {
				 pm25: integer,
				 pm10: integer,
				 temp: byte,
				 pressure: integer,
				 humidity: byte,
				 co2: integer,
				 tvoc: integer,
				 salinity: byte,
				 battery: boolean
			  },
			  locations: {
				frm-payload: {
						latitude: -6.220360548375914,
						longitude: 39.21113847179748
					}
				}
			  },
		...
	}
```

The complete uplink message in the Node-RED debug window:

<img src="./assets/Uplink message.png" alt="Uplink message in Node-RED" width="50%">


---

				 
#### Code

```javascript
	var data = 
	{
		id: msg.payload.end_device_ids.device_id,
		name: "Node 1",
		datetime: msg.payload.received_at,
		latitude: msg.payload.uplink_message.locations["frm-payload"].latitude,
		longitude: msg.payload.uplink_message.locations["frm-payload"].longitude
	}
	data = Object.assign(data, msg.payload.uplink_message.decoded_payload);

	msg = data;
	return msg;
```

<img src="./assets/NodeRed function.png" alt="Code example in Node-RED function" width="50%">
<img src="./assets/NodeRed function FULL.png" alt="Code example in Node-RED function FULL" width="100%">


---


#### Result

In the following screenshot you can see the incoming uplink message at the top and the parsed result underneath it.

<img src="./assets/NodeRed incoming and parsed data.png" alt="Result" width="50%">
<img src="./assets/NodeRed incoming and parsed data FULL.png" alt="Result FULL" width="100%">
