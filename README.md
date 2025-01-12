# ESP-NOW-Data-Visualization-with-LVGL-and-CrowPanel
This repository contains code for displaying real-time data received via ESP-NOW on a CrowPanel ESP32 display. It combines ESP-NOW's wireless communication capabilities with LVGL's graphical interface for a visually appealing and functional data dashboard.

## Features

<ul>
  <li><b>ESP-NOW Communication:</b> Wireless data exchange between ESP32 devices, supporting up to 4 Wemos D1 Mini boards.</li>
  <li><b>Real-Time Visualization:</b> Dynamically updates and displays incoming data on the CrowPanel screen using LVGL.</li>
  <li><b>Custom UI:</b> Designed with manual screen coordinates for precise data presentation.</li>
  <li><b>Touch Support:</b> Integrated touch handling for future interactivity.</li>
  <li><b>Data Freshness:</b> Displays "N/A" for devices that fail to update within 1200 ms.</li>
</ul>

---

## Requirements

<h4>Hardware</h4>
<ul>
  <li>ESP32-S3 with CrowPanel Display (supports 4.3", 5.0", or 7.0")</li>
  <li>Up to 4 Wemos D1 Mini boards (or equivalent ESP8266/ESP32 devices)</li>
</ul>

<h4>Libraries</h4>
<ul>
  <li><a href="https://www.arduino.cc/reference/en/libraries/wifi/">WiFi</a></li>
  <li><a href="https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html">esp_now</a></li>
  <li><a href="https://www.arduino.cc/en/reference/wire">Wire</a></li>
  <li><a href="https://www.arduino.cc/en/reference/SPI">SPI</a></li>
  <li><a href="https://github.com/lvgl/lvgl">LVGL</a></li>
  <li><a href="https://github.com/lovyan03/LovyanGFX">LovyanGFX</a></li>
</ul>

---

## How It Works

<h4>1. ESP-NOW Communication</h4>
<p>The ESP32 CrowPanel listens for data sent by Wemos D1 Mini boards. Each device sends:</p>
<ul>
  <li><code>deviceId</code>: Unique identifier (1–4)</li>
  <li><code>randomValue</code>: Simulated sensor data</li>
</ul>

<h4>2. Data Processing</h4>
<p>Received data is stored along with a timestamp. If no update is received within 1200 ms, the value is marked as "N/A".</p>

<h4>3. Dynamic Display</h4>
<p>The screen displays:</p>
<ul>
  <li>Title bar with a label</li>
  <li>Data from 4 devices, updated every 100 ms</li>
  <li>Color-coded indicators for data availability</li>
</ul>

<h4>4. Touch Input</h4>
<p>Touch functionality is included for future enhancements, enabling user interaction with the display.</p>

---
