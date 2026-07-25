import type { Meta } from "@storybook/preact-vite";
import { useState } from "preact/hooks";
import { Card } from "./Card";
import { Button, FormRow, Slider, TextField, Toggle } from "..";

const meta: Meta<typeof Card> = {
  title: "Components/Card",
  component: Card,
  args: {
    title: "Display",
    subtitle: "Brightness, color and matrix behaviour",
  },
};
export default meta;

export const Basic = {
  args: { title: "Display", subtitle: "Glass section container" },
  render: (args: { title: string; subtitle?: string }) => (
    <Card title={args.title} subtitle={args.subtitle}>
      <p style={{ color: "var(--label-secondary)" }}>
        Cards are the primary Liquid Glass surface — translucent, blurred, with a
        specular top edge.
      </p>
    </Card>
  ),
};

export const WithForm = {
  render: () => {
    const [name, setName] = useState("Svitrix");
    const [auto, setAuto] = useState(true);
    const [bright, setBright] = useState(180);
    return (
      <Card title="Display" subtitle="Brightness, color and matrix behaviour">
        <div style={{ display: "flex", flexDirection: "column", gap: "12px" }}>
          <TextField label="Device name" value={name} onChange={setName} />
          <FormRow>
            <Toggle label="Auto brightness" checked={auto} onChange={setAuto} />
          </FormRow>
          <Slider
            label="Brightness"
            min={0}
            max={255}
            value={bright}
            onChange={setBright}
          />
          <div style={{ display: "flex", gap: "8px" }}>
            <Button variant="primary">Save</Button>
            <Button variant="default">Reset</Button>
          </div>
        </div>
      </Card>
    );
  },
};
