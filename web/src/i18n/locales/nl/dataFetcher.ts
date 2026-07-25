import type en from "../en/dataFetcher";

const nl: typeof en = {
  deleteTitle: "Gegevensbron verwijderen?",
  err: { url: "URL moet beginnen met http:// of https://" },
  hint: { jsonPath: "Puntpad in de JSON-respons, bijv. data.price", displayFormat: "printf-opmaakreeks, bijv. $%.0f" },
  title: "Databronnen",
  addSource: "+ Bron toevoegen",
  newSource: "Nieuwe bron",
  editSource: "Bron bewerken",
  empty: "Geen databronnen geconfigureerd.",
  name: "Naam",
  intervalSec: "Interval (sec)",
  url: "URL",
  jsonPath: "JSON-pad",
  displayFormat: "Weergaveformaat",
  iconName: "Pictogramnaam",
  color: "Kleur",
  update: "Bijwerken",
  fetch: "Ophalen",
  metaUrl: "URL: {{url}}",
  metaLine: "Pad: {{path}} | Formaat: {{format}} | Elke {{interval}}s",
  metaIcon: "Pictogram: {{icon}}",
  requiredFields: "Naam, URL en JSON-pad zijn verplicht",
  saved: "Opgeslagen!",
  errorSaving: "Fout bij opslaan",
  confirmDelete: '"{{name}}" verwijderen?',
  deleted: "Verwijderd",
  fetched: "Opgehaald!",
};

export default nl;
