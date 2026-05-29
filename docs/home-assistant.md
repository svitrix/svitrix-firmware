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

## Entidades Disponibles (59 total)

### Luces (10)

| Entidad | Descripción | Controles |
|---------|-------------|-----------|
| **Matrix** | Pantalla LED principal | On/Off, Brillo (0-255), Color RGB |
| **Indicator 1** | LED indicador superior derecho | On/Off, Color RGB |
| **Indicator 2** | LED indicador medio derecho | On/Off, Color RGB |
| **Indicator 3** | LED indicador inferior derecho | On/Off, Color RGB |
| **Night color** | Color del texto en modo nocturno | Color RGB |
| **Clock color** | Color de la app de hora | Color RGB |
| **Date color** | Color de la app de fecha | Color RGB |
| **Temperature color** | Color de la app de temperatura | Color RGB |
| **Humidity color** | Color de la app de humedad | Color RGB |
| **Battery color** | Color de la app de batería | Color RGB |

### Switches (14)

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
| **Show outdoor temp** | Muestra/oculta temperatura exterior (weather) |
| **Show outdoor humidity** | Muestra/oculta humedad exterior (weather) |
| **Show pressure** | Muestra/oculta presión atmosférica (weather) |
| **Show air quality** | Muestra/oculta índice de calidad de aire (weather) |
| **Show UV index** | Muestra/oculta índice UV (weather) |

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

## Integración con Google Calendar

Muestra eventos, aniversarios y tareas de Google Calendar en SVITRIX usando Home Assistant como puente.

### Requisitos

- Home Assistant con integración MQTT funcionando
- Cuenta de Google con acceso a Google Calendar

### 1. Configurar Google Calendar en Home Assistant

1. Ve a **Settings > Devices & Services > Add Integration**
2. Busca **Google Calendar**
3. Sigue el flujo de autenticación OAuth con tu cuenta Google
4. Selecciona los calendarios que quieres importar

Una vez configurado, tendrás entidades como:
- `calendar.mi_calendario_personal`
- `calendar.cumpleanos`
- `calendar.trabajo`

### 2. Probar manualmente

Antes de automatizar, prueba enviando un evento manualmente desde **Developer Tools > Services**:

```yaml
service: mqtt.publish
data:
  topic: "svitrix/notify"
  payload: |
    {
      "text": "Prueba de calendario",
      "icon": "6741",
      "duration": 10,
      "color": "#00BFFF"
    }
```

Si aparece en SVITRIX, la conexión MQTT funciona correctamente.

### 3. Automatizaciones

#### Mostrar próximo evento como app permanente

```yaml
automation:
  - alias: "SVITRIX - Próximo evento del calendario"
    trigger:
      - platform: time_pattern
        minutes: "/15"  # actualizar cada 15 min
      - platform: state
        entity_id: calendar.mi_calendario
    condition:
      - condition: state
        entity_id: calendar.mi_calendario
        state: "on"  # hay evento activo o próximo
    action:
      - service: mqtt.publish
        data:
          topic: "svitrix/custom/calendar"
          payload: >
            {
              "text": "{{ state_attr('calendar.mi_calendario', 'message') }}",
              "icon": "6741",
              "duration": 10,
              "color": "#00BFFF"
            }
```

#### Notificación de cumpleaños/aniversario

```yaml
automation:
  - alias: "SVITRIX - Aniversarios del día"
    trigger:
      - platform: time
        at: "08:00:00"
    condition:
      - condition: state
        entity_id: calendar.cumpleanos
        state: "on"
    action:
      - service: mqtt.publish
        data:
          topic: "svitrix/notify"
          payload: >
            {
              "text": "{{ state_attr('calendar.cumpleanos', 'message') }}",
              "icon": "955",
              "duration": 30,
              "color": "#FF69B4",
              "sound": "birthday"
            }
```

#### Recordatorio de reuniones (15 min antes)

```yaml
automation:
  - alias: "SVITRIX - Recordatorio reunión"
    trigger:
      - platform: calendar
        event: start
        entity_id: calendar.trabajo
        offset: "-0:15:00"
    action:
      - service: mqtt.publish
        data:
          topic: "svitrix/notify"
          payload: >
            {
              "text": "En 15 min: {{ trigger.calendar_event.summary }}",
              "icon": "7956",
              "duration": 60,
              "color": "#FFA500",
              "sound": "chime"
            }
```

#### Mostrar tareas pendientes (desde Google Tasks via calendario)

```yaml
automation:
  - alias: "SVITRIX - Tareas del día"
    trigger:
      - platform: time
        at: "09:00:00"
    action:
      - service: mqtt.publish
        data:
          topic: "svitrix/custom/tasks"
          payload: >
            {
              "text": "{{ states('sensor.google_tasks_count') }} tareas pendientes",
              "icon": "51167",
              "color": "#9966FF"
            }
```

### 4. Iconos recomendados

| Icono | ID | Uso |
|-------|-----|-----|
| Calendario | 6741 | Eventos generales |
| Pastel | 955 | Cumpleaños |
| Regalo | 52 | Aniversarios |
| Reunión | 7956 | Trabajo/reuniones |
| Check | 51167 | Tareas |
| Alarma | 5765 | Recordatorios |

Puedes buscar más iconos en [LaMetric Icon Gallery](https://developer.lametric.com/icons).

### 5. Template Sensors (opcional)

Para más control, crea template sensors que formateen los eventos:

```yaml
# configuration.yaml
template:
  - sensor:
      - name: "Próximo Evento Hoy"
        state: >
          {% set cal = state_attr('calendar.mi_calendario', 'message') %}
          {% if cal %}
            {{ cal[:30] }}{% if cal|length > 30 %}...{% endif %}
          {% else %}
            Sin eventos
          {% endif %}
        attributes:
          full_message: "{{ state_attr('calendar.mi_calendario', 'message') }}"
          start_time: "{{ state_attr('calendar.mi_calendario', 'start_time') }}"
          
      - name: "Eventos Hoy Count"
        state: >
          {% if is_state('calendar.mi_calendario', 'on') %}1{% else %}0{% endif %}
```

### 6. Eliminar app de calendario

Para quitar la app de calendario de la rotación:

```yaml
service: mqtt.publish
data:
  topic: "svitrix/custom/calendar"
  payload: "{}"
```

### Solución de problemas

| Problema | Solución |
|----------|----------|
| Calendario no muestra eventos | Verifica que `calendar.X` tenga estado `on` cuando hay evento |
| Texto muy largo | Usa template para truncar: `{{ message[:25] }}...` |
| Caracteres extraños | SVITRIX soporta UTF-8, pero algunos emojis pueden no renderizar |
| Evento no se actualiza | Aumenta frecuencia del trigger `time_pattern` |

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
