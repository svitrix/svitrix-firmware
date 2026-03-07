import { defineConfig } from 'vitepress'

const sharedSocialLinks = [
  { icon: 'github', link: 'https://github.com/svitrix/svitrix-firmware' }
]

function sidebarEn() {
  return [
    {
      text: 'Getting Started',
      collapsed: false,
      items: [
        { text: 'Quick Start', link: '/quickstart' },
        { text: 'Flasher', link: '/flasher' },
        { text: 'Hardware', link: '/hardware' },
        { text: 'FAQ', link: '/faq' },
        { text: 'Remove SVITRIX', link: '/original' }
      ]
    },
    {
      text: 'Settings',
      collapsed: false,
      items: [
        { text: 'Webinterface', link: '/webinterface' },
        { text: 'Onscreen Menu', link: '/onscreen' }
      ]
    },
    {
      text: 'Features',
      collapsed: false,
      items: [
        { text: 'Apps', link: '/apps' },
        { text: 'Effects', link: '/effects' },
        { text: 'Icons', link: '/icons' },
        { text: 'Sounds', link: '/sounds' },
        { text: 'Hidden Features', link: '/dev' }
      ]
    },
    {
      text: 'API',
      collapsed: false,
      items: [
        { text: 'MQTT / HTTP', link: '/api' }
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
        { text: 'Екранне меню', link: '/uk/onscreen' }
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
        { text: 'MQTT / HTTP', link: '/uk/api' }
      ]
    }
  ]
}

export default defineConfig({
  title: 'SVITRIX',
  base: '/svitrix-firmware/',

  head: [
    ['link', { rel: 'icon', href: '/svitrix-firmware/assets/icon.png' }]
  ],

  locales: {
    root: {
      label: 'English',
      lang: 'en',
      description: 'Custom firmware for the Ulanzi Smart Pixel clock and self build matrix clocks.',
      themeConfig: {
        nav: [
          { text: 'Home', link: '/' },
          { text: 'Quick Start', link: '/quickstart' },
          { text: 'API', link: '/api' }
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
