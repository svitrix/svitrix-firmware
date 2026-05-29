import { useRef, useState } from "preact/hooks";
import { useT } from "../../i18n";
import styles from "./FileInput.module.css";

interface FileInputProps {
  accept?: string;
  disabled?: boolean;
  onChange: (file: File) => void;
  buttonText?: string;
}

export function FileInput({ accept, disabled, onChange, buttonText }: FileInputProps) {
  const inputRef = useRef<HTMLInputElement>(null);
  const [fileName, setFileName] = useState<string | null>(null);
  const t = useT();

  function handleClick() {
    inputRef.current?.click();
  }

  function handleChange(e: Event) {
    const file = (e.target as HTMLInputElement).files?.[0];
    if (file) {
      setFileName(file.name);
      onChange(file);
    }
  }

  return (
    <div class={styles.wrapper}>
      <input
        ref={inputRef}
        type="file"
        accept={accept}
        disabled={disabled}
        onChange={handleChange}
        class={styles.hidden}
      />
      <button
        type="button"
        class={styles.button}
        onClick={handleClick}
        disabled={disabled}
      >
        {buttonText || t.chooseFile}
      </button>
      <span class={styles.fileName}>
        {fileName || t.noFileSelected}
      </span>
    </div>
  );
}
