import { render } from "preact";
import Router from "preact-router";
import { Nav } from "./components/Nav";
import { ToastContainer } from "./components/Toast";
import { ScreenPage } from "./pages/Screen";
import { SettingsPage } from "./pages/Settings";
import { DataFetcherPage } from "./pages/DataFetcher";
import { BackupPage } from "./pages/Backup";
import { UpdatePage } from "./pages/Update";
import { FilesPage } from "./pages/Files";
import "./styles/global.css";

function App() {
  return (
    <div>
      <Nav />
      <main style={{ padding: "16px", maxWidth: 900, margin: "0 auto" }}>
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
