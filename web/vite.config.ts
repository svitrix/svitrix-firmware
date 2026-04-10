import { defineConfig } from "vite";
import preact from "@preact/preset-vite";
import { compression } from "vite-plugin-compression2";
import path from "path";

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
    proxy: {
      "/api": "http://192.168.50.92",
      "/list": "http://192.168.50.92",
      "/edit": "http://192.168.50.92",
      "/scan": "http://192.168.50.92",
      "/status": "http://192.168.50.92",
      "/connect": "http://192.168.50.92",
      "/restart": "http://192.168.50.92",
      "/update": "http://192.168.50.92",
      "/version": "http://192.168.50.92",
      "/save": "http://192.168.50.92",
      "/DoNotTouch.json": "http://192.168.50.92",
    },
  },
});
