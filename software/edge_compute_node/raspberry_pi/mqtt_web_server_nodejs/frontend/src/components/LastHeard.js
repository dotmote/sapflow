import React from 'react';
import { DateTime } from 'luxon';
import { schemeCategory10 } from 'd3-scale-chromatic';

const LastHeard = ({ lastHeard = {} }) => (
  <div style={{ margin: 'auto' }}>
    <h2>Node Health</h2>
    {
      Object.keys(lastHeard).length ?
        <table align='center'>
          <thead>
            <tr>
              <th>Node ID</th>
              <th>Last Heard</th>
            </tr>
          </thead>
          <tbody>
            {
              Object.keys(lastHeard)
                .map((id, index) => {
                  const lastHeardWithin60Min = (new Date().valueOf() - new Date(lastHeard[id]).valueOf()) < (60 * 60 * 1000) ? true : false;
                  return (
                    <tr key={index}>
                      <td style={{ color: schemeCategory10[index % 10] }}>{id}</td>
                      <td
                        title={lastHeardWithin60Min ? 'Last message from this node received within the past hour' : 'Last message > 1 hour ago'}
                        style={{
                          color: lastHeardWithin60Min ? 'green' : 'red'
                        }}
                      >
                        {DateTime.fromISO(lastHeard[id]).toLocaleString(DateTime.DATETIME_MED_WITH_SECONDS)}
                      </td>
                    </tr>
                  )
                })
            }
          </tbody>
        </table>
        : <p>Waiting for data...</p>
    }

  </div>
)

export default LastHeard;