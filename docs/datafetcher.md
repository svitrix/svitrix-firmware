# Data Fetcher

The Data Fetcher lets SVITRIX automatically pull data from external HTTP/HTTPS APIs and display the results on the LED matrix — no smart home system required.

## How It Works

1. You configure one or more **data sources**, each pointing to an external API
2. SVITRIX periodically fetches each source and extracts a value from the JSON response
3. The extracted value is formatted and displayed as a custom app in the app loop

## Web Interface

Navigate to `http://[IP]/datafetcher` in your browser to manage data sources through a visual interface. You can add, edit, and remove sources without writing any API calls.

## Configuration

Each data source has the following settings:

| Setting | Description |
|---------|-------------|
| **Name** | Unique identifier — becomes the app name shown in the loop |
| **URL** | Full HTTP or HTTPS URL of the API endpoint |
| **JSON Path** | Dot-notation path to the value in the response (e.g., `bitcoin.usd` or `data.0.price`) |
| **Display Format** | Optional printf-style format (e.g., `$%.0f` for "$67432", `%.1f°C` for "23.5°C") |
| **Icon** | Optional icon name from the ICONS folder |
| **Color** | Optional text color as hex `#RRGGBB` |
| **Interval** | Polling interval in seconds (minimum 60, default 900) |

## Example: Bitcoin Price

To display the current Bitcoin price:

- **Name**: `btc`
- **URL**: `https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd`
- **JSON Path**: `bitcoin.usd`
- **Display Format**: `$%.0f`
- **Icon**: `btc`
- **Color**: `#F7931A`
- **Interval**: `300` (every 5 minutes)

## Limitations

- Maximum **8 data sources** simultaneously
- API response must be under **4 KB**
- Only **public APIs** are supported (no custom authentication headers)
- Minimum polling interval is **60 seconds**
- HTTPS is supported but without certificate validation

## API

Data sources can also be managed via the [HTTP API](./api#data-fetcher).
