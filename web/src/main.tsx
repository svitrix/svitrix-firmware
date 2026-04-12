import { render } from "preact";
import Router from "preact-router";
import { Nav } from "./components/Nav";
import { ToastContainer } from "./components/Toast";
import { ScreenPage } from "./pages/screen";
import { SettingsPage } from "./pages/settings";
import { DataFetcherPage } from "./pages/data-fetcher";
import { BackupPage } from "./pages/backup";
import { UpdatePage } from "./pages/update";
import { FilesPage } from "./pages/files";
import "./styles/global.css";
import styles from "./main.module.css";

function App() {
  return (
    <div>
      <Nav />
      <main class={styles.main}>
        <Router>
          <ScreenPage path="/" />
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
