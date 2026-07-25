import type en from "../en/backup";

const nl: typeof en = {
  fileLabel: "Back-upbestand (.json)",
  confirmRestoreTitle: "Bestandssysteem overschrijven en herstarten?",
  confirmRestoreBody: "Herstellen overschrijft de huidige bestanden op het apparaat met deze back-up en start vervolgens opnieuw op. Dit kan niet ongedaan worden gemaakt.",
  confirmRestore: "Herstellen en herstarten",
  title: "Back-up & Herstel",
  collecting: "Bestanden verzamelen...",
  restoring: "Herstellen...",
  restoringProgress: "Herstellen {{done}}/{{total}}...",
  backupDownloaded: "Back-up gedownload!",
  backupFailed: "Back-up mislukt",
  restoreComplete: "Herstel voltooid! Herstarten...",
  restoreFailed: "Herstel mislukt",
  backupHeading: "Back-up",
  backupHint:
    "Download alle bestanden van het apparaatbestandssysteem als JSON-back-up.",
  downloadBackup: "Back-up downloaden",
  restoreHeading: "Herstel",
  restoreHint:
    "Upload een eerder gedownload back-upbestand. Het apparaat herstart na het herstel.",
};

export default nl;
