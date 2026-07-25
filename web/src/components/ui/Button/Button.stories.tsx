import type { Meta, StoryObj } from "@storybook/preact-vite";
import { Button } from "./Button";

const meta: Meta<typeof Button> = {
  title: "Components/Button",
  component: Button,
  args: { children: "Button" },
  argTypes: {
    variant: {
      control: "select",
      options: ["default", "primary", "danger"],
    },
  },
};
export default meta;

type Story = StoryObj<typeof Button>;

export const Primary: Story = { args: { variant: "primary", children: "Save" } };
export const Default: Story = { args: { variant: "default", children: "Cancel" } };
export const Danger: Story = { args: { variant: "danger", children: "Delete" } };
export const Disabled: Story = {
  args: { variant: "primary", children: "Save", disabled: true },
};
export const Loading: Story = {
  args: { variant: "primary", children: "Save", loading: true },
};

export const AllVariants: Story = {
  render: () => (
    <div style={{ display: "flex", gap: "12px", flexWrap: "wrap" }}>
      <Button variant="primary">Primary</Button>
      <Button variant="default">Default</Button>
      <Button variant="danger">Danger</Button>
    </div>
  ),
};
