import type en from "../en/update";

const nl: typeof en = {
  title: "Firmware-update",
  otaHeading: "OTA-update",
  otaHint:
    "Selecteer een firmware .bin-bestand om te uploaden. Het apparaat herstart automatisch na een geslaagde update.",
  uploading: "Firmware uploaden...",
  uploadComplete: "Upload voltooid! Apparaat herstart...",
  uploadFailedStatus: "Upload mislukt: {{status}}",
  uploadFailedConnection:
    "Upload mislukt — verbinding verbroken (apparaat herstart mogelijk)",
  toastUploaded: "Firmware geüpload!",
  toastFailed: "Upload mislukt",
};

export default nl;
