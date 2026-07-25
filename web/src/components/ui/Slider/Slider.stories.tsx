import type { Meta } from "@storybook/preact-vite";
import { useState } from "preact/hooks";
import { Slider } from "./Slider";

const meta: Meta<typeof Slider> = {
  title: "Components/Slider",
  component: Slider,
};
export default meta;

export const Brightness = {
  render: () => {
    const [v, setV] = useState(180);
    return (
      <Slider label="Brightness" min={0} max={255} value={v} onChange={setV} />
    );
  },
};

export const WithUnit = {
  render: () => {
    const [v, setV] = useState(30);
    return (
      <Slider
        label="Scroll speed"
        min={0}
        max={100}
        step={5}
        unit="%"
        value={v}
        onChange={setV}
      />
    );
  },
};
