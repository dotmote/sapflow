import React from 'react';

const DataTable = ({ sapflowData }) => (
  <div>
    <h2>Raw Data</h2>
    {
      sapflowData.length ?
        <div style={{ overflow: 'scroll' }}>
          <table align='center'>
            <thead>
              <tr>
                {
                  Object.keys(sapflowData[0]).map((key, i) =>
                    <th key={`header-${i}`}>{key}</th>
                  )
                }
              </tr>
            </thead>
            <tbody>
              {
                sapflowData.map((datum, i) =>
                  <tr key={`row-${i}`}>
                    {
                      Object.keys(datum).map((key, j) =>
                        <td key={`cell-${i}-${j}`}>{datum[key]}</td>
                      )
                    }
                  </tr>
                )
              }
            </tbody>
          </table>
        </div>
        : <p>Waiting for data...</p>
    }
  </div>
)

export default DataTable;