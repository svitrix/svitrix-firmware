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
| **Display Format** | Optional printf-style format for a single value (e.g., `$%.0f` for "$67432", `%.1f°C` for "23.5°C"). Allowed specifiers: `%d %i %u %o %x %X %f %g %e %E %s` with optional width and precision (1–2 digits each). Use `%%` for a literal `%` (e.g., `%d%%` → "85%"). |
| **Icon** | Optional icon name from the ICONS folder |
| **Color** | Optional text color as hex `#RRGGBB` |
| **Interval** | Polling interval in seconds (minimum 60, default 900) |

## Examples

### Bitcoin Price

- **Name**: `btc`
- **URL**: `https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd`
- **JSON Path**: `bitcoin.usd`
- **Format**: `$%.0f`
- **Color**: `#F7931A`
- **Interval**: `300`

### USD/MXN Exchange Rate

- **Name**: `USDMXN`
- **URL**: `https://open.er-api.com/v6/latest/USD`
- **JSON Path**: `rates.MXN`
- **Format**: `$%.2f`
- **Color**: `#00AA00`
- **Interval**: `3600`

### Ethereum Price

- **Name**: `eth`
- **URL**: `https://api.coingecko.com/api/v3/simple/price?ids=ethereum&vs_currencies=usd`
- **JSON Path**: `ethereum.usd`
- **Format**: `$%.0f`
- **Color**: `#627EEA`
- **Interval**: `300`

### GitHub Stars for a Repository

- **Name**: `stars`
- **URL**: `https://api.github.com/repos/USER/REPO`
- **JSON Path**: `stargazers_count`
- **Format**: `%d ★`
- **Color**: `#FFD700`
- **Interval**: `3600`

### Your Public IP

- **Name**: `myip`
- **URL**: `https://api.ipify.org?format=json`
- **JSON Path**: `ip`
- **Format**: `%s`
- **Interval**: `86400`

---

## Public API Sources

These collections contain hundreds of free APIs compatible with SVITRIX:

| Resource | URL | Description |
|----------|-----|-------------|
| **Public APIs** | https://github.com/public-apis/public-apis | 1400+ APIs in 50 categories, filterable by auth |
| **API List** | https://apilist.fun | Search by topic and popularity |
| **Free APIs** | https://free-apis.github.io | Curated list of free APIs |
| **No Auth APIs** | https://mixedanalytics.com/blog/list-actually-free-open-no-auth-needed-apis/ | APIs without authentication |
| **RapidAPI Free** | https://rapidapi.com/collection/list-of-free-apis | With examples and documentation |

### Compatibility Requirements

For an API to work with SVITRIX:

- ✅ **No authentication** or API key in the URL (no custom headers)
- ✅ **JSON response** under 4 KB
- ✅ **Extractable value** with dot notation (`data.value`, `items.0.price`)
- ✅ **Public endpoint** accessible from your network

### Ideas by Category

| Category | Suggested APIs |
|----------|----------------|
| **Finance** | CoinGecko, ExchangeRate-API, Alpha Vantage |
| **Weather** | Open-Meteo, WeatherAPI (already integrated) |
| **Monitoring** | UptimeRobot, Pi-hole, Home Assistant |
| **Development** | GitHub API, GitLab API |
| **Entertainment** | Spotify (with token), Twitch |
| **Utilities** | ipify, WorldTimeAPI |

---

## Limitations

- Maximum **8 data sources** simultaneously
- API response must be under **4 KB**
- Only **public APIs** are supported (no custom authentication headers)
- Minimum polling interval is **60 seconds**
- HTTPS is supported but without certificate validation

## API

Data sources can also be managed via the [HTTP API](./api#data-fetcher).
