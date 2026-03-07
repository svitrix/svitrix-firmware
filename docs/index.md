---
layout: home

hero:
  name: "SVITRIX"
  text: "Smart Pixel Clock Firmware"
  tagline: "Custom firmware for the Ulanzi Smart Pixel clock and self build matrix clocks."
  image:
    src: /assets/icon.png
    alt: SVITRIX
  actions:
    - theme: brand
      text: Quick Start
      link: /quickstart
    - theme: alt
      text: Flasher
      link: /flasher
    - theme: alt
      text: GitHub
      link: https://github.com/svitrix/svitrix-firmware

features:
  - title: Ready Out of the Box
    details: Pre-installed with time, date, temperature, and humidity apps. Just turn it on.
  - title: Smart Home Companion
    details: Integrates with HomeAssistant, IOBroker, NodeRed and more via MQTT and HTTP APIs.
  - title: Custom Apps
    details: Create dynamic pages that rotate on your display, controlled from your smart home system.
  - title: AI-First Development
    details: Built with Claude (Anthropic) as the core development tool — from architecture to testing and CI.
---

::: info Disclaimer
This open-source software is not affiliated with or endorsed by the company Ulanzi in any way. Use of the software is at your own risk and discretion, and i assume no liability for any potential damages or issues that may arise from using the software. It is important to be aware that using this open-source software comes without direct support or guarantees from the company Ulanzi.
:::

SVITRIX is a custom firmware for the [Ulanzi Smart Pixel clock](https://www.ulanzi.com/products/ulanzi-pixel-smart-clock-2882?ref=28e02dxl) (Ref link).
It meant to be a companion for your smarthome like HomeAssistant, IOBroker, NodeRed and so on.

It is ready to use straight out of the box, with time, date, temperature, and humidity apps pre-installed. You don't need to do anything other than turning it on to start using these features.
However, for those with more advanced skills, the customization options available with custom apps allow you to take SVITRIX to its full potential.

::: warning
In SVITRIX, the term 'Apps' does not refer to traditional smartphone apps that you download and install. Instead, in SVITRIX, CustomApps function more like dynamic pages that rotate within the Apploop rotation of the display. These pages do not store or execute their own logic; instead, they display content that is sent from an external system, such as a smarthome. This content must be transmitted using MQTT or HTTP protocols via the [CustomApp API](./api#custom-apps-and-notifications).
It is important to note that all the logic for managing the content displayed in these CustomApps needs to be handled by your external system. SVITRIX only provides the platform for displaying the information. You have the flexibility to update the content shown on your CustomApps in real-time at any moment, making it a versatile tool for displaying personalized information in your smarthome setup.
:::

SVITRIX is a community-driven fork based on the original [AWTRIX 3](https://github.com/Blueforcer/awtrix3) project. The decision to fork was made to enable active development and accept contributions from the community.

## AI-First Development

SVITRIX is developed using an **AI-first** approach. [Claude](https://claude.ai) by Anthropic is a core part of the development workflow — from architecture decisions and code generation to refactoring, testing, and CI pipeline management. The project uses [Claude Code](https://github.com/anthropics/claude-code) as the primary development tool, with custom hooks and skills tailored to the embedded firmware workflow.

## Support

If you like this project, star it on [GitHub](https://github.com/svitrix/svitrix-firmware) and contribute!
