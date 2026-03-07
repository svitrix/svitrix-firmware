# Webinterface

Once SVITRIX is connected to your Wi-Fi network, you can access the web interface via port 80. To do this, simply enter the SVITRIX IP address in your web browser. This IP address is displayed on the matrix at each boot.
The web interface provides essential settings primarily needed for the initial setup:

| Section      | Description                                                                                                                                                                                  |
|--------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| **WiFi Setup** | Configure your Wi-Fi settings and change your access point if necessary.                                                                                                                      |
| **Network**    | Assign a static IP address to SVITRIX.                                                                                                                                                         |
| **MQTT**       | Set up an MQTT broker and enable Home Assistant discovery if you use this smart home system.                                                                                                  |
| **Time**       | Configure your time server and time zone to ensure SVITRIX displays the correct time and date. It also supports automatic daylight saving time adjustments.                                    |
| **Icons**      | Use icons from LaMetric. SVITRIX includes an internal downloader for convenience. Enter the appropriate icon ID, and SVITRIX will download the icon for immediate use.                           |
| **Auth**       | Secure the web interface and the API with basic authentication. Every page, API call, and the SVITRIX app will require these login credentials. Do not lose them; otherwise, you will need to reset your SVITRIX completely. |
| **Files**      | Access the integrated file manager to upload and download data, such as icons or RTTTL melodies.                                                                                              |
| **Update**     | Manually upload an update in the "Firmware" section. The required .bin file can be found in the [SVITRIX release section](https://github.com/svitrix/svitrix-firmware/releases).                     |
| **LiveView**   | View your SVITRIX matrix live, take screenshots, and create animated GIFs.                                                                                                                     |
| **Backup**     | The backup system compiles the entire flash memory into a zip file for download. You can restore this backup on another SVITRIX device.                                                        |
| **Docs**       | A link to the official SVITRIX documentation.                                                                                                                                                |
| **Flows**      | A link to the SVITRIX Flows website.                                                                                                                                                           |

