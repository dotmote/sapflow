import React, { useState, useEffect } from 'react';
import './App.css';
import LastHeard from './components/LastHeard';
import SapflowScatterplot from './components/SapflowScatterplot';
import DataTable from './components/DataTable';
import Logo from './logo.png';

function App() {
  const [lastHeard, setLastHeard] = useState({});
  const [sapflowState, setSapflowState] = useState([]);
  const [websocketOpened, setWebsocketOpened] = useState(false);

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
    fetch('/serverIp')
      .then(res => res.json())
      .then(data => {
        const ws = new WebSocket(`ws://${data.serverIp || 'localhost'}:8080`); // TODO: resolve IP address automatically somehow
        ws.onopen = () => {
          console.log('Websocket connection opened');
          setWebsocketOpened(true);
        };

        ws.onclose = () => {
          console.log('Websocket connection closed');
        };

        ws.onmessage = (event) => {
          const data = JSON.parse(event.data);

          switch (data.type) {
            case 'mqtt': {
              const { topic } = data;
              if (topic === 'sapflow') {
                const { message } = data;
                const messagePayload = JSON.parse(message);
                const { id } = messagePayload;
                const timeReceived = new Date().toISOString();

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
              break;
            }

            case 'wsHeartbeat': {
              break;
            }

            default:
              console.error(`Unhandled data type received from websocket: ${data.type}`);
          }
        };

        ws.onerror = (event) => {
          console.error("WebSocket error observed:", event);
        };
      })
      .catch(e => {
        console.error('Could not fetch lastHeard route from server due to error: ', e)
      });
  }, []);

  return (
    <div className="App">
      <header className="App-header">
        <h1>Sapflow Status</h1>
      </header>
      <div className='dashboard-container'>
        {
          websocketOpened
            ? <p style={{ color: 'green' }}>Websocket connection opened.</p>
            : <p style={{ color: 'red' }}>Websocket failed to connect. Refresh to try again.</p>
        }
        <SapflowScatterplot lastHeard={lastHeard} sapflowData={sapflowState} />
        <LastHeard lastHeard={lastHeard} />
        <DataTable sapflowData={sapflowState} />
        <img style={{ maxWidth: '100px' }} alt='Dotmote Labs logo' src={Logo}></img>
      </div>
    </div >
  );
}

export default App;
