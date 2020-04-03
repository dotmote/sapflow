# CSV Aggregator Script

## Purpose
This script combines multiple CSV files into a single file.

## Usage

1. Install dependencies by running `$ npm install`
2 Delete the sampleData CSVs in the `dataToAggregate` folder
3. Copy your CSV files to the `dataToAggregate` folder
4. Manually edit the `headerFields` property of the `config` object in `index.js`, if needed, to match the columns of your CSV files
5. Run `$ node index.js`
6. View the aggregated file inside the `aggregates` folder