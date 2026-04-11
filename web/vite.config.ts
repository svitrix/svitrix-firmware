import { defineConfig } from "vite";
import preact from "@preact/preset-vite";
import { compression } from "vite-plugin-compression2";
import path from "path";

// Device IP — override via .env.local: VITE_DEVICE_IP=192.168.1.42
const deviceIP = process.env.VITE_DEVICE_IP || "192.168.50.92";
const target = `http://${deviceIP}`;

const proxyRoutes = [
  "/api", "/list", "/edit", "/scan", "/status",
  "/connect", "/restart", "/update", "/version",
  "/save", "/DoNotTouch.json",
];

// Post-build: replace type="module" with plain script tag for IIFE compatibility
function fixScriptType() {
  return {
    name: "fix-script-type",
    enforce: "post" as const,
    generateBundle(_: unknown, bundle: Record<string, { type: string; source?: string }>) {
      for (const file of Object.values(bundle)) {
        if (file.type === "asset" && typeof file.source === "string" && file.source.includes("type=\"module\"")) {
          file.source = file.source
            .replace(' type="module" crossorigin', "")
            .replace(' crossorigin', "");
        }
      }
    },
  };
}

export default defineConfig({
  plugins: [
    preact(),
    fixScriptType(),
    compression({
      algorithms: ["gzip"],
      include: /\.(js|css|html)$/,
      deleteOriginalAssets: true,
    }),
  ],
  build: {
    outDir: path.resolve(__dirname, "../data"),
    emptyOutDir: true,
    target: "es2018",
    modulePreload: false,
    cssCodeSplit: false,
    assetsInlineLimit: 4096,
    rollupOptions: {
      output: {
        manualChunks: undefined,
        format: "iife",
        entryFileNames: "app.js",
        assetFileNames: "[name][extname]",
      },
    },
  },
  server: {
    proxy: Object.fromEntries(proxyRoutes.map((r) => [r, target])),
  },
});
