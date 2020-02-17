const fs = require('fs');
const { normalize } = require('path'); // allows for compatibility between how
// different operating systems treat filepaths
const csv = require('csvtojson');
const { parse } = require('json2csv');

const config = {
  pathToInputDataFiles: normalize('./dataToAggregate'),
  pathToOutputAggregates: normalize('./aggregates'),

  // Manually add header fields here. This script assumes that
  // your csv data files do not have a header row. For example, if your csv 
  // csv files look like:
  // 1581880850,February 16 2020 7:20:50 PM,1,22.1,22.02,10000,20000
  // then your headerFields might be:
  // 'unix_timestamp',
  // 'human_timestamp',
  // 'temp1',
  // 'temp2',
  // 'millisSinceHeatPulse',
  // 'millisSinceReferenceTemp'
  headerFields: [
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
}

const {
  headerFields: fields, // assign `fields` to value of `headerFields`
  pathToInputDataFiles,
  pathToOutputAggregates
} = config;

const aggregateFile = fs.createWriteStream(normalize(
  `${pathToOutputAggregates}/aggregated_${new Date().valueOf()}`
), { flags: 'a' });

aggregateFile.on('close', () => {
  console.log(`Done writing aggregateFile. See ${pathToOutputAggregates} folder to view file.`);
})

fs.readdir(pathToInputDataFiles, async (err, files) => {
  if (err) {
    console.error('Error reading the directory: ', err);
  }

  // filter for only .csv files
  files = files.filter((item) => /.*.csv/.test(item));

  for (let [index, file] of files.entries()) {

    // Only write the header for the first row of the output file
    const header = index === 0 ? true : false;

    const jsonArray = await csv().fromFile(`${pathToInputDataFiles}/${file}`);
    jsonArray.sort((a, b) => +a.unix_timestamp - +b.unix_timestamp);

    try {
      const csvDataToWrite = parse(jsonArray, {
        fields,
        header
      });

      aggregateFile.write(csvDataToWrite);
      aggregateFile.write('\n');
      console.log(`Data written for file: ${file}`);
    } catch (err) {
      console.error('Error writing file: ', err);
    }
  }

  aggregateFile.end();
});