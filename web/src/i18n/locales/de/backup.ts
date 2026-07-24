import type en from "../en/backup";

const de: typeof en = {
  title: "Backup & Wiederherstellung",
  collecting: "Dateien werden gesammelt...",
  restoring: "Wird wiederhergestellt...",
  restoringProgress: "Wiederherstellung {{done}}/{{total}}...",
  backupDownloaded: "Backup heruntergeladen!",
  backupFailed: "Backup fehlgeschlagen",
  restoreComplete: "Wiederherstellung abgeschlossen! Neustart...",
  restoreFailed: "Wiederherstellung fehlgeschlagen",
  backupHeading: "Backup",
  backupHint: "Alle Dateien vom Gerätedateisystem als JSON-Backup herunterladen.",
  downloadBackup: "Backup herunterladen",
  restoreHeading: "Wiederherstellen",
  restoreHint:
    "Eine zuvor heruntergeladene Backup-Datei hochladen. Das Gerät startet nach der Wiederherstellung neu.",
};

export default de;
