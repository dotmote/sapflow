import React, { useState, useEffect } from 'react';
import './App.css';

function App() {
  const [lastHeard, setLastHeard] = useState({});
  const [sapflowState, setSapflowState] = useState([]);

  useEffect(() => {
    fetch('/lastHeard')
      .then(res => res.json())
      .then(data => {
        setLastHeard(data.lastHeard);
      })
      .catch(e => {
        console.error('Could not fetch lastHeard route from server due to error: ', e)
      });
  }, []);

  useEffect(() => {
    const ws = new WebSocket('ws://localhost:8080');
    ws.onopen = () => {
      console.log('Websocket connection opened');
    }

    ws.onclose = () => {
      console.log('Websocket connection closed');
    }

    ws.onmessage = (event) => {
      const mqttData = JSON.parse(event.data);
      const { type } = mqttData;
      if (type === 'mqtt') {
        const { topic } = mqttData;
        if (topic === 'sapflow') {
          const { message } = mqttData;
          const messagePayload = JSON.parse(message);
          const { id } = messagePayload;
          const timeReceived = new Date();

          setLastHeard((prevState) => {
            return {
              ...prevState,
              [id]: timeReceived
            };
          });

          setSapflowState((prevState) => {
            return [...prevState, { ...messagePayload, timeReceived }];
          });
        }
      }
    }
  }, []);

  return (
    <div className="App">
      <header className="App-header">
        <h1>Dotmote Labs Sapflow Dashboard</h1>
      </header>
    </div>
  );
}

export default App;
