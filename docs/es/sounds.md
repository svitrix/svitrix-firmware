# Sonidos

SVITRIX usa el buzzer pasivo integrado en el Ulanzi TC001 para reproducir melodías RTTTL monofónicas.

## Reproducir Melodías

Puedes reproducir melodías RTTTL de dos formas:

**Vía API:**
Envía la cadena RTTTL directamente con tu petición API.
Ver documentación para comandos:
- [Sonidos individuales](./api#sound-playback)
- [Con tu notificación](./api#json-properties)

**Vía archivo:**
Este método evita payloads JSON largos ya que el buffer de recepción es limitado.
Ve al administrador de archivos en la [interfaz web](./webinterface) y crea un nuevo archivo de texto en la carpeta `MELODIES`.
Nómbralo como quieras pero usa la extensión `.txt`, ej. `alarma.txt`. Dentro del archivo, coloca una melodía en formato RTTTL.
Cuando hagas referencia al archivo de sonido en llamadas API, omite la extensión del archivo.

Puedes encontrar muchas melodías en internet:
* [Laub-Home Wiki: RTTTL Songs](https://www.laub-home.de/wiki/RTTTL_Songs)
* [Reproductor RTTTL Online](https://adamonsoon.github.io/rtttl-play/)
* [Editor RTTTL](https://corax89.github.io/esp8266Game/soundEditor.html)

## Control de Volumen

El volumen controla el ciclo de trabajo PWM del buzzer. Rango: **0–30** (por defecto: **25**).

**Menú en pantalla:**
Navega al elemento de menú `VOLUME` en el reloj. Usa los botones izquierdo/derecho para ajustar. Mantén presionado seleccionar para guardar.

**Vía API:**
```bash
# Establecer volumen a 15
curl -X POST http://<ip>/api/settings -d '{"VOL": 15}'

# Obtener volumen actual
curl http://<ip>/api/settings
```

La configuración de volumen se guarda en flash y persiste entre reinicios.

## Alternar Sonido

El sonido puede habilitarse o deshabilitarse globalmente:

**Menú en pantalla:**
Navega al elemento de menú `SOUND`. Alterna con los botones izquierdo/derecho. Mantén presionado seleccionar para guardar.

**Vía API:**
```json
{"SOUND": true}
```

Cuando el sonido está deshabilitado, toda la reproducción de melodías (notificaciones, sonido de arranque, llamadas API) está silenciada. El nivel de volumen se preserva y restaura cuando el sonido se vuelve a habilitar.
