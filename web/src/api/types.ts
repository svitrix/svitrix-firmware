export interface Settings {
  MATP: boolean;
  ABRI: boolean;
  BRI: number;
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
  SSPEED: number;
  TIM: boolean;
  DAT: boolean;
  HUM: boolean;
  TEMP: boolean;
  BAT: boolean;
  VOL: number;
}

export interface Stats {
  bat: number;
  bat_raw: number;
  type: number;
  lux: number;
  ldr_raw: number;
  ram: number;
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
  duration?: number;
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

export interface TransitionInfo {
  name: string;
}

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
  [key: string]: unknown;
}
