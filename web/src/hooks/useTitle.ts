import { useEffect } from "preact/hooks";

/**
 * Sets the document title to `Svitrix — <title>` for the lifetime of the
 * calling component / until `title` changes (WCAG 2.4.2 Page Titled).
 * Driven from the route→title map in main.tsx so page components don't each
 * need to manage their own title.
 */
export function useTitle(title: string): void {
  useEffect(() => {
    document.title = `Svitrix — ${title}`;
  }, [title]);
}
