import type en from "../en/dataFetcher";

const uk: typeof en = {
  deleteTitle: "Видалити джерело даних?",
  err: { url: "URL має починатися з http:// або https://" },
  hint: { jsonPath: "Шлях у JSON через крапку, напр. data.price", displayFormat: "Рядок формату printf, напр. $%.0f" },
  title: "Джерела даних",
  addSource: "+ Додати джерело",
  newSource: "Нове джерело",
  editSource: "Редагувати джерело",
  empty: "Джерела даних не налаштовані.",
  name: "Назва",
  intervalSec: "Інтервал (с)",
  url: "URL",
  jsonPath: "Шлях JSON",
  displayFormat: "Формат показу",
  iconName: "Назва іконки",
  color: "Колір",
  update: "Оновити",
  fetch: "Отримати",
  metaUrl: "URL: {{url}}",
  metaLine: "Шлях: {{path}} | Формат: {{format}} | Кожні {{interval}} с",
  metaIcon: "Іконка: {{icon}}",
  requiredFields: "Потрібні назва, URL і шлях JSON",
  saved: "Збережено!",
  errorSaving: "Помилка збереження",
  confirmDelete: 'Видалити «{{name}}»?',
  deleted: "Видалено",
  fetched: "Отримано!",
};

export default uk;
