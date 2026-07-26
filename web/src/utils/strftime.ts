/**
 * Minimal, dependency-free C `strftime` formatter.
 *
 * Supports the common tokens the firmware uses. Day/month names are English —
 * this is an APPROXIMATION of the device's locale (the device default is
 * English), so the browser preview may differ from a localized device.
 * Unknown tokens are passed through verbatim (the `%` is kept), never thrown.
 */

const DAYS_LONG = [
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday",
];
const DAYS_SHORT = ["Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"];
const MONTHS_LONG = [
  "January",
  "February",
  "March",
  "April",
  "May",
  "June",
  "July",
  "August",
  "September",
  "October",
  "November",
  "December",
];
const MONTHS_SHORT = [
  "Jan",
  "Feb",
  "Mar",
  "Apr",
  "May",
  "Jun",
  "Jul",
  "Aug",
  "Sep",
  "Oct",
  "Nov",
  "Dec",
];

const pad2 = (n: number) => String(n).padStart(2, "0");

function dayOfYear(d: Date): number {
  const start = new Date(d.getFullYear(), 0, 0);
  const diff = d.getTime() - start.getTime();
  return Math.floor(diff / 86_400_000);
}

export function formatStrftime(fmt: string, d: Date): string {
  const hours = d.getHours();
  const hours12 = hours % 12 === 0 ? 12 : hours % 12;

  return fmt.replace(/%(.)/g, (match, token: string): string => {
    switch (token) {
      case "H":
        return pad2(hours);
      case "I":
        return pad2(hours12);
      case "M":
        return pad2(d.getMinutes());
      case "S":
        return pad2(d.getSeconds());
      case "p":
        return hours < 12 ? "AM" : "PM";
      case "P":
        return hours < 12 ? "am" : "pm";
      case "d":
        return pad2(d.getDate());
      case "e":
        return String(d.getDate()).padStart(2, " ");
      case "m":
        return pad2(d.getMonth() + 1);
      case "y":
        return pad2(d.getFullYear() % 100);
      case "Y":
        return String(d.getFullYear());
      case "a":
        return DAYS_SHORT[d.getDay()];
      case "A":
        return DAYS_LONG[d.getDay()];
      case "b":
        return MONTHS_SHORT[d.getMonth()];
      case "B":
        return MONTHS_LONG[d.getMonth()];
      case "j":
        return String(dayOfYear(d)).padStart(3, "0");
      case "%":
        return "%";
      default:
        // Unknown token — pass through verbatim rather than throwing.
        return match;
    }
  });
}
