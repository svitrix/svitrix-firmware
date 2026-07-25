import type en from "../en/update";

const uk: typeof en = {
  fileLabel: "Файл прошивки (.bin)",
  title: "Оновлення прошивки",
  otaHeading: "Оновлення OTA",
  otaHint:
    "Виберіть файл прошивки .bin для вивантаження. Після успішного оновлення пристрій перезавантажиться автоматично.",
  uploading: "Вивантаження прошивки...",
  uploadComplete: "Вивантаження завершено! Пристрій перезавантажується...",
  uploadFailedStatus: "Помилка вивантаження: {{status}}",
  uploadFailedConnection: "Помилка вивантаження — з'єднання втрачено (можливо, пристрій перезавантажується)",
  toastUploaded: "Прошивку вивантажено!",
  toastFailed: "Помилка вивантаження",
};

export default uk;
