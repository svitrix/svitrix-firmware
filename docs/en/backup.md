# Backup

The backup and restore functionality is available in the [web interface](./webinterface) under the **Backup** page (`/backup`).

## Backup (Download)

Click **Download Backup** to save all files from the device's filesystem (icons, melodies, palettes, custom apps, configuration) as a single JSON file.

## Restore (Upload)

Select a previously downloaded backup file to restore. All files will be uploaded to the device, and it will reboot automatically after a successful restore.

::: warning
Restoring a backup will overwrite existing files on the device. Make sure you're restoring the correct backup.
:::
