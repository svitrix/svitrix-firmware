import type en from "../en/dataFetcher";

const de: typeof en = {
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
