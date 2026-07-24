// English translations barrel. Each page/area lives in its own slice under
// locales/en/ and is merged here into the single `translation` namespace.
import nav from "./en/nav";
import common from "./en/common";
import screen from "./en/screen";
import stats from "./en/stats";
import dataFetcher from "./en/dataFetcher";
import backup from "./en/backup";
import update from "./en/update";
import files from "./en/files";
import settings from "./en/settingsInfra";
import settingsDisplay from "./en/settingsDisplay";

export default {
  nav,
  common,
  screen,
  stats,
  dataFetcher,
  backup,
  update,
  files,
  settings,
  settingsDisplay,
};
