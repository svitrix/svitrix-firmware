import type en from "../en/backup";

const uk: typeof en = {
  fileLabel: "Файл резервної копії (.json)",
  confirmRestoreTitle: "Перезаписати файлову систему та перезавантажити?",
  confirmRestoreBody: "Відновлення перезапише поточні файли на пристрої цією резервною копією, потім перезавантажить. Це неможливо скасувати.",
  confirmRestore: "Відновити та перезавантажити",
  title: "Резервна копія та відновлення",
  collecting: "Збирання файлів...",
  restoring: "Відновлення...",
  restoringProgress: "Відновлення {{done}}/{{total}}...",
  backupDownloaded: "Резервну копію завантажено!",
  backupFailed: "Не вдалося створити резервну копію",
  restoreComplete: "Відновлення завершено! Перезавантаження...",
  restoreFailed: "Не вдалося відновити",
  backupHeading: "Резервна копія",
  backupHint:
    "Завантажте всі файли з файлової системи пристрою як резервну копію JSON.",
  downloadBackup: "Завантажити резервну копію",
  restoreHeading: "Відновлення",
  restoreHint:
    "Вивантажте раніше завантажений файл резервної копії. Після відновлення пристрій перезавантажиться.",
};

export default uk;
