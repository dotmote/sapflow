import React from 'react';
import ResponsiveXYFrame from 'semiotic/lib/ResponsiveXYFrame';
import { schemeCategory10 } from 'd3-scale-chromatic';
import { scaleTime } from 'd3-scale';
import { timeFormat } from 'd3-time-format';
import './SapflowScatterplot.css';

const SapflowScatterplot = ({ sapflowData = [], lastHeard = {} }) => {
  const frameData = sapflowData.flatMap(datum => [{ ...datum, pointId: `${datum.id}-temp1`, temp: datum.temp1 }, { ...datum, pointId: `${datum.id}-temp2`, temp: datum.temp2 }]);

  const frameProps = {
    points: frameData,
    responsiveWidth: true,
    hoverAnnotation: true,
    size: [700, 400],
    margin: { left: 60, bottom: 90, right: 10, top: 40 },
    xAccessor: d => new Date(d.rtcUnixTimestamp),
    yAccessor: 'temp',
    xScaleType: scaleTime(),
    pointStyle: d => {
      return {
        r: 5,
        fill: schemeCategory10[Object.keys(lastHeard).indexOf(d.id) % 10]
      };
    },
    title: (
      <text textAnchor="middle">
        Sapflow Raw Datapoints
      </text>
    ),
    axes: [
      { orient: "left", label: "Temperature (C)" },
      { orient: "bottom", label: "Time", tickFormat: timeFormat("%I:%M:%S %p"), ticks: 4 }
    ],
    tooltipContent: d => (
      (
        <div className="tooltip-content">
          <p>Time: {timeFormat("%I:%M:%S %p")(new Date(d.rtcUnixTimestamp))}</p>
          <p>Temperature (C): {d.temp}</p>
          <p>Node: {d.pointId}</p>
        </div>
      )
    )
  }

  return (
    <div style={{ maxWidth: '1000px', margin: 'auto' }}>
      <h2>Dashboard</h2>
      {
        sapflowData.length ?
          <ResponsiveXYFrame {...frameProps} />
          : <p style={{ height: '400px' }}>Waiting for data...</p>
      }
    </div>
  )
}

export default SapflowScatterplot;