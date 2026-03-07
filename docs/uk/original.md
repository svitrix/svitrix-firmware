Якщо ви хочете видалити SVITRIX з Ulanzi TC001, вам потрібно прошити оригінальну прошивку.


## Спосіб 1
1. Завантажте дамп [оригінальної прошивки](https://raw.githubusercontent.com/svitrix/svitrix-firmware/main/docs/assets/ulanzi_original_firmware.bin)
2. Підключіть Ulanzi до ПК і відкрийте https://esp.huhn.me/ у браузері.
3. Натисніть Connect -> Erase
4. Завантажте .bin файл і введіть 0x00000. Потім натисніть "program"
![image](https://github.com/svitrix/svitrix-firmware/assets/31169771/b79bdf7e-477e-47f6-a41e-9106519f636b)

## Спосіб 2 (лише Windows)
1. Завантажте [ESP32 download tool](https://www.espressif.com/en/support/download/other-tools)
2. Завантажте дамп [оригінальної прошивки](https://raw.githubusercontent.com/svitrix/svitrix-firmware/main/docs/assets/ulanzi_original_firmware.bin)
3. Запустіть ESP32 download tool і виберіть ESP32 як тип чіпа

Налаштуйте як показано нижче і натисніть "Start"

![image](https://github.com/svitrix/svitrix-firmware/assets/31169771/48a29f33-4896-4ee5-a001-17b44710c8ae)
