# Data Fetcher

El Data Fetcher permite que SVITRIX automáticamente obtenga datos de APIs HTTP/HTTPS externas y muestre los resultados en la matriz LED — sin necesidad de un sistema de hogar inteligente.

## Cómo Funciona

1. Configuras una o más **fuentes de datos**, cada una apuntando a una API externa
2. SVITRIX obtiene periódicamente cada fuente y extrae un valor de la respuesta JSON
3. El valor extraído se formatea y muestra como una app personalizada en el ciclo de apps

## Interfaz Web

Navega a `http://[IP]/datafetcher` en tu navegador para gestionar fuentes de datos a través de una interfaz visual. Puedes agregar, editar y eliminar fuentes sin escribir ninguna llamada API.

## Configuración

Cada fuente de datos tiene las siguientes configuraciones:

| Configuración | Descripción |
|---------------|-------------|
| **Nombre** | Identificador único — se convierte en el nombre de la app mostrada en el ciclo |
| **URL** | URL completa HTTP o HTTPS del endpoint de la API |
| **Ruta JSON** | Ruta con notación de punto al valor en la respuesta (ej., `bitcoin.usd` o `data.0.price`) |
| **Formato de Visualización** | Formato opcional estilo printf para un solo valor (ej., `$%.0f` para "$67432", `%.1f°C` para "23.5°C"). Especificadores permitidos: `%d %i %u %o %x %X %f %g %e %E %s` con ancho y precisión opcionales (1–2 dígitos cada uno). Usa `%%` para un `%` literal (ej., `%d%%` → "85%"). |
| **Icono** | Nombre de icono opcional de la carpeta ICONS |
| **Color** | Color de texto opcional como hex `#RRGGBB` |
| **Intervalo** | Intervalo de polling en segundos (mínimo 60, por defecto 900) |

## Ejemplo: Precio de Bitcoin

Para mostrar el precio actual de Bitcoin:

- **Nombre**: `btc`
- **URL**: `https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd`
- **Ruta JSON**: `bitcoin.usd`
- **Formato de Visualización**: `$%.0f`
- **Icono**: `btc`
- **Color**: `#F7931A`
- **Intervalo**: `300` (cada 5 minutos)

## Limitaciones

- Máximo **8 fuentes de datos** simultáneamente
- La respuesta de la API debe ser menor a **4 KB**
- Solo se soportan **APIs públicas** (sin encabezados de autenticación personalizados)
- Intervalo mínimo de polling es **60 segundos**
- HTTPS está soportado pero sin validación de certificado

## API

Las fuentes de datos también pueden gestionarse vía el [API HTTP](./api#data-fetcher).
