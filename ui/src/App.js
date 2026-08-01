import React, { useEffect, useState } from 'react';
import io from 'socket.io-client';
import SpectrumMap from './SpectrumMap';

const App = () => {
  const [telemetry, setTelemetry] = useState(null);
  const [connected, setConnected] = useState(false);

  useEffect(() => {
    const socket = io('http://localhost:3003');

    socket.on('connect', () => setConnected(true));
    socket.on('disconnect', () => setConnected(false));
    socket.on('telemetry', (data) => {
      setTelemetry(data);
    });

    return () => socket.close();
  }, []);

  return (
    <div style={{ padding: '20px' }}>
      <div style={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', marginBottom: '20px' }}>
        <h1 style={{ margin: 0 }}>6G MAC Simulator Dashboard</h1>
        <div style={{ display: 'flex', alignItems: 'center', gap: '8px', color: connected ? '#10b981' : '#ef4444' }}>
          <div style={{ width: 10, height: 10, borderRadius: '50%', backgroundColor: connected ? '#10b981' : '#ef4444' }}></div>
          {connected ? 'Connected' : 'Disconnected'}
        </div>
      </div>

      <div style={{ display: 'grid', gridTemplateColumns: '300px 1fr', gap: '20px' }}>
        <div style={{ backgroundColor: '#1a1a1a', padding: '20px', borderRadius: '8px' }}>
          <h3 style={{ marginTop: 0 }}>UE Statistics</h3>
          <p>Current Slot: {telemetry?.slot || 'N/A'}</p>
          <hr style={{ borderColor: '#333' }} />
          {telemetry?.ues.map(ue => (
            <div key={ue.id} style={{ marginBottom: '15px', borderBottom: '1px solid #333', paddingBottom: '10px' }}>
              <div style={{ fontWeight: 'bold' }}>UE ID: {ue.id}</div>
              <div style={{ fontSize: '14px', color: '#aaa' }}>Buffer: {ue.buffer} bytes</div>
              <div style={{ fontSize: '14px', color: '#aaa' }}>RBs: {ue.rbs}</div>
            </div>
          ))}
          {(!telemetry || telemetry.ues.length === 0) && <p style={{ color: '#666' }}>Waiting for UE telemetry...</p>}
        </div>

        <SpectrumMap telemetry={telemetry} />
      </div>
    </div>
  );
};

export default App;
