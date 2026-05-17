# 👓 AuraVision – AI Powered Smart Assistive Glasses

AuraVision is an IoT-based assistive smart glasses system designed to help visually impaired individuals navigate their surroundings more safely and independently. The project combines embedded systems, computer vision, sensor fusion, and cloud AI technologies to provide real-time environmental awareness and intelligent assistance.

The system integrates multiple sensors, AI-powered object recognition, GPS tracking, and caregiver monitoring into a wearable smart device capable of detecting obstacles, recognizing objects, and delivering voice-based feedback to users.

---

# 🚀 Features

- 👁️ Real-time object detection and recognition
- 🚶 Obstacle detection and distance measurement
- 📍 GPS location tracking and navigation
- 🧠 AI-powered scene understanding
- 🔊 Voice feedback and alerts
- 📡 IoT-based cloud connectivity
- 📱 Caregiver monitoring dashboard
- ⚡ Low-power embedded system design
- 🛰️ Multi-sensor fusion integration

---

# 🛠️ Technologies Used

## Hardware
- ESP32 Microcontroller
- Camera Module
- ToF Distance Sensor
- IMU Sensor
- GPS Module
- Battery Management System

## Software & AI
- Python
- Embedded C++
- Google Vision API
- OpenCV
- Machine Learning
- IoT Communication Protocols

## Web & Dashboard
- HTML5
- CSS3
- JavaScript
- Cloud Dashboard System

---

# 📂 Project Architecture

```bash
AuraVision/
│
├── firmware/             # ESP32 embedded firmware
├── sensors/              # Sensor integration modules
├── ai-processing/        # AI & computer vision logic
├── dashboard/            # Caregiver monitoring dashboard
├── cloud-services/       # Cloud API integration
├── hardware-design/      # PCB and circuit design files
├── documentation/
└── README.md
```

---

# 📖 System Workflow

## 1️⃣ Environmental Sensing
- Camera captures surroundings
- ToF sensor measures obstacle distance
- IMU tracks movement and orientation
- GPS tracks user location

## 2️⃣ AI Processing
- Images are processed using AI vision models
- Objects and obstacles are identified
- Important environmental data is extracted

## 3️⃣ User Assistance
- Audio feedback is generated
- Alerts are sent for nearby obstacles
- Navigation assistance is provided

## 4️⃣ Cloud Connectivity
- Device data is synced to cloud services
- Caregivers can monitor user location and status
- Emergency alerts can be triggered

---

# 🎯 Core Functionalities

| Feature | Description |
|---|---|
| Object Detection | Identifies surrounding objects |
| Obstacle Avoidance | Detects nearby obstacles |
| GPS Tracking | Provides real-time location |
| Voice Assistance | Delivers audio guidance |
| Caregiver Dashboard | Remote monitoring system |
| Sensor Fusion | Combines multiple sensor inputs |

---

# ▶️ Installation & Setup

## Firmware Setup

```bash
# Clone repository
git clone <repository-url>

# Navigate to firmware folder
cd firmware
```

## Install Dependencies

```bash
pip install opencv-python requests numpy
```

## Upload Firmware to ESP32

```bash
# Using Arduino IDE or PlatformIO
Upload firmware to ESP32 board
```

---

# 📊 Project Goals

- Improve mobility for visually impaired users
- Enhance environmental awareness
- Provide safer navigation assistance
- Enable real-time caregiver monitoring
- Combine AI and IoT into wearable technology

---

# 🎯 Learning Outcomes

Through this project, I gained experience in:

- Embedded systems development
- IoT architecture design
- Sensor fusion techniques
- AI-powered computer vision
- Cloud-connected devices
- Hardware-software integration
- PCB and electronics design
- Real-time system development

---

# 📌 Future Improvements

- Edge AI processing for offline detection
- Voice command support
- Advanced navigation assistance
- LiDAR integration
- Emergency SOS system
- Mobile application integration
- Battery optimization
- Miniaturized hardware design

---

# 🌍 Impact

AuraVision aims to empower visually impaired individuals through accessible and intelligent wearable technology, improving independence, mobility, and overall quality of life using AI and IoT innovation.
