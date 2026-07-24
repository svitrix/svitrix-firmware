import type { Meta } from "@storybook/preact-vite";
import { useState } from "preact/hooks";
import { Toggle } from "./Toggle";

const meta: Meta<typeof Toggle> = {
  title: "Components/Toggle",
  component: Toggle,
  tags: ["autodocs"],
};
export default meta;

export const Interactive = {
  render: () => {
    const [on, setOn] = useState(true);
    return <Toggle label="Night mode" checked={on} onChange={setOn} />;
  },
};

export const OnAndOff = {
  render: () => {
    const [a, setA] = useState(true);
    const [b, setB] = useState(false);
    return (
      <div style={{ display: "flex", flexDirection: "column", gap: "12px" }}>
        <Toggle label="Enabled" checked={a} onChange={setA} />
        <Toggle label="Disabled" checked={b} onChange={setB} />
      </div>
    );
  },
};
