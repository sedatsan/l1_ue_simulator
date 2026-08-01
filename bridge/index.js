const net = require('net');
const http = require('http');
const { Server } = require('socket.io');

const httpServer = http.createServer();
const io = new Server(httpServer, {
    cors: { origin: "*" }
});

const SOCKET_PATH = '/tmp/mac_sim.sock';
const PORT = 3003;

io.on('connection', (socket) => {
    console.log('Web client connected');
});

const connectToSim = () => {
    const client = net.createConnection(SOCKET_PATH);

    client.on('connect', () => {
        console.log('Connected to MAC Simulator');
    });

    let buffer = '';
    client.on('data', (data) => {
        buffer += data.toString();
        let boundary = buffer.indexOf('\n');
        while (boundary !== -1) {
            const line = buffer.substring(0, boundary);
            buffer = buffer.substring(boundary + 1);
            try {
                const stats = JSON.parse(line);
                io.emit('telemetry', stats);
            } catch (e) {
                console.error('Failed to parse telemetry JSON:', e);
            }
            boundary = buffer.indexOf('\n');
        }
    });

    client.on('error', (err) => {
        console.log('Socket error (Simulator likely not running), retrying in 1s...');
        setTimeout(connectToSim, 1000);
    });

    client.on('end', () => {
        console.log('Disconnected from MAC Simulator, retrying in 1s...');
        setTimeout(connectToSim, 1000);
    });
};

httpServer.listen(PORT, () => {
    console.log(`Bridge listening on port ${PORT}`);
    connectToSim();
});
