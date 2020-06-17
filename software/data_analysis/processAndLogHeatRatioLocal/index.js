const fs = require('fs');
const { normalize } = require('path');
const neatCsv = require('neat-csv');
const { getHeatRatios } = require('./heatRatio');

const FILEPATH = './data/aggregated_1586033609056.csv';

const file = fs.readFileSync(normalize(FILEPATH));
console.log(`Processing file: ${FILEPATH}`);

neatCsv(file.toString('utf8'))
  .then(results => {
    const hr = getHeatRatios(results);
    console.log('hrData', Object.entries(hr.data).flatMap(([id, hrDataForNode]) => hrDataForNode.filter(datum => datum.meanHeatRatio)));
  });