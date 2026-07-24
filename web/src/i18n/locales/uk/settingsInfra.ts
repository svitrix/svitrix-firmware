import type en from "../en/settingsInfra";

const uk: typeof en = {
  wifi: {
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
    title: "Мережа",
    staticIp: "Статична IP",
    localIp: "Локальна IP",
    gateway: "Шлюз",
    subnet: "Маска підмережі",
    primaryDns: "Основний DNS",
  },
  mqtt: {
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
