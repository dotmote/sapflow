import React, { useState, useEffect } from 'react';
import logo from './logo.svg';
import './App.css';

function App() {
  const [sapflowData, setSapflowData] = useState([]);

  useEffect(() => {
    const ws = new WebSocket('ws://localhost:8080');
    ws.onopen = () => {
      console.log('Websocket connection opened');
    }

    ws.onclose = () => {
      console.log('Websocket connection closed');
    }

    ws.onmessage = (event) => {
      console.log('Websocket message received: ', event);
      console.log('event.data', event.data);
      const mqttData = JSON.parse(event.data);
      const { type } = mqttData;
      if (type === 'mqtt') {
        const { topic } = mqttData;
        if (topic === 'sapflow') {
          const { message } = mqttData;
          const messagePayload = JSON.parse(message);
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
          } = messagePayload;

          setSapflowData((prevState) =>
            [
              ...prevState,
              {
                id,
                temp1,
                temp2,
                millisSinceHeatPulse,
                outsideTemp,
                outsideHumidity,
                soilMoisture,
                rtcUnixTimestamp,
                millisSinceReferenceTemp
              }
            ]
          );
        }
      }
    }
  }, [])

  return (
    <div className="App">
      <header className="App-header">
        <img src={logo} className="App-logo" alt="logo" />
        <p>
          Edit <code>src/App.js</code> and save to reload.
        </p>
        <a
          className="App-link"
          href="https://reactjs.org"
          target="_blank"
          rel="noopener noreferrer"
        >
          Learn React
        </a>
      </header>
    </div>
  );
}

export default App;
