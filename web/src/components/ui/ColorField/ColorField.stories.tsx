import type { Meta } from "@storybook/preact-vite";
import { useState } from "preact/hooks";
import { ColorField } from "./ColorField";

const meta: Meta<typeof ColorField> = {
  title: "Components/ColorField",
  component: ColorField,
};
export default meta;

export const Accent = {
  render: () => {
    const [v, setV] = useState(0xffc93c);
    return <ColorField label="Text color" value={v} onChange={setV} />;
  },
};
