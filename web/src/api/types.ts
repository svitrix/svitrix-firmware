export interface Settings {
  MATP: boolean;
  ABRI: boolean;
  BRI: number;
  MINBRI: number;
  MAXBRI: number;
  ATRANS: boolean;
  TCOL: number;
  TEFF: number;
  TSPEED: number;
  ATIME: number;
  TMODE: number;
  CHCOL: number;
  CTCOL: number;
  CBCOL: number;
  TFORMAT: string;
  DFORMAT: string;
  SOM: boolean;
  CEL: boolean;
  BLOCKN: boolean;
  MAT: number;
  SOUND: boolean;
  GAMMA: number;
  UPPERCASE: boolean;
  CCORRECTION: string;
  CTEMP: string;
  WD: boolean;
  WDCA: number;
  WDCI: number;
  TIME_COL: number;
  DATE_COL: number;
  HUM_COL: number;
  TEMP_COL: number;
  BAT_COL: number;
  TIMER_COL: number;
  SW_COL: number;
  ALARMS_COL: number;
  SSPEED: number;
  TIM: boolean;
  DAT: boolean;
  HUM: boolean;
  TEMP: boolean;
  BAT: boolean;
  STIMER: boolean;
  SSW: boolean;
  SALARMS: boolean;
  VOL: number;
  NMODE: boolean;
  NSTART: number;
  NEND: number;
  NBRI: number;
  NCOL: number;
  BEFF: number;
  NBTRANS: boolean;
  TIMEDUR: number;
  DATEDUR: number;
  TEMPDUR: number;
  HUMDUR: number;
  BATDUR: number;
  TOFF: number;
}

export interface Stats {
  bat: number;
  bat_raw: number;
  type: number;
  lux: number;
  ldr_raw: number;
  ram: number;
  ram_total: number;
  bri: number;
  temp: number;
  hum: number;
  uptime: string;
  wifi_signal: number;
  messages: number;
  version: string;
  indicator1: boolean;
  indicator2: boolean;
  indicator3: boolean;
  app: string;
  uid: string;
  matrix: boolean;
  ip_address: string;
}

export interface AppInfo {
  name: string;
  icon?: string;
  [key: string]: unknown;
}

export interface DataSource {
  name: string;
  url: string;
  jsonPath: string;
  displayFormat: string;
  icon: string;
  color: string;
  interval: number;
}

export interface FileEntry {
  type: "file" | "dir";
  name: string;
}

export interface Notification {
  text: string;
  icon?: string;
  layout?: "left" | "right" | "none";
  duration?: number;
  hold?: boolean;
  color?: string | [number, number, number];
  rainbow?: boolean;
  rtttl?: string;
  sound?: string;
}

export interface Indicator {
  color?: string | [number, number, number];
  blink?: number;
  fade?: number;
}

export interface EffectInfo {
  name: string;
}

export type TransitionInfo = string;

export interface InfraConfig {
  "Static IP": boolean;
  "Local IP": string;
  "Gateway": string;
  "Subnet": string;
  "Primary DNS": string;
  "Secondary DNS": string;
  "Broker": string;
  "Port": number;
  "Username": string;
  "Password": string;
  "Prefix": string;
  "Homeassistant Discovery": boolean;
  "NTP Server": string;
  "Timezone": string;
  "Auth Username": string;
  "Auth Password": string;
}

export interface WeatherConfig {
  apiKey: string;
  locationType: number;  // 0=city, 1=coords, 2=auto:ip, 3=station
  city: string;
  latitude: number;
  longitude: number;
  stationId: string;
  updateInterval: number;
  showOutdoorTemp: boolean;
  showOutdoorHumidity: boolean;
  showPressure: boolean;
  showAirQuality: boolean;
  showIndoorTemp: boolean;
  showIndoorHumidity: boolean;
  showUV: boolean;
  // Display settings
  outdoorTempColor: number;
  outdoorHumColor: number;
  pressureColor: number;
  aqiColor: number;
  uvColor: number;
  outdoorTempDuration: number;
  outdoorHumDuration: number;
  pressureDuration: number;
  aqiDuration: number;
  uvDuration: number;
}

export interface WeatherData {
  valid: boolean;
  outdoorTemp: number;
  outdoorHumidity: number;
  pressure: number;
  aqi: number;
  uv: number;
  condition: string;
  conditionCode: number;
  lastUpdate: number;
}

// Autonomous Mode Types
export interface TimerState {
  remaining: number;
  running: boolean;
  finished: boolean;
}

export interface StopwatchState {
  elapsed: number;
  running: boolean;
}

export interface Alarm {
  id: number;
  hour: number;
  minute: number;
  days: number;  // Bitmask: Sun=0x01, Mon=0x02, Tue=0x04, Wed=0x08, Thu=0x10, Fri=0x20, Sat=0x40
  enabled: boolean;
  label: string;
  melody: string;
}

export interface AlarmsState {
  alarms: Alarm[];
  ringing: boolean;
}
