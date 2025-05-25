#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

// Startar HTTP-server med endpoints:
//   GET  /       → Namnge enhet + länk till namnge TAG
//   GET  /set    → Tar emot ?device=…, sparar och registerDevice
//   GET  /tag    → Formulär för att namnge senaste blippade TAG
//   POST /tag    → Skickar nameTag-anrop
void start_http_server(void);

#endif // HTTP_SERVER_H