declare module "preact-i18n" {
  /** Look up a nested `id` in `dictionary`, interpolating `{{field}}` templates.
   *  Returns `fallback` (or `id`) when not found. */
  export function translate(
    id: string,
    scope: string,
    dictionary: unknown,
    fields?: Record<string, string | number> | undefined,
    plural?: number,
    fallback?: string
  ): string;
}
