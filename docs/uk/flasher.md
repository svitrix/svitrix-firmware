
# Онлайн-прошивальник

Доступний у браузерах Google Chrome та Microsoft Edge.
Якщо ви прошиваєте Ulanzi Clock вперше, поставте галочку "erase".

Прошивальник встановлює і прошивку, і веб-інтерфейс (SPA) за один крок.

## Прошивальник для Ulanzi TC001 та кастомних збірок

<iframe src="/svitrix-firmware/ulanzi_flasher/index.html?lang=uk" width="100%" height="400" frameborder="0" style="border: 1px solid var(--vp-c-border); border-radius: 8px;"></iframe>

## Що прошивається

| Розділ | Файл | Офсет | Опис |
|--------|------|-------|------|
| Bootloader | `bootloader.bin` | 0x1000 | Завантажувач ESP32 |
| Partitions | `partitions.bin` | 0x8000 | Таблиця розділів |
| Firmware | `firmware.bin` | 0x10000 | Прошивка SVITRIX |
| LittleFS | `littlefs.bin` | 0x3C0000 | Веб-інтерфейс (SPA) + стандартні файли |

