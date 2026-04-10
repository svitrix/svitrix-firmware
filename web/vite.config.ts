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

export default defineConfig({
  plugins: [
    preact(),
    compression({
      algorithms: ["gzip"],
      include: /\.(js|css|html)$/,
      deleteOriginalAssets: true,
    }),
  ],
  build: {
    outDir: path.resolve(__dirname, "../data/web"),
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
