const mqtt = require('mqtt');
const fs = require('fs');
const csv = require('fast-csv');

const HOST = 'mqtt://localhost'; // IP address (e.g., 192.168.1.2) of the MQTT broker (in this case, the IP address of the raspberry pi itself).
const TOPIC = 'sapflow';

const client = mqtt.connect(HOST, {
	// Config fields if needed to connect to the MQTT broker
	// username: 'username',
	// password: 'password',
	clientId: 'clientName'
});

client.on('connect', () => {
	client.subscribe(TOPIC);
});

client.on('message', (topic, message) => {
	try {
		const messageJSON = JSON.parse(message.toString());
		console.log(`MQTT message received: ${messageJSON}`);
		const {
			id,
			temp1,
			temp2,
			millisSinceHeatPulse,
			outsideTemp,
			outsideHumidity,
			soilMoisture,
			rtcUnixTimestamp
		} = messageJSON;
		console.log('nodeID: ', id);
		console.log('temp1', temp1);
		console.log('temp2', temp2);
		console.log('millisSinceHeatPulse', millisSinceHeatPulse);

		const currentTime = new Date();

		const currentTimeRoundedToHour =
			Date.UTC(
				currentTime.getUTCFullYear(),
				currentTime.getUTCMonth(),
				currentTime.getUTCDate(),
				currentTime.getUTCHours()
			) / 1000;

		const unixTimestamp = Math.floor(Date.now() / 1000);

		const humanTimestamp = `${currentTime.getUTCMonth() +
			1}/${currentTime.getUTCDate()}/${currentTime.getUTCFullYear()} ${currentTime.getUTCHours()}:${currentTime.getUTCMinutes()}:${currentTime.getUTCSeconds()}`;

		console.log('Writing to csv...');

		const wsHeader = fs.createWriteStream(`./data/${id}_${currentTimeRoundedToHour}.csv`, { flags: 'wx' });
		wsHeader.on('error', (err) => {
			console.log('Header row written. Appending data to existing file.');
		});

		const wsRow = fs.createWriteStream(`./data/${id}_${currentTimeRoundedToHour}.csv`, { flags: 'a' });

		// write header row to top of CSV
		csv.writeToStream(
			wsHeader,
			[
				[
					'unix_timestamp',
					'human_timestamp',
					'nodeID',
					'temp1',
					'temp2',
					'millisSinceHeatPulse',
					'outsideTemp',
					'outsideHumidity',
					'soilMoisture',
					'rtcUnixTimestamp',
					'millisSinceReferenceTemp'
				]
			],
			{ headers: false }
		);

		// append data to existing CSV
		csv.writeToStream(
			wsRow,
			[
				// [ 'unix_timestamp', 'human_timestamp', 'nodeID', 'temp1', 'temp2', 'millisSinceHeatPulse' ],
				[ '' ],
				[
					unixTimestamp,
					humanTimestamp,
					id,
					temp1,
					temp2,
					millisSinceHeatPulse,
					outsideTemp,
					outsideHumidity,
					soilMoisture,
					rtcUnixTimestamp,
					millisSinceReferenceTemp
				]
			],
			{ headers: false }
		);
	} catch (e) {
		if (e instanceof SyntaxError) {
			console.log(`Syntax error in MQTT message: ${e.message}`);
		} else {
			console.log(`Error: ${e}`);
		}
	}
});
