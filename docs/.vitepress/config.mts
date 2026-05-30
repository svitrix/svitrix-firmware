import { defineConfig } from 'vitepress'

const sharedSocialLinks = [
  { icon: 'github', link: 'https://github.com/XE1E/svitrix-firmware-XE1E' }
]

function sidebarEs() {
  return [
    {
      text: 'Primeros Pasos',
      collapsed: false,
      items: [
        { text: 'Inicio Rápido', link: '/quickstart' },
        { text: 'Flasher', link: '/flasher' },
        { text: 'Hardware', link: '/hardware' },
        { text: 'FAQ', link: '/faq' },
        { text: 'Eliminar SVITRIX', link: '/original' }
      ]
    },
    {
      text: 'Configuración',
      collapsed: false,
      items: [
        { text: 'Interfaz Web', link: '/webinterface' },
        { text: 'Menú en Pantalla', link: '/onscreen' },
        { text: 'Data Fetcher', link: '/datafetcher' }
      ]
    },
    {
      text: 'Funciones',
      collapsed: false,
      items: [
        { text: 'Apps', link: '/apps' },
        { text: 'Efectos', link: '/effects' },
        { text: 'Iconos', link: '/icons' },
        { text: 'Sonidos', link: '/sounds' },
        { text: 'Funciones Ocultas', link: '/dev' }
      ]
    },
    {
      text: 'API',
      collapsed: false,
      items: [
        { text: 'MQTT / HTTP', link: '/api' },
        { text: 'Home Assistant', link: '/en/home-assistant' }
      ]
    }
  ]
}

function sidebarEn() {
  return [
    {
      text: 'Getting Started',
      collapsed: false,
      items: [
        { text: 'Quick Start', link: '/en/quickstart' },
        { text: 'Flasher', link: '/en/flasher' },
        { text: 'Hardware', link: '/en/hardware' },
        { text: 'FAQ', link: '/en/faq' },
        { text: 'Remove SVITRIX', link: '/en/original' }
      ]
    },
    {
      text: 'Settings',
      collapsed: false,
      items: [
        { text: 'Webinterface', link: '/en/webinterface' },
        { text: 'Onscreen Menu', link: '/en/onscreen' },
        { text: 'Data Fetcher', link: '/en/datafetcher' }
      ]
    },
    {
      text: 'Features',
      collapsed: false,
      items: [
        { text: 'Apps', link: '/en/apps' },
        { text: 'Effects', link: '/en/effects' },
        { text: 'Icons', link: '/en/icons' },
        { text: 'Sounds', link: '/en/sounds' },
        { text: 'Hidden Features', link: '/en/dev' }
      ]
    },
    {
      text: 'API',
      collapsed: false,
      items: [
        { text: 'MQTT / HTTP', link: '/en/api' },
        { text: 'Home Assistant', link: '/en/home-assistant' }
      ]
    }
  ]
}

function sidebarUk() {
  return [
    {
      text: 'Початок роботи',
      collapsed: false,
      items: [
        { text: 'Швидкий старт', link: '/uk/quickstart' },
        { text: 'Прошивка', link: '/uk/flasher' },
        { text: 'Обладнання', link: '/uk/hardware' },
        { text: 'FAQ', link: '/uk/faq' },
        { text: 'Видалити SVITRIX', link: '/uk/original' }
      ]
    },
    {
      text: 'Налаштування',
      collapsed: false,
      items: [
        { text: 'Веб-інтерфейс', link: '/uk/webinterface' },
        { text: 'Екранне меню', link: '/uk/onscreen' },
        { text: 'Збирач даних', link: '/uk/datafetcher' }
      ]
    },
    {
      text: 'Функції',
      collapsed: false,
      items: [
        { text: 'Застосунки', link: '/uk/apps' },
        { text: 'Ефекти', link: '/uk/effects' },
        { text: 'Іконки', link: '/uk/icons' },
        { text: 'Звуки', link: '/uk/sounds' },
        { text: 'Приховані функції', link: '/uk/dev' }
      ]
    },
    {
      text: 'API',
      collapsed: false,
      items: [
        { text: 'MQTT / HTTP', link: '/uk/api' },
        { text: 'Home Assistant', link: '/en/home-assistant' }
      ]
    }
  ]
}

export default defineConfig({
  title: 'Svitrix - XE1E',
  base: '/svitrix-firmware-XE1E/',

  head: [
    ['link', { rel: 'icon', href: '/svitrix-firmware-XE1E/assets/icon.png' }]
  ],

  locales: {
    root: {
      label: 'Español',
      lang: 'es',
      description: 'Firmware personalizado para el reloj Ulanzi Smart Pixel y relojes matriciales caseros.',
      themeConfig: {
        nav: [
          { text: 'Inicio', link: '/' },
          { text: 'Inicio Rápido', link: '/quickstart' },
          { text: 'API', link: '/api' }
        ],
        sidebar: sidebarEs(),
        socialLinks: sharedSocialLinks,
        outline: { level: [2, 3], label: 'En esta página' },
        returnToTopLabel: 'Volver arriba',
        sidebarMenuLabel: 'Menú',
        darkModeSwitchLabel: 'Tema',
        docFooter: {
          prev: 'Página anterior',
          next: 'Página siguiente'
        }
      }
    },
    en: {
      label: 'English',
      lang: 'en',
      description: 'Custom firmware for the Ulanzi Smart Pixel clock and self build matrix clocks.',
      themeConfig: {
        nav: [
          { text: 'Home', link: '/en/' },
          { text: 'Quick Start', link: '/en/quickstart' },
          { text: 'API', link: '/en/api' }
        ],
        sidebar: sidebarEn(),
        socialLinks: sharedSocialLinks,
        outline: { level: [2, 3], label: 'On this page' }
      }
    },
    uk: {
      label: 'Українська',
      lang: 'uk',
      description: 'Кастомна прошивка для Ulanzi Smart Pixel clock та саморобних матричних годинників.',
      themeConfig: {
        nav: [
          { text: 'Головна', link: '/uk/' },
          { text: 'Швидкий старт', link: '/uk/quickstart' },
          { text: 'API', link: '/uk/api' }
        ],
        sidebar: sidebarUk(),
        socialLinks: sharedSocialLinks,
        outline: { level: [2, 3], label: 'На цій сторінці' },
        returnToTopLabel: 'Повернутися на початок',
        sidebarMenuLabel: 'Меню',
        darkModeSwitchLabel: 'Тема',
        docFooter: {
          prev: 'Попередня сторінка',
          next: 'Наступна сторінка'
        }
      }
    }
  },

  themeConfig: {
    logo: '/assets/icon.png',
    search: {
      provider: 'local'
    }
  }
})
