import { render } from "preact";
import Router from "preact-router";
import { Nav } from "./components/Nav";
import { ToastContainer } from "./components/Toast";
import { LanguageProvider } from "./i18n";
import { ScreenPage } from "./pages/screen";
import { SettingsPage } from "./pages/settings";
import { MqttPage } from "./pages/mqtt";
import { DisplayPage } from "./pages/display";
import { AppsPage } from "./pages/apps";
import { DateTimePage } from "./pages/datetime";
import { SoundPage } from "./pages/sound";
import { DataFetcherPage } from "./pages/data-fetcher";
import { BackupPage } from "./pages/backup";
import { UpdatePage } from "./pages/update";
import { FilesPage } from "./pages/files";
import { IconsPage } from "./pages/icons";
import { SystemPage } from "./pages/system";
import { AutonomousPage } from "./pages/autonomous/Autonomous";
import "./styles/global.css";
import styles from "./main.module.css";

function App() {
  return (
    <LanguageProvider>
      <div>
        <Nav />
        <main class={styles.main}>
          <Router>
            <SettingsPage path="/" />
            <MqttPage path="/mqtt" />
            <DisplayPage path="/display" />
            <AppsPage path="/apps" />
            <DateTimePage path="/datetime" />
            <SoundPage path="/sound" />
            <ScreenPage path="/screen" />
            <DataFetcherPage path="/datafetcher" />
            <AutonomousPage path="/autonomous" />
            <FilesPage path="/files" />
            <IconsPage path="/icons" />
            <BackupPage path="/backup" />
            <SystemPage path="/system" />
            <UpdatePage path="/update" />
          </Router>
        </main>
        <ToastContainer />
      </div>
    </LanguageProvider>
  );
}

render(<App />, document.getElementById("app")!);
