import type en from "../en/backup";

const es: typeof en = {
  title: "Copia de seguridad y restauración",
  collecting: "Recopilando archivos...",
  restoring: "Restaurando...",
  restoringProgress: "Restaurando {{done}}/{{total}}...",
  backupDownloaded: "¡Copia de seguridad descargada!",
  backupFailed: "Error en la copia de seguridad",
  restoreComplete: "¡Restauración completa! Reiniciando...",
  restoreFailed: "Error en la restauración",
  backupHeading: "Copia de seguridad",
  backupHint:
    "Descarga todos los archivos del sistema de archivos del dispositivo como copia de seguridad JSON.",
  downloadBackup: "Descargar copia de seguridad",
  restoreHeading: "Restaurar",
  restoreHint:
    "Sube un archivo de copia de seguridad descargado previamente. El dispositivo se reiniciará tras la restauración.",
};

export default es;
