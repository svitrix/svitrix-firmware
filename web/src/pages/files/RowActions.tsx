import { useTranslation } from "react-i18next";
import styles from "./Files.module.css";

/** Hover/focus/touch-revealed icon cluster. Delete is neutral in the cluster and
 *  only goes red on its own hover; it routes through a confirm dialog upstream. */
export function RowActions({
  canDownload,
  canEdit,
  canDelete,
  onDownload,
  onEdit,
  onDelete,
}: {
  canDownload: boolean;
  canEdit: boolean;
  canDelete: boolean;
  onDownload: () => void;
  onEdit: () => void;
  onDelete: () => void;
}) {
  const { t } = useTranslation();
  return (
    <div class={styles.actions}>
      {canDownload && (
        <button class={styles.iconBtn} title={t("common.download")} aria-label={t("common.download")} onClick={onDownload}>
          <svg width="17" height="17" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true">
            <path d="M12 4v10m0 0 4-4m-4 4-4-4" />
            <path d="M5 18h14" />
          </svg>
        </button>
      )}
      {canEdit && (
        <button class={styles.iconBtn} title={t("common.edit")} aria-label={t("common.edit")} onClick={onEdit}>
          <svg width="17" height="17" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true">
            <path d="M4 20h4L18.5 9.5a2 2 0 0 0-3-3L5 17z" />
            <path d="M13.5 6.5l3 3" />
          </svg>
        </button>
      )}
      {canDelete && (
        <button class={`${styles.iconBtn} ${styles.iconDelete}`} title={t("common.delete")} aria-label={t("common.delete")} onClick={onDelete}>
          <svg width="17" height="17" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.8" stroke-linecap="round" stroke-linejoin="round" aria-hidden="true">
            <path d="M5 7h14M10 4h4M6 7l1 12a2 2 0 0 0 2 2h6a2 2 0 0 0 2-2l1-12" />
            <path d="M10 11v6M14 11v6" />
          </svg>
        </button>
      )}
    </div>
  );
}
