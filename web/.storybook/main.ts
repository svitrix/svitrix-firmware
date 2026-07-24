import type { StorybookConfig } from "@storybook/preact-vite";

const config: StorybookConfig = {
  stories: ["../src/**/*.stories.@(ts|tsx)"],
  addons: ["@storybook/addon-docs", "@storybook/addon-a11y"],
  framework: "@storybook/preact-vite",
  async viteFinal(cfg) {
    // The app's vite config gzips and DELETES original assets (device build).
    // Strip that plugin so Storybook's own static build keeps its .js files.
    cfg.plugins = (cfg.plugins ?? []).filter(
      (p) => !(p && typeof p === "object" && "name" in p && p.name === "vite-plugin-compression")
    );
    return cfg;
  },
};

export default config;
