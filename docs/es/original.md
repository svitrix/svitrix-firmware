Si quieres eliminar SVITRIX de tu Ulanzi TC001, necesitas flashear el firmware original.


## Método 1
1. Descarga el dump del [firmware original](https://raw.githubusercontent.com/svitrix/svitrix-firmware/main/docs/assets/ulanzi_original_firmware.bin)
2. Conecta tu Ulanzi a tu PC y abre https://esp.huhn.me/ en tu navegador.
3. Haz clic en Connect -> Erase
4. Sube el archivo .bin e ingresa 0x00000. Luego haz clic en "program"
![image](https://github.com/svitrix/svitrix-firmware/assets/31169771/b79bdf7e-477e-47f6-a41e-9106519f636b)

## Método 2 (Solo Windows)
1. Descarga la [herramienta de descarga ESP32](https://www.espressif.com/en/support/download/other-tools)
2. Descarga el dump del [firmware original](https://raw.githubusercontent.com/svitrix/svitrix-firmware/main/docs/assets/ulanzi_original_firmware.bin)
3. Inicia la herramienta de descarga ESP32 y selecciona ESP32 como tipo de chip

Configúralo así y haz clic en "Start"

![image](https://github.com/svitrix/svitrix-firmware/assets/31169771/48a29f33-4896-4ee5-a001-17b44710c8ae)
