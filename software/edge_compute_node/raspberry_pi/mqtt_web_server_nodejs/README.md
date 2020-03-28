# MQTT  + Web Server

## Overview
This script subscribes to the MQTT broker running on the Raspberry Pi and writes the incoming messages to CSV files in a `/data` directory, creating a new CSV file each hour. This script serves a web app at port 4000 of the Raspberry Pi's IP address (e.g., 192.168.1.2:4000). The web app can also be accessed at http://sapflow.local:4000 for devices that support mDNS.

## Setup and running the script
1. `cd` into the directory.
2. Run `$ npm install` to install the dependencies for this MQTT broker + web server.
3. `$ cd frontend`.
4. Run `$ npm install` to install the dependencies for the web app.
5. Run `$ npm run build` to build the web app.
6. `cd ..` to return to the server directory.
7. Run `$ node index.js`.

## Running the script
1. Ensure that you are in the same directory as the script `index.js` to run the script

