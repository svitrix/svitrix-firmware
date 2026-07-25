import type en from "../en/settingsInfra";

const uk: typeof en = {
  wifi: {
    confirmConnectTitle: "Підключитися та перезавантажити?",
    confirmConnectBody: "Пристрій збереже дані Wi-Fi для «{{ssid}}» і перезавантажиться для повторного підключення. Ви можете втратити доступ до цієї сторінки, якщо він приєднається до іншої мережі.",
    confirmConnect: "Підключити та перезавантажити",
    title: "WiFi",
    apSubtitle:
      "Підключіться до домашньої мережі WiFi. Після підключення пристрій перезавантажиться з повним набором налаштувань.",
    scan: "Сканувати мережі",
    scanning: "Сканування...",
    connect: "Підключитися",
    ssid: "SSID",
    ssidPlaceholder: "Назва мережі",
    password: "Пароль",
    scanFailed: "Не вдалося просканувати",
    enterSsid: "Введіть SSID",
    connecting: "Підключення до WiFi...",
    rebooting: "Пристрій перезавантажується... перевірте нову IP-адресу на матриці",
    connectionFailed: "Не вдалося підключитися",
  },
  network: {
    invalidIp: "Введіть коректну адресу IPv4, напр. 192.168.1.100",
    title: "Мережа",
    staticIp: "Статична IP",
    localIp: "Локальна IP",
    gateway: "Шлюз",
    subnet: "Маска підмережі",
    primaryDns: "Основний DNS",
  },
  mqtt: {
    invalidPort: "Введіть порт від 1 до 65535",
    title: "MQTT",
    broker: "Брокер",
    port: "Порт",
    username: "Ім'я користувача",
    password: "Пароль",
    prefix: "Префікс",
    haDiscovery: "Виявлення Home Assistant",
  },
  ntp: {
    title: "NTP і часовий пояс",
    server: "Сервер NTP",
    timezone: "Часовий пояс",
    hint: "Знайдіть свій часовий пояс на",
  },
  auth: {
    title: "Автентифікація",
    username: "Ім'я користувача",
    password: "Пароль",
    hint: "Залиште порожнім, щоб вимкнути HTTP-автентифікацію.",
  },
  infra: {
    save: "Зберегти Мережу/MQTT/Час/Автентифікацію",
  },
};

export default uk;
