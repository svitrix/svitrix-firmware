import type en from "../en/update";

const es: typeof en = {
  title: "Actualización de firmware",
  otaHeading: "Actualización OTA",
  otaHint:
    "Selecciona un archivo de firmware .bin para subir. El dispositivo se reiniciará automáticamente tras una actualización correcta.",
  uploading: "Subiendo firmware...",
  uploadComplete: "¡Subida completa! El dispositivo se está reiniciando...",
  uploadFailedStatus: "Error en la subida: {{status}}",
  uploadFailedConnection:
    "Error en la subida — conexión perdida (el dispositivo puede estar reiniciándose)",
  toastUploaded: "¡Firmware subido!",
  toastFailed: "Error en la subida",
};

export default es;
