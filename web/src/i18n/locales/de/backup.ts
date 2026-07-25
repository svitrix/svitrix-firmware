import type en from "../en/backup";

const de: typeof en = {
  fileLabel: "Backup-Datei (.json)",
  confirmRestoreTitle: "Dateisystem überschreiben und neu starten?",
  confirmRestoreBody: "Beim Wiederherstellen werden die aktuellen Dateien auf dem Gerät durch diese Sicherung überschrieben und das Gerät wird neu gestartet. Dies kann nicht rückgängig gemacht werden.",
  confirmRestore: "Wiederherstellen & neu starten",
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
