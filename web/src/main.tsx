import { render } from "preact";
import { useState } from "preact/hooks";
import Router, { getCurrentUrl } from "preact-router";
import { Nav } from "./components/Nav";
import { ToastContainer } from "./components/Toast";
import { ScreenPage } from "./pages/screen";
import { SettingsPage } from "./pages/settings";
import { StatsPage } from "./pages/stats";
import { DataFetcherPage } from "./pages/data-fetcher";
import { BackupPage } from "./pages/backup";
import { UpdatePage } from "./pages/update";
import { FilesPage } from "./pages/files";
import { useTranslation } from "./i18n";
import { useTitle } from "./hooks/useTitle";
import "./i18n";
import "./styles/global.css";
import styles from "./main.module.css";

/** Route path → i18n key for the browser/tab title. Reuses the
 *  existing nav labels so no page component needs editing. */
const ROUTE_TITLES: Record<string, string> = {
  "/": "nav.screen",
  "/stats": "nav.stats",
  "/settings": "nav.settings",
  "/datafetcher": "nav.data",
  "/backup": "nav.backup",
  "/update": "nav.update",
  "/files": "nav.files",
};

function pathname(url: string): string {
  return url.split(/[?#]/)[0] || "/";
}

function App() {
  const { t } = useTranslation();
  const [path, setPath] = useState(pathname(getCurrentUrl()));
  useTitle(t(ROUTE_TITLES[path] ?? "nav.screen"));

  return (
    <div>
      <a href="#main" class="skip-link sr-only">
        {t("common.skipToContent")}
      </a>
      <Nav />
      <main id="main" tabIndex={-1} class={styles.main}>
        <Router onChange={(e) => setPath(pathname(e.url))}>
          <ScreenPage path="/" />
          <StatsPage path="/stats" />
          <SettingsPage path="/settings" />
          <DataFetcherPage path="/datafetcher" />
          <BackupPage path="/backup" />
          <UpdatePage path="/update" />
          <FilesPage path="/files" />
          <ScreenPage default />
        </Router>
      </main>
      <ToastContainer />
    </div>
  );
}

render(<App />, document.getElementById("app")!);
