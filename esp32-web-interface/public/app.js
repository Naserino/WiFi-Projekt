const ESP32_IP = '192.168.1.100';
const toggleButton = document.getElementById('toggleButton');
const statusDisplay = document.getElementById('status');

async function toggleLED() {
    try {
        const response = await fetch(`http://${ESP32_IP}/toggle`);
        if (response.ok) {
            updateStatus();
        }
    } catch (error) {
        console.error('Error:', error);
        statusDisplay.textContent = 'Status: Error connecting';
    }
}

async function updateStatus() {
    try {
        const response = await fetch(`http://${ESP32_IP}/status`);
        const data = await response.json();
        statusDisplay.textContent = `Status: ${data.ledState ? 'ON' : 'OFF'}`;
        toggleButton.textContent = data.ledState ? 'Turn OFF' : 'Turn ON';
    } catch (error) {
        console.error('Status check failed:', error);
    }
}

toggleButton.addEventListener('click', toggleLED);

// Första statuskollen
updateStatus();
// Checkar efter statusuppdatering varannan sekund
setInterval(updateStatus, 2000);