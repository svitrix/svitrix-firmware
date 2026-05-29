# Home Assistant Integration

SVITRIX soporta auto-discovery de Home Assistant a través de MQTT. Al habilitarlo, el dispositivo se registra automáticamente en HA con 39 entidades controlables.

## Requisitos

- MQTT broker configurado (Mosquitto, EMQX, etc.)
- Home Assistant con integración MQTT habilitada
- SVITRIX conectado al mismo broker MQTT

## Configuración

### 1. En SVITRIX

Ve a **Settings > MQTT** en la interfaz web:

| Campo | Descripción |
|-------|-------------|
| Host | IP o hostname del broker MQTT |
| Port | Puerto (default: 1883) |
| User / Password | Credenciales del broker |
| Prefix | Prefijo para topics (default: `svitrix`) |
| **HA Discovery** | Habilitar para auto-discovery |

### 2. En Home Assistant

La integración MQTT debe estar configurada. Si usas el add-on Mosquitto de HA, ya está listo. Si usas broker externo:

```yaml
# configuration.yaml
mqtt:
  broker: 192.168.1.100
  port: 1883
  username: tu_usuario
  password: tu_password
```

Una vez habilitado HA Discovery en SVITRIX y reiniciado, las entidades aparecerán automáticamente en **Settings > Devices & Services > MQTT**.

---

## Entidades Disponibles (49 total)

### Luces (5)

| Entidad | Descripción | Controles |
|---------|-------------|-----------|
| **Matrix** | Pantalla LED principal | On/Off, Brillo (0-255), Color RGB |
| **Indicator 1** | LED indicador superior derecho | On/Off, Color RGB |
| **Indicator 2** | LED indicador medio derecho | On/Off, Color RGB |
| **Indicator 3** | LED indicador inferior derecho | On/Off, Color RGB |
| **Night color** | Color del texto en modo nocturno | Color RGB |

### Switches (9)

| Entidad | Descripción |
|---------|-------------|
| **Transition** | Habilita/deshabilita transición automática entre apps |
| **Night mode** | Activa/desactiva el modo nocturno |
| **Night block transition** | Bloquea transiciones durante modo nocturno |
| **Sound enabled** | Habilita/deshabilita sonidos del buzzer |
| **Show time app** | Muestra/oculta la app de hora |
| **Show date app** | Muestra/oculta la app de fecha |
| **Show temperature app** | Muestra/oculta la app de temperatura |
| **Show humidity app** | Muestra/oculta la app de humedad |
| **Show battery app** | Muestra/oculta la app de batería |

### Números (6)

| Entidad | Rango | Descripción |
|---------|-------|-------------|
| **Night brightness** | 1-50 | Brillo durante modo nocturno |
| **Sound volume** | 0-30 | Volumen del buzzer |
| **Time per app** | 1-60s | Duración de cada app en rotación |
| **Scroll speed** | 20-200ms | Velocidad de scroll del texto |
| **Clock duration** | 1-300s | Duración de la app de hora |
| **Date duration** | 1-60s | Duración de la app de fecha |

### Selectores (3)

| Entidad | Opciones | Descripción |
|---------|----------|-------------|
| **Brightness mode** | Manual, Auto | Modo de brillo (manual o según sensor de luz) |
| **Transition effect** | 14 efectos | Efecto de transición entre apps |
| **Background effect** | 21 opciones | Efecto de fondo (None, Fade, Matrix, Plasma, Fire, etc.) |

### Botones (6)

| Entidad | Acción |
|---------|--------|
| **Dismiss notification** | Cierra la notificación actual |
| **Start Update** | Inicia actualización de firmware OTA |
| **Next app** | Cambia a la siguiente app |
| **Previous app** | Cambia a la app anterior |
| **Reboot** | Reinicia el dispositivo |
| **Play test sound** | Reproduce un beep de prueba |

### Sensores (16-17)

| Entidad | Unidad | Descripción |
|---------|--------|-------------|
| **Current app** | — | Nombre de la app activa |
| **Device topic** | — | Prefijo MQTT del dispositivo |
| **Temperature** | °C | Temperatura ambiente (sensor interno) |
| **Humidity** | % | Humedad relativa (sensor interno) |
| **Illuminance** | lx | Luz ambiente (sensor LDR) |
| **WiFi signal** | dB | Intensidad de señal WiFi |
| **Firmware version** | — | Versión del firmware |
| **Free RAM** | bytes | Memoria heap disponible |
| **Uptime** | s | Tiempo desde el último reinicio |
| **IP address** | — | Dirección IP del dispositivo |
| **Battery** | % | Nivel de batería (solo Ulanzi TC001) |
| **Outdoor temperature** | °C | Temperatura exterior (WeatherAPI) |
| **Outdoor humidity** | % | Humedad exterior (WeatherAPI) |
| **Pressure** | hPa | Presión atmosférica (WeatherAPI) |
| **Air quality** | AQI | Índice de calidad del aire (WeatherAPI) |
| **Weather condition** | — | Condición climática actual (WeatherAPI) |
| **UV index** | — | Índice UV (WeatherAPI) |

### Sensores Binarios (3)

| Entidad | Descripción |
|---------|-------------|
| **Button left** | Estado del botón izquierdo |
| **Button middle** | Estado del botón central |
| **Button right** | Estado del botón derecho |

---

## Ejemplos de Automatizaciones

### Silenciar cuando TV está encendida

```yaml
alias: "SVITRIX - Silenciar con TV"
trigger:
  - platform: state
    entity_id: media_player.living_room_tv
    to: "on"
action:
  - service: switch.turn_off
    target:
      entity_id: switch.svitrix_sound_enabled
```

### Activar modo nocturno con escena

```yaml
alias: "SVITRIX - Modo cine"
trigger:
  - platform: state
    entity_id: input_boolean.movie_mode
    to: "on"
action:
  - service: switch.turn_on
    target:
      entity_id: switch.svitrix_night_mode
  - service: number.set_value
    target:
      entity_id: number.svitrix_night_brightness
    data:
      value: 5
  - service: light.turn_on
    target:
      entity_id: light.svitrix_night_color
    data:
      rgb_color: [255, 50, 0]
```

### Notificación al presionar botón

```yaml
alias: "SVITRIX - Botón central presionado"
trigger:
  - platform: state
    entity_id: binary_sensor.svitrix_button_middle
    to: "on"
action:
  - service: notify.mobile_app
    data:
      message: "Botón central presionado en SVITRIX"
```

### Brillo automático según hora

```yaml
alias: "SVITRIX - Brillo por horario"
trigger:
  - platform: time
    at: "22:00:00"
action:
  - service: select.select_option
    target:
      entity_id: select.svitrix_brightness_mode
    data:
      option: "Manual"
  - service: light.turn_on
    target:
      entity_id: light.svitrix_matrix
    data:
      brightness: 30
```

### Indicador de estado de alarma

```yaml
alias: "SVITRIX - Indicador alarma"
trigger:
  - platform: state
    entity_id: alarm_control_panel.home
action:
  - choose:
      - conditions:
          - condition: state
            entity_id: alarm_control_panel.home
            state: "armed_away"
        sequence:
          - service: light.turn_on
            target:
              entity_id: light.svitrix_indicator_1
            data:
              rgb_color: [255, 0, 0]
      - conditions:
          - condition: state
            entity_id: alarm_control_panel.home
            state: "disarmed"
        sequence:
          - service: light.turn_on
            target:
              entity_id: light.svitrix_indicator_1
            data:
              rgb_color: [0, 255, 0]
```

### Enviar notificación via MQTT

Para notificaciones más avanzadas, usa el topic MQTT directamente:

```yaml
alias: "SVITRIX - Notificación de timbre"
trigger:
  - platform: state
    entity_id: binary_sensor.doorbell
    to: "on"
action:
  - service: mqtt.publish
    data:
      topic: "svitrix/notify"
      payload: >
        {
          "text": "Alguien en la puerta",
          "icon": "door",
          "color": "#FFAA00",
          "duration": 10,
          "sound": "doorbell"
        }
```

---

## Solución de Problemas

### Las entidades no aparecen en HA

1. Verifica que MQTT esté conectado (icono WiFi+MQTT en SVITRIX)
2. Confirma que "HA Discovery" está habilitado en Settings > MQTT
3. Reinicia SVITRIX después de habilitar discovery
4. Revisa los logs de HA en **Settings > System > Logs**

### Entidades aparecen como "unavailable"

- El dispositivo puede estar desconectado o reiniciándose
- Verifica la conexión MQTT en ambos lados

### Los cambios no se reflejan

- Algunos cambios requieren que SVITRIX publique su estado (cada 5 segundos por defecto)
- Para forzar actualización inmediata, reinicia el dispositivo

---

## Ver también

- [API MQTT/HTTP](api.md) — Comandos avanzados via MQTT
- [Configuración MQTT con Docker](mqtt-docker-setup.md) — Instalación de Mosquitto
