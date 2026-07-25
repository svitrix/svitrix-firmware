import type en from "../en/dataFetcher";

const de: typeof en = {
  deleteTitle: "Datenquelle löschen?",
  err: { url: "Die URL muss mit http:// oder https:// beginnen" },
  hint: { jsonPath: "Punktpfad in der JSON-Antwort, z. B. data.price", displayFormat: "printf-Formatzeichenfolge, z. B. $%.0f" },
  title: "Datenquellen",
  addSource: "+ Quelle hinzufügen",
  newSource: "Neue Quelle",
  editSource: "Quelle bearbeiten",
  empty: "Keine Datenquellen konfiguriert.",
  name: "Name",
  intervalSec: "Intervall (Sek.)",
  url: "URL",
  jsonPath: "JSON-Pfad",
  displayFormat: "Anzeigeformat",
  iconName: "Icon-Name",
  color: "Farbe",
  update: "Aktualisieren",
  fetch: "Abrufen",
  metaUrl: "URL: {{url}}",
  metaLine: "Pfad: {{path}} | Format: {{format}} | Alle {{interval}}s",
  metaIcon: "Icon: {{icon}}",
  requiredFields: "Name, URL und JSON-Pfad sind erforderlich",
  saved: "Gespeichert!",
  errorSaving: "Fehler beim Speichern",
  confirmDelete: '"{{name}}" löschen?',
  deleted: "Gelöscht",
  fetched: "Abgerufen!",
};

export default de;
