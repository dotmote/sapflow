// Websocket server
const WebSocket = require('ws');
const wss = new WebSocket.Server({ port: 8080 });

wss.on('connection', function connection(ws) {
	console.log(`New client connected.`);
	console.log('Number of clients: ', wss.clients.size);
});

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

let lastHeard = {};

client.on('message', (topic, message) => {
	wss.clients.forEach(function each(client) {
		if (client.readyState === WebSocket.OPEN) {
			client.send(JSON.stringify({
				type: 'mqtt',
				topic,
				message: message.toString()
			}));
		}
	});

	try {
		const messageJSON = JSON.parse(message.toString());
		console.log(`MQTT message received: ${JSON.stringify(messageJSON)}`);
		const {
			id,
			temp1,
			temp2,
			millisSinceHeatPulse,
			outsideTemp,
			outsideHumidity,
			soilMoisture,
			rtcUnixTimestamp,
			millisSinceReferenceTemp
		} = messageJSON;
		console.log('nodeID: ', id);
		console.log('temp1', temp1);
		console.log('temp2', temp2);
		console.log('millisSinceHeatPulse', millisSinceHeatPulse);

		const currentTime = new Date();
		lastHeard[id] = currentTime;

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

		// human readable string for datafiles. format is:
		// <nodeId>_<localTimeRoundedToHour>_<unixTimestamp>.csv
		// for example:
		// 2_2_2020-02-17T21_00_00.000Z_1581973200.csv
		const localTimeRoundedToHour = new Date(
			currentTime.getFullYear(),
			currentTime.getMonth(),
			currentTime.getDate(),
			currentTime.getHours()
		).toISOString().replace(/:/g, '_');

		console.log('Writing to csv...');

		const wsHeader = fs.createWriteStream(`./data/${id}_${localTimeRoundedToHour}_${currentTimeRoundedToHour}.csv`, { flags: 'wx' });
		wsHeader.on('error', (err) => {
			console.log('Header row written. Appending data to existing file.');
		});

		const wsRow = fs.createWriteStream(`./data/${id}_${localTimeRoundedToHour}_${currentTimeRoundedToHour}.csv`, { flags: 'a' });

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
				[''],
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

// Express web server
const express = require('express');
const app = express();
const PORT = 4000;
const path = require('path');
const LOCAL_NAME = 'sapflow.local';

const ifs = require('os').networkInterfaces();
const serverIp = Object.keys(ifs)
	.map(x => ifs[x].filter(x => x.family === 'IPv4' && !x.internal)[0])
	.filter(x => x)[0].address;

app.use(express.static(path.join(__dirname, 'frontend/build')))

app.get('/', (req, res) => res.sendFile(path.join(__dirname, 'frontend/build', 'index.html')));
app.get('/lastHeard', (req, res) => res.json({ lastHeard }));
app.get('/serverIp', (req, res) => res.json({ serverIp }));
app.listen(PORT, () => {
	console.log(`Web server listening at ${serverIp}:${PORT}`);
	console.log(`Alternatively, go to http://${LOCAL_NAME}:${4000}`);
});

// multi-cast dns. Connect to web app by going to http://sapflow.local:4000.
const mdns = require('multicast-dns')();

mdns.on('query', function (query) {
	// console.log('got a query packet:', query);
	if (query.questions[0] && query.questions[0].name === LOCAL_NAME) {
		console.log('request for sapflow.local received');
		mdns.respond([{ name: LOCAL_NAME, type: 'A', data: `${serverIp}:${PORT}`, ttl: 300, flush: true }])
	}
});