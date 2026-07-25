import type en from "../en/dataFetcher";

const es: typeof en = {
  deleteTitle: "¿Eliminar la fuente de datos?",
  err: { url: "La URL debe empezar por http:// o https://" },
  hint: { jsonPath: "Ruta con puntos en el JSON, p. ej. data.price", displayFormat: "Cadena de formato printf, p. ej. $%.0f" },
  title: "Fuentes de datos",
  addSource: "+ Añadir fuente",
  newSource: "Nueva fuente",
  editSource: "Editar fuente",
  empty: "No hay fuentes de datos configuradas.",
  name: "Nombre",
  intervalSec: "Intervalo (seg)",
  url: "URL",
  jsonPath: "Ruta JSON",
  displayFormat: "Formato de visualización",
  iconName: "Nombre del icono",
  color: "Color",
  update: "Actualizar",
  fetch: "Obtener",
  metaUrl: "URL: {{url}}",
  metaLine: "Ruta: {{path}} | Formato: {{format}} | Cada {{interval}}s",
  metaIcon: "Icono: {{icon}}",
  requiredFields: "El nombre, la URL y la ruta JSON son obligatorios",
  saved: "¡Guardado!",
  errorSaving: "Error al guardar",
  confirmDelete: '¿Eliminar "{{name}}"?',
  deleted: "Eliminado",
  fetched: "¡Obtenido!",
};

export default es;
