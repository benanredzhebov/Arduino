#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_NeoPixel.h>

// WiFi Credentials

const char *ssid = "ssi";
const char *password = "Password";

// LED Matrix Configuration
#define LED_PIN D2
#define NUM_LEDS 64
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Web Server on Port 80
AsyncWebServer server(80);

// LED Status Tracking
bool ledStatus[8][8] = {false}; // 8x8 grid

// Define emoticon patterns (8x8)
const uint8_t smilePattern[8] = {
		0b00111100,
		0b01000010,
		0b10100101,
		0b10000001,
		0b10100101,
		0b10011001,
		0b01000010,
		0b00111100};

const uint8_t coolPattern[8] = {
		0b00111100,
		0b01000010,
		0b10100101,
		0b10000001,
		0b10011001,
		0b10100101,
		0b01000010,
		0b00111100};

const uint8_t sadPattern[8] = {
		0b00000000,
		0b01100110,
		0b11111111,
		0b11111111,
		0b01111110,
		0b00111100,
		0b00011000,
		0b00000000};

const uint8_t angryPattern[8] = {
		0b00111100,
		0b01000010,
		0b10100101,
		0b10000001,
		0b10111101,
		0b10000001,
		0b01000010,
		0b00111100};

// Function to map (row, col) to LED index
uint16_t getLEDIndex(uint8_t row, uint8_t col)
{
	return row * 8 + col; // Adjust this if your matrix wiring is different (e.g., serpentine)
}

// Function to display a given pattern on the LED matrix
void displayEmoticon(const uint8_t pattern[8])
{
	strip.clear(); // Clear existing display
	for (uint8_t row = 0; row < 8; row++)
	{
		for (uint8_t col = 0; col < 8; col++)
		{
			bool isOn = bitRead(pattern[row], 7 - col); // MSB is leftmost LED
			ledStatus[row][col] = isOn;
			uint16_t index = getLEDIndex(row, col);
			if (isOn)
			{
				strip.setPixelColor(index, strip.Color(255, 255, 255)); // White color
			}
			else
			{
				strip.setPixelColor(index, strip.Color(0, 0, 0)); // Off
			}
		}
	}
	strip.show(); // Refresh the matrix display
}

// HTML Content
const char webpage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>LED Matrix Control</title>
	<style>
		/* Existing CSS styles... */
		html, body {
			min-height: 100%;
			margin: 0;
			display: flex;
			justify-content: center;
			align-items: center;
			background-image: linear-gradient(to top, #edecec, #cecbc9);
			font-family: Arial, sans-serif;
		}

		.container {
			display: flex;
			flex-direction: column;
			align-items: center;
			gap: 20px;
			background-color: #e0e0e0;
			padding: 20px;
			border-radius: 20px;
			box-shadow: 8px 8px 20px rgba(0, 0, 0, 0.2),
									-8px -8px 20px rgba(255, 255, 255, 0.6);
			width: 100%;
			max-width: 500px;
		}

		.title {
			font-size: 1.5em;
			font-weight: bold;
			margin-bottom: 10px;
			text-align: center; /* Center the text */
		}

		/* Frame for 8x8 grid */
		.grid-frame {
			padding: 20px;
			background-color: #f0f0f0;
			border-radius: 20px;
			box-shadow: inset 8px 8px 20px rgba(0, 0, 0, 0.1),
									inset -8px -8px 20px rgba(255, 255, 255, 0.5);
			width: 100%;
			max-width: 370px; /* Max width for grid to prevent stretching */
			box-sizing: border-box;
		}

		.grid {
			display: grid;
			grid-template-columns: repeat(8, 1fr);
			grid-gap: 5px;
			aspect-ratio: 1 / 1; /* Maintain square aspect ratio */
		}

		.grid button {
			width: 100%;
			height: 0;
			padding-bottom: 100%; /* This ensures the button stays square */
			background-color: #f4f1ee;
			border: none;
			border-radius: 6px;
			box-shadow: 0px 8px 10px rgba(0, 0, 0, 0.3),
									inset 0px 2px 1px 1px white,
									inset 0px -1px 1px 1px rgba(204, 198, 197, 0.5);
			cursor: pointer;
		}

		.grid button.active {
			background-color: red;
		}

		/* Emoticons and Reset/Slider - Two frames in one row */
		.controls-container {
			font-size: 14px;
			font-weight: bold;
			background-color: transparent;
			display: flex;
			flex-direction: column;
			align-items: center;
			width: 100%; /* Full width */
			max-width: 370px; /* Same as grid width */
		}

		.emoticons-frame, .reset-slider-frame {
			display: flex;
			justify-content: space-between; /* Space between elements */
			gap: 10px;
			background-color: #f0f0f0; /* Same as grid frame */
			padding: 10px;
			border-radius: 20px;
			box-shadow: inset 8px 8px 20px rgba(0, 0, 0, 0.1),
									inset -8px -8px 20px rgba(255, 255, 255, 0.5);
			flex: 1;
			box-sizing: border-box;
			margin-top: 10px; /* Add some space between frames */
			width: 100%;
			max-width: 370px; /* Ensure the width matches the grid frame */
		}

		.reset-slider-frame {
			align-items: center;
			position: relative;
		}

		/* Emoticon buttons */
		.emoticon-button {
			width: 40px;
			height: 40px;
			background-color: #ddd;
			border: none;
			border-radius: 5px;
			box-shadow: 3px 3px 8px rgba(0, 0, 0, 0.2),
									-3px -3px 8px rgba(255, 255, 255, 0.7);
			cursor: pointer;
			font-size: 20px; /* Ensure emoticons are visible */
		}

		/* Reset and Toggle */
		.reset-button {
			width: 80px;
			height: 40px;
			background-color: #ddd;
			border: none;
			border-radius: 5px;
			box-shadow: 3px 3px 8px rgba(0, 0, 0, 0.2),
									-3px -3px 8px rgba(255, 255, 255, 0.7);
			cursor: pointer;
			text-align: center;
			line-height: 40px;
			display: flex;
			justify-content: center;
			align-items: center;
		}

		.toggle-container {
			display: flex;
			align-items: center;
		}

		.toggle {
			position: relative;
			width: 60px;
			height: 30px;
			background-color: #ddd;
			border-radius: 15px;
			box-shadow: inset 3px 3px 8px rgba(0, 0, 0, 0.2),
									inset -3px -3px 8px rgba(255, 255, 255, 0.7);
			cursor: pointer;
			margin-left: 10px;
		}

		.toggle::before {
			content: '';
			position: absolute;
			top: 3px;
			left: 3px;
			width: 24px;
			height: 24px;
			background-color: #fff;
			border-radius: 50%;
			box-shadow: 3px 3px 8px rgba(0, 0, 0, 0.2),
									-3px -3px 8px rgba(255, 255, 255, 0.7);
			transition: all 0.3s;
		}

		.toggle.active {
			background-color: #4caf50;
		}

		.toggle.active::before {
			left: 33px;
		}

		/* Text field and Display button frame */
		.text-frame {
			display: flex;
			justify-content: space-between;
			align-items: center;
			width: 100%; /* Full width */
			max-width: 370px; /* Same as grid width */
			padding: 10px;
			background-color: #f0f0f0;
			border-radius: 20px;
			box-shadow: 8px 8px 20px rgba(0, 0, 0, 0.2),
									-8px -8px 20px rgba(255, 255, 255, 0.6);
		}

		.text-field {
			width: 70%; /* To balance with the display button */
			padding: 10px;
			font-size: 14px;
			border-radius: 5px;
			border: 1px solid #ccc;
			box-shadow: inset 1px 1px 4px rgba(0, 0, 0, 0.1);
		}

		.display-button {
			width: 25%;
			padding: 10px;
			font-size: 14px;
			border-radius: 5px;
			background-color: #ddd;
			border: none;
			box-shadow: 3px 3px 8px rgba(0, 0, 0, 0.2),
									-3px -3px 8px rgba(255, 255, 255, 0.7);
			cursor: pointer;
		}

		.controls-label {
			position: absolute;
			top: -20px;
			left: 50%;
			transform: translateX(-50%);
			font-size: 14px;
			font-weight: bold;
			background-color: transparent;
			padding: 0 5px;
		}

		@media (max-width: 600px) {
			.grid button {
				width: 100%;
				padding-bottom: 100%; /* Maintain square buttons on mobile */
			}

			.emoticon-button {
				width: 30px;
				height: 30px;
				font-size: 16px;
			}

			.reset-button {
				width: 60px;
				height: 30px;
			}

			.text-frame{
				max-width: 280px;
			}

			.text-field {
				font-size: 12px;
			}
		}
	</style>
	<script>
		function toggleLED(row, col) {
			var xhttp = new XMLHttpRequest();
			xhttp.onreadystatechange = function() {
				if (this.readyState == 4 && this.status == 200) {
					var btn = document.getElementById("btn_" + row + "_" + col);
					if (this.responseText == '1') {
						btn.classList.add('active');
					} else {
						btn.classList.remove('active');
					}
				}
			};
			xhttp.open("GET", "/press?row=" + row + "&col=" + col, true);
			xhttp.send();
		}

		function resetMatrix() {
			var xhttp = new XMLHttpRequest();
			xhttp.onreadystatechange = function() {
				if (this.readyState == 4 && this.status == 200) {
					// Clear all active buttons
					for (let r = 0; r < 8; r++) {
						for (let c = 0; c < 8; c++) {
							var btn = document.getElementById("btn_" + r + "_" + c);
							btn.classList.remove('active');
						}
					}
				}
			};
			xhttp.open("GET", "/reset", true);
			xhttp.send();
		}

		function displayText() {
			var text = document.getElementById('textInput').value;
			var xhttp = new XMLHttpRequest();
			xhttp.onreadystatechange = function() {
				if (this.readyState == 4 && this.status == 200) {
					// No alert message
				}
			};
			xhttp.open("GET", "/display?text=" + encodeURIComponent(text), true);
			xhttp.send();
		}

		function sendEmoticon(type) {
			var xhttp = new XMLHttpRequest();
			xhttp.onreadystatechange = function() {
				if (this.readyState == 4 && this.status == 200) {
					console.log("Emoticon displayed: " + type);
				}
			};
			xhttp.open("GET", "/emoticon?type=" + type, true);
			xhttp.send();
		}

		function toggleSwitch() {
			var toggle = document.getElementById('toggleSwitch');
			toggle.classList.toggle('active');
			// Add your logic to handle the toggle state change here
		}
	</script>
</head>
<body>
	<div class="container">
		<!-- Frame for 8x8 Grid -->
		<div class="grid-frame">
			<!-- Text Above Grid -->
			<div class="title">LED Matrix Control</div>
			<div class="grid">
				%BUTTON_GRID%
			</div>
		</div>

		<!-- Row for Emoticons -->
		<div class="controls-container">Emoticons
			<div class="emoticons-frame">
				<button class="emoticon-button" onclick="sendEmoticon('smile')">😊</button>
				<button class="emoticon-button" onclick="sendEmoticon('cool')">😎</button>
				<button class="emoticon-button" onclick="sendEmoticon('sad')">😞</button>
				<button class="emoticon-button" onclick="sendEmoticon('angry')">😡</button>
				<button class="emoticon-button" onclick="sendEmoticon('arrow')">◀️</button>
			</div>
		</div>
		
		<!-- Frame for Reset and Toggle -->
		<div class="reset-slider-frame">
			<div class="controls-label">Controls</div>
			<button class="reset-button" onclick="resetMatrix()">Reset</button>
			<div class="toggle-container">
				<span>🔊</span>
				<div id="toggleSwitch" class="toggle" onclick="toggleSwitch()"></div>
			</div>
		</div>
		
		<!-- Text Field and Display Button in One Row -->
		<div class="text-frame">
			<input type="text" id="textInput" class="text-field" placeholder="Enter text">
			<button class="display-button" onclick="displayText()">Display</button>
		</div>
	</div>
</body>
</html>
)rawliteral";

// Function to Generate 8x8 Grid Buttons Dynamically
String generateButtonGrid()
{
	String grid = "";
	for (int row = 0; row < 8; row++)
	{
		for (int col = 0; col < 8; col++)
		{
			grid += "<button id='btn_" + String(row) + "_" + String(col) + "' class='button' onclick='toggleLED(" + String(row) + "," + String(col) + ")'></button>";
		}
	}
	return grid;
}

void setup()
{
	Serial.begin(115200);
	WiFi.begin(ssid, password);

	// Wait for WiFi Connection
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(1000);
		Serial.println("Connecting to WiFi...");
	}
	Serial.println("Connected to WiFi");
	Serial.println(WiFi.localIP());

	strip.begin(); // Initialize the NeoPixel strip
	strip.show();  // Initialize all pixels to 'off'

	// Serve the HTML Page
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
						{
		String htmlContent = String(webpage);
		htmlContent.replace("%BUTTON_GRID%", generateButtonGrid()); // Insert dynamic grid
		request->send(200, "text/html", htmlContent); });

	// Handle Button Press Requests
	server.on("/press", HTTP_GET, [](AsyncWebServerRequest *request)
						{
		if (request->hasParam("row") && request->hasParam("col")) {
			int row = request->getParam("row")->value().toInt();
			int col = request->getParam("col")->value().toInt();

			if (row < 0 || row > 7 || col < 0 || col > 7) {
				request->send(400, "text/plain", "Invalid row or column");
				return;
			}

			// Toggle LED and Update Matrix
			ledStatus[row][col] = !ledStatus[row][col];
			uint16_t index = getLEDIndex(row, col);
			if (ledStatus[row][col]) {
				strip.setPixelColor(index, strip.Color(255, 255, 255)); // White color
			} else {
				strip.setPixelColor(index, strip.Color(0, 0, 0));       // Off
			}
			strip.show();

			// Return Current State
			String response = String(ledStatus[row][col] ? "1" : "0");
			request->send(200, "text/plain", response);
		} else {
			request->send(400, "text/plain", "Missing parameters");
		} });

	// Handle Reset Requests
	server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request)
						{
		for(int r = 0; r < 8; r++) {
			for(int c = 0; c < 8; c++) {
				ledStatus[r][c] = false;
				uint16_t index = getLEDIndex(r, c);
				strip.setPixelColor(index, strip.Color(0, 0, 0)); // Turn off
			}
		}
		strip.show(); // Refresh the matrix
		request->send(200, "text/plain", "Matrix reset"); });

	// Handle Display Text Requests
	server.on("/display", HTTP_GET, [](AsyncWebServerRequest *request)
						{
		if (request->hasParam("text")) {
			String text = request->getParam("text")->value();
			// Implement your text display logic here
			// For demonstration, we'll just clear the matrix
			for(int r = 0; r < 8; r++) {
				for(int c = 0; c < 8; c++) {
					ledStatus[r][c] = false;
					uint16_t index = getLEDIndex(r, c);
					strip.setPixelColor(index, strip.Color(0, 0, 0)); // Turn off
				}
			}
			strip.show(); // Refresh the matrix
			request->send(200, "text/plain", "Text displayed");
		} else {
			request->send(400, "text/plain", "Missing text parameter");
		} });

	// Handle Emoticon Requests
	server.on("/emoticon", HTTP_GET, [](AsyncWebServerRequest *request)
						{
		if (request->hasParam("type")) {
			String type = request->getParam("type")->value();
			if (type == "smile") {
				displayEmoticon(smilePattern);
				request->send(200, "text/plain", "Smile emoticon displayed");
			}
			else if (type == "cool") {
				displayEmoticon(coolPattern);
				request->send(200, "text/plain", "Cool emoticon displayed");
			}
			else if (type == "sad") {
				displayEmoticon(sadPattern);
				request->send(200, "text/plain", "Sad emoticon displayed");
			}
			else if (type == "angry") {
				displayEmoticon(angryPattern);
				request->send(200, "text/plain", "Angry emoticon displayed");
			}
			else if (type == "arrow") { // Added 'arrow' for the fifth emoticon button
				// Define an arrow pattern or any other pattern you'd like
				// For demonstration, we'll clear the matrix
				for(int r = 0; r < 8; r++) {
					for(int c = 0; c < 8; c++) {
						ledStatus[r][c] = false;
						uint16_t index = getLEDIndex(r, c);
						strip.setPixelColor(index, strip.Color(0, 0, 0)); // Turn off
					}
				}
				strip.show();
				request->send(200, "text/plain", "Arrow pattern displayed");
			}
			else {
				request->send(400, "text/plain", "Unknown emoticon type");
			}
		}
		else {
			request->send(400, "text/plain", "Missing type parameter");
		} });

	server.begin();
	Serial.println("HTTP server started");
}

void loop()
{
	// Nothing needed here; everything is handled asynchronously
}