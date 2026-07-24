import type { Preview, Decorator } from "@storybook/preact-vite";
import "../src/styles/global.css";

/** Applies the selected Liquid Glass theme + scene background around each story. */
const withTheme: Decorator = (Story, context) => {
  const theme = context.globals.theme ?? "dark";
  document.documentElement.setAttribute("data-theme", theme);
  return (
    <div
      data-theme={theme}
      style={{
        minHeight: "100vh",
        padding: "24px",
        background: "var(--scene)",
        color: "var(--text)",
        fontFamily: "var(--font)",
      }}
    >
      <Story />
    </div>
  );
};

const preview: Preview = {
  decorators: [withTheme],
  globalTypes: {
    theme: {
      description: "Liquid Glass theme",
      defaultValue: "dark",
      toolbar: {
        title: "Theme",
        icon: "circlehollow",
        items: [
          { value: "dark", title: "Dark", icon: "moon" },
          { value: "light", title: "Light", icon: "sun" },
        ],
        dynamicTitle: true,
      },
    },
  },
  parameters: {
    layout: "fullscreen",
    controls: {
      matchers: { color: /(background|color)$/i, date: /Date$/i },
    },
    a11y: { test: "todo" },
  },
};

export default preview;
