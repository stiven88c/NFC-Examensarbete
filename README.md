NFC-WebServer Example

Det här är ett enkelt exempel på en HTTP-server för ESP32 som låter dig:

Namnge enhet (Device ID) och spara det i NVS.

Visa det aktuella Device ID på en webbsida.

(Kommande) Namnge NFC-taggar via en separat endpoint.

Struktur

├── main
│   └── http_server.c      Implementerar HTTP-servern med endpoints `/` och `/set`
│   └── http_client.c      Hjälper till med POST-anrop mot Google Apps Script
│   └── nfc_communication.c Initierar PN532 och läser NFC-UID
│   └── http_server.h
│   └── http_client.h
│   └── nfc_communication.h
│   └── printer.h          Makron för loggutskrifter (PR_*)
│   └── wifi.c/h           (Valfritt) Wi‑Fi-setup, start av HTTP-server
├── CMakeLists.txt         ESP-IDF projektdefinition
└── README.md              Denna fil

Komma igång

Installera och konfigurera ESP-IDF enligt officiell guide.

Klona detta exempel eller kopiera filerna till din projektmapp.

Konfigurera Wi‑Fi-uppgifter i sdkconfig (SSID, lösenord).

Kör:

idf.py set-target esp32
idf.py menuconfig    # justera eventuellt andra inställningar
idf.py build
idf.py flash monitor

Endpoints

GET /

Visar huvud­sidan där du kan ange Device ID.

Fältet är förifyllt med sparat värde från NVS.

GET /set?device=

Tar emot nytt Device ID (1–999) via URL-param.

Sparar i NVS och anropar Google Apps Script för registrering.

Returnerar omdirigering till / med resultat.

Anpassning för Google Apps Script

Alla anrop mot ditt Apps Script-steg körs via SCRIPT_URL i nfc_communication.h.
Uppdatera den till din egen endpoint.

Payload för registerDevice:

{ "action":"registerDevice", "mac":"AA:BB:...", "device_id":"123", "url":"", "desc":"" }

Nästa steg

Implementera UI och endpoint /tag för att namnge NFC-taggar.

Visa senaste blippade UID och eventuellt namn på huvud­sidan.

Följa upp med logik för att hämta och uppdatera tagg­listan i kalkyl­arket.
