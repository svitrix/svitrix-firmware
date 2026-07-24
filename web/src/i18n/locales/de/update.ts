import type en from "../en/update";

const de: typeof en = {
  title: "Firmware-Update",
  otaHeading: "OTA-Update",
  otaHint:
    "Eine Firmware-.bin-Datei zum Hochladen auswählen. Das Gerät startet nach einem erfolgreichen Update automatisch neu.",
  uploading: "Firmware wird hochgeladen...",
  uploadComplete: "Upload abgeschlossen! Gerät startet neu...",
  uploadFailedStatus: "Upload fehlgeschlagen: {{status}}",
  uploadFailedConnection: "Upload fehlgeschlagen — Verbindung verloren (Gerät startet evtl. neu)",
  toastUploaded: "Firmware hochgeladen!",
  toastFailed: "Upload fehlgeschlagen",
};

export default de;
