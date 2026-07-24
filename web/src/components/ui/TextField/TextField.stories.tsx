import type { Meta } from "@storybook/preact-vite";
import { useState } from "preact/hooks";
import { TextField } from "./TextField";

const meta: Meta<typeof TextField> = {
  title: "Components/TextField",
  component: TextField,
  tags: ["autodocs"],
};
export default meta;

export const Text = {
  render: () => {
    const [v, setV] = useState("Svitrix");
    return <TextField label="Device name" value={v} onChange={setV} />;
  },
};

export const WithPlaceholder = {
  render: () => {
    const [v, setV] = useState("");
    return (
      <TextField
        label="MQTT host"
        placeholder="192.168.1.10"
        value={v}
        onChange={setV}
      />
    );
  },
};

export const Password = {
  render: () => {
    const [v, setV] = useState("");
    return (
      <TextField
        label="WiFi password"
        type="password"
        value={v}
        onChange={setV}
      />
    );
  },
};
