import { useTranslation } from "react-i18next";
import { useStatsPoll } from "./useStatsPoll";
import { StatTile, Meter, SignalBars, BatteryGlyph, Sparkline, StatePill } from "./viz";
import {
  formatBytes,
  formatCount,
  formatUptime,
  ramBand,
  humBand,
  tempBand,
  wifiInfo,
} from "./format";
import styles from "./stats.module.css";

/** Device statistics dashboard — self-contained (polls /api/stats itself, no
 *  SettingsProvider needed). Started from null; the first poll fills it in. */
export function StatsPage(_props: { path?: string }) {
  const { t } = useTranslation();
  const { stats: s, history } = useStatsPoll(null);
  if (!s) return <p>{t("common.loading")}</p>;

  const ramKb = Math.round(s.ram / 1024);
  const wifi = wifiInfo(s.wifi_signal);

  return (
    <div class="card">
      <section class={styles.section}>
        <div class={styles.groupHead}>{t("stats.device")}</div>
        <div class={styles.kpis}>
          <StatTile label={t("stats.firmware")} value={"v" + s.version} />
          <StatTile label={t("stats.uptime")} value={formatUptime(s.uptime)} />
          {s.ip_address && <StatTile label={t("stats.ipAddress")} value={s.ip_address} mono />}
        </div>
        <div class={styles.trend}>
          <StatTile label={t("stats.freeRam")} value={formatBytes(s.ram)}>
            <Meter value={ramKb} max={320} status={ramBand(ramKb)} ariaLabel={t("stats.freeRam")} />
            <Sparkline data={history.ram} minSpan={20} />
          </StatTile>
        </div>
      </section>

      <section class={styles.section}>
        <div class={styles.groupHead}>{t("stats.environment")}</div>
        <div class={styles.kpis}>
          {s.hum > 0 && (
            <StatTile label={t("stats.humidity")} value={s.hum} unit="%">
              <Meter value={s.hum} max={100} status={humBand(s.hum)} ariaLabel={t("stats.humidity")} />
            </StatTile>
          )}
          <StatTile label={t("stats.ambient")} value={formatCount(s.lux)} unit="lx" />
        </div>
        {s.temp > 0 && (
          <div class={styles.trend}>
            <StatTile label={t("stats.temperature")} value={s.temp} unit="°">
              <Meter value={s.temp} min={0} max={40} status={tempBand(s.temp)} ariaLabel={t("stats.temperature")} />
              <Sparkline data={history.temp} minSpan={4} />
            </StatTile>
          </div>
        )}
      </section>

      <section class={styles.section}>
        <div class={styles.groupHead}>{t("stats.displayPower")}</div>
        <div class={styles.kpis}>
          <StatTile label={t("stats.brightness")} value={s.bri}>
            <Meter value={s.bri} max={255} ariaLabel={t("stats.brightness")} />
          </StatTile>
          {s.bat > 0 && (
            <StatTile label={t("stats.battery")} value={s.bat} unit="%">
              <BatteryGlyph percent={s.bat} />
            </StatTile>
          )}
          <StatTile label={t("stats.matrix")} value={<StatePill on={s.matrix} />} />
          {s.app && <StatTile label={t("stats.currentApp")} value={s.app} />}
          <StatTile label={t("stats.mqttMsgs")} value={formatCount(s.messages)} unit={t("stats.msgsUnit")} />
        </div>
        <div class={styles.trend}>
          <StatTile label={t("stats.wifi")} value={s.wifi_signal} unit="dBm">
            <span class={styles.statusRow}>
              <SignalBars dbm={s.wifi_signal} />
              {wifi.word}
            </span>
            <Sparkline data={history.wifi} minSpan={10} />
          </StatTile>
        </div>
      </section>
    </div>
  );
}
