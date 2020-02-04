# MQTT Node.js Script

## Overview
This script subscribes to the MQTT broker running on the Raspberry Pi and writes the incoming messages to CSV files in a `/data` directory, creating a new CSV file each hour.

## Setup
1. `cd` into the directory.
2. Run `$ npm install`.
3. Run `$ mkdir data` to create a directory called `data`.
4. Run `$ node index.js`.