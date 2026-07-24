import type { Meta } from "@storybook/preact-vite";
import { useState } from "preact/hooks";
import { Select } from "./Select";

const meta: Meta<typeof Select> = {
  title: "Components/Select",
  component: Select,
  tags: ["autodocs"],
};
export default meta;

export const TimeMode = {
  render: () => {
    const [v, setV] = useState<string | number>(0);
    return (
      <Select
        label="Time mode"
        value={v}
        onChange={setV}
        options={[
          { value: 0, label: "HH:MM" },
          { value: 1, label: "HH:MM:SS" },
          { value: 2, label: "HH:MM (12h)" },
        ]}
      />
    );
  },
};
