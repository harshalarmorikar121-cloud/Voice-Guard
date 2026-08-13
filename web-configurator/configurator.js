// SafeShield USB Registrar App Logic (Web Serial API)

let currentStep = 1;
let serialPort = null;
let reader = null;
let writer = null;
let isConnected = false;
let generatedOTP = "";

function logTerminal(msg) {
    const logBox = document.getElementById('terminalLog');
    const timeStr = new Date().toLocaleTimeString();
    logBox.innerText += `\n[${timeStr}] ${msg}`;
    logBox.scrollTop = logBox.scrollHeight;
}

// Pair & Connect to ESP32 over USB Serial
async function connectUSBDevice() {
    if (!("serial" in navigator)) {
        alert("Web Serial API is not supported in this browser. Please use Chrome, Edge, or Opera.");
        logTerminal("ERROR: Web Serial API unsupported.");
        return;
    }

    try {
        logTerminal("Requesting USB Serial Port...");
        serialPort = await navigator.serial.requestPort();
        await serialPort.open({ baudRate: 115200 });

        isConnected = true;
        document.getElementById('connStatusText').innerText = "✅ Device Connected (115200 Baud)";
        document.getElementById('connStatusText').style.color = "var(--accent-green)";
        document.getElementById('btnConnect').innerText = "✅ USB Connected";
        document.getElementById('btnNext').disabled = false;

        logTerminal("Connected to ESP32 successfully!");

        // Start listening to response loop
        readSerialLoop();

    } catch (err) {
        logTerminal(`Connection Error: ${err.message}`);
    }
}

async function readSerialLoop() {
    const textDecoder = new TextDecoderStream();
    const readableStreamClosed = serialPort.readable.pipeTo(textDecoder.writable);
    reader = textDecoder.readable.getReader();

    try {
        while (true) {
            const { value, done } = await reader.read();
            if (done) break;
            if (value) {
                logTerminal(`[ESP32] ${value.trim()}`);
                if (value.includes("SUCCESS:CONFIG_SAVED")) {
                    alert("🎉 Success! Emergency numbers successfully saved to ESP32 Flash Memory!");
                    logTerminal("CONFIRMED: Numbers written to NVS Flash Memory!");
                    document.getElementById('btnBurn').innerText = "✅ Saved & Programmed!";
                }
            }
        }
    } catch (e) {
        logTerminal(`Read Loop Ended.`);
    }
}

async function sendSerialCommand(cmdStr) {
    if (!serialPort || !serialPort.writable) return;
    const encoder = new TextEncoder();
    writer = serialPort.writable.getWriter();
    await writer.write(encoder.encode(cmdStr + "\n"));
    writer.releaseLock();
}

function nextStep() {
    if (currentStep === 1) {
        if (!isConnected) {
            alert("Please connect your USB device first!");
            return;
        }
        showStep(2);
    } else if (currentStep === 2) {
        const pNum = document.getElementById('primaryNumber').value.trim();
        if (!pNum) {
            alert("Please enter a Primary Mobile Number!");
            return;
        }

        // Generate 4-digit OTP
        generatedOTP = Math.floor(1000 + Math.random() * 9000).toString();
        logTerminal(`Generated OTP for verification: [${generatedOTP}]. Requesting ESP32 to send SMS...`);
        
        // Command ESP32 to send OTP
        sendSerialCommand(`SEND_OTP_SMS|${pNum}|${generatedOTP}`);

        document.getElementById('summaryNum1').innerText = `Primary Contact: ${pNum}`;
        document.getElementById('summaryNum2').innerText = `Secondary Contact: ${document.getElementById('secondaryNumber').value.trim() || 'None'}`;
        showStep(3);
    }
}

function verifyOTP() {
    const userInput = document.getElementById('otpInput').value.trim();
    if (userInput === generatedOTP) {
        logTerminal("OTP Verified successfully!");
        showStep(4);
    } else {
        alert("Incorrect OTP. Please try again.");
        logTerminal("OTP Verification failed.");
    }
}

function prevStep() {
    if (currentStep > 1) {
        showStep(currentStep - 1);
    }
}

function showStep(stepNum) {
    currentStep = stepNum;

    // Update UI Content visibility
    document.getElementById('stepContent1').style.display = stepNum === 1 ? 'block' : 'none';
    document.getElementById('stepContent2').style.display = stepNum === 2 ? 'block' : 'none';
    document.getElementById('stepContent3').style.display = stepNum === 3 ? 'block' : 'none';
    document.getElementById('stepContent4').style.display = stepNum === 4 ? 'block' : 'none';

    // Update Indicators
    for (let i = 1; i <= 4; i++) {
        const ind = document.getElementById(`stepIndicator${i}`);
        if (ind) {
            if (i === stepNum) {
                ind.className = 'step-item active';
            } else if (i < stepNum) {
                ind.className = 'step-item completed';
            } else {
                ind.className = 'step-item';
            }
        }
    }

    // Button States
    document.getElementById('btnBack').disabled = (stepNum === 1 || stepNum === 3 || stepNum === 4);
    document.getElementById('btnNext').style.display = (stepNum >= 3) ? 'none' : 'inline-flex';
}

async function burnToFlashMemory() {
    const num1 = document.getElementById('primaryNumber').value.trim();
    const num2 = document.getElementById('secondaryNumber').value.trim();

    const payload = JSON.stringify({
        type: "SET_CONFIG",
        num1: num1,
        num2: num2
    });

    logTerminal(`Transmitting payload to Flash Memory...`);
    document.getElementById('btnBurn').innerText = "⏳ Flashing Flash Memory...";
    let checksum = calculateChecksum(payload);
    sendSerialCommand(`SET_CONFIG|${payload}|${checksum}`);
}

// --- MODAL LOGIC ---
function openModal(modalId) {
    document.getElementById(modalId).classList.add('show');
}

function forceClose(modalId) {
    document.getElementById(modalId).classList.remove('show');
}

function closeModal(event, modalId) {
    if (event.target.id === modalId) {
        forceClose(modalId);
    }
}

let bookingOTP = "";

function handleBookingSendOTP(event) {
    event.preventDefault();
    
    const phone = document.getElementById('bookPhone').value;
    if(!phone) return;
    
    // Generate 4 digit OTP
    bookingOTP = Math.floor(1000 + Math.random() * 9000).toString();
    
    // If hardware is connected, use the Voice-Guard hardware to send the real SMS!
    if (port && port.writable) {
        logTerminal(`[BOOKING] Hardware connected! Using SIM to send OTP to ${phone}...`);
        sendSerialCommand(`SEND_OTP_SMS|${phone}|${bookingOTP}`);
    } else {
        // Fallback simulation if no hardware is connected
        alert(`[SIMULATION] Since the Voice-Guard hardware is not plugged in via USB, we cannot send a real SMS.\n\nYour simulated Booking OTP is: ${bookingOTP}`);
    }

    // Switch UI
    document.getElementById('bookingForm').style.display = 'none';
    document.getElementById('bookingOTPForm').style.display = 'block';
}

function handleBookingVerifyOTP(event) {
    event.preventDefault();
    const enteredOTP = document.getElementById('bookOTPInput').value;
    
    if (enteredOTP === bookingOTP) {
        document.getElementById('bookingOTPForm').style.display = 'none';
        document.getElementById('bookingSuccess').style.display = 'block';
    } else {
        alert("Incorrect OTP. Please try again.");
    }
}

function handleSupportSubmit(event) {
    event.preventDefault();
    document.getElementById('supportForm').style.display = 'none';
    document.getElementById('supportSuccess').style.display = 'block';
}
