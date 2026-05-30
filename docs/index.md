---
layout: home

hero:
  name: "Svitrix - XE1E"
  text: "Firmware para Reloj Pixel Inteligente"
  tagline: "Firmware personalizado para el reloj Ulanzi Smart Pixel y relojes matriciales caseros."
  image:
    src: /assets/icon.png
    alt: Svitrix
  actions:
    - theme: brand
      text: Inicio Rápido
      link: /quickstart
    - theme: alt
      text: Flasher
      link: /flasher
    - theme: alt
      text: GitHub
      link: https://github.com/XE1E/svitrix-firmware-XE1E

features:
  - title: Listo para Usar
    details: Preinstalado con apps de hora, fecha, temperatura y humedad. Solo enciéndelo.
  - title: Compañero para Smart Home
    details: Se integra con HomeAssistant, IOBroker, NodeRed y más vía MQTT y APIs HTTP.
  - title: Apps Personalizadas
    details: Crea páginas dinámicas que rotan en tu pantalla, controladas desde tu sistema domótico.
  - title: Desarrollo con IA
    details: Desarrollado con Claude (Anthropic) como herramienta principal — desde arquitectura hasta testing y CI.
---

<div style="text-align: center; margin: 2rem 0;">
  <img src="/assets/svitrix-screen.png" alt="Svitrix Display" style="max-width: 100%; border-radius: 8px; box-shadow: 0 4px 12px rgba(0,0,0,0.3);">
</div>

::: info Aviso Legal
Este software de código abierto no está afiliado ni respaldado por la empresa Ulanzi de ninguna manera. El uso del software es bajo tu propio riesgo y discreción, y no asumo responsabilidad por posibles daños o problemas que puedan surgir del uso del software. Es importante saber que usar este software de código abierto no incluye soporte directo ni garantías de la empresa Ulanzi.
:::

SVITRIX es un firmware personalizado para el [reloj Ulanzi Smart Pixel](https://www.ulanzi.com/products/ulanzi-pixel-smart-clock-2882?ref=28e02dxl) (enlace de referencia).
Está diseñado para ser un compañero para tu hogar inteligente como HomeAssistant, IOBroker, NodeRed y más.

Está listo para usar directamente desde la caja, con apps de hora, fecha, temperatura y humedad preinstaladas. No necesitas hacer nada más que encenderlo para comenzar a usar estas funciones.
Sin embargo, para aquellos con habilidades más avanzadas, las opciones de personalización disponibles con apps personalizadas te permiten aprovechar SVITRIX al máximo.

::: warning
En SVITRIX, el término 'Apps' no se refiere a aplicaciones tradicionales de smartphone que descargas e instalas. En cambio, en SVITRIX, las CustomApps funcionan más como páginas dinámicas que rotan dentro del ciclo de apps de la pantalla. Estas páginas no almacenan ni ejecutan su propia lógica; en cambio, muestran contenido que se envía desde un sistema externo, como un hogar inteligente. Este contenido debe transmitirse usando protocolos MQTT o HTTP a través del [API de CustomApp](./api#custom-apps-and-notifications).
Es importante notar que toda la lógica para gestionar el contenido mostrado en estas CustomApps necesita ser manejada por tu sistema externo. SVITRIX solo proporciona la plataforma para mostrar la información. Tienes la flexibilidad de actualizar el contenido mostrado en tus CustomApps en tiempo real en cualquier momento, convirtiéndolo en una herramienta versátil para mostrar información personalizada en tu configuración de hogar inteligente.
:::

SVITRIX es un fork impulsado por la comunidad basado en el proyecto original [AWTRIX 3](https://github.com/Blueforcer/awtrix3). La decisión de hacer un fork se tomó para permitir el desarrollo activo y aceptar contribuciones de la comunidad.

## Desarrollo con IA

SVITRIX se desarrolla usando un enfoque **IA-first**. [Claude](https://claude.ai) de Anthropic es una parte central del flujo de trabajo de desarrollo — desde decisiones de arquitectura y generación de código hasta refactorización, testing y gestión del pipeline de CI. El proyecto usa [Claude Code](https://github.com/anthropics/claude-code) como herramienta principal de desarrollo, con hooks y skills personalizados adaptados al flujo de trabajo de firmware embebido.

## Soporte

Si te gusta este proyecto, dale una estrella en [GitHub](https://github.com/XE1E/svitrix-firmware-XE1E) y contribuye!
