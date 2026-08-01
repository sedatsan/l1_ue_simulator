import React, { useRef, useEffect, useState } from 'react';

const MAX_HISTORY = 100;
const TOTAL_RBS = 100;
const COLORS = ['#3b82f6', '#10b981', '#f59e0b', '#ef4444', '#8b5cf6', '#ec4899'];

const SpectrumMap = ({ telemetry }) => {
  const canvasRef = useRef(null);
  const [history, setHistory] = useState([]);

  useEffect(() => {
    if (!telemetry) return;
    setHistory(prev => [telemetry, ...prev].slice(0, MAX_HISTORY));
  }, [telemetry]);

  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;
    const ctx = canvas.getContext('2d');
    const width = canvas.width;
    const height = canvas.height;
    const cellWidth = width / TOTAL_RBS;
    const cellHeight = height / MAX_HISTORY;

    ctx.clearRect(0, 0, width, height);

    history.forEach((slotData, y) => {
      let rbOffset = 0;
      slotData.ues.forEach((ue, index) => {
        ctx.fillStyle = COLORS[index % COLORS.length];
        const rbWidth = ue.rbs * cellWidth;
        ctx.fillRect(rbOffset, y * cellHeight, rbWidth, cellHeight);
        rbOffset += rbWidth;
      });
    });
  }, [history]);

  return (
    <div style={{ padding: '20px', backgroundColor: '#1a1a1a', borderRadius: '8px' }}>
      <h3 style={{ marginTop: 0 }}>RB Allocation Waterfall</h3>
      <canvas ref={canvasRef} width={800} height={400} style={{ border: '1px solid #333', width: '100%' }} />
      <div style={{ display: 'flex', gap: '10px', marginTop: '10px' }}>
        {telemetry?.ues.map((ue, i) => (
          <div key={ue.id} style={{ fontSize: '12px', display: 'flex', alignItems: 'center', gap: '4px' }}>
            <div style={{ width: 12, height: 12, backgroundColor: COLORS[i % COLORS.length] }}></div>
            UE {ue.id}
          </div>
        ))}
      </div>
    </div>
  );
};

export default SpectrumMap;
