import { createContext } from "preact";
import { useContext, useState, useCallback } from "preact/hooks";
import { translations } from "./translations";
import type { Language } from "./translations";

type Translations = typeof translations.es | typeof translations.en;

type LanguageContextType = {
  lang: Language;
  setLang: (lang: Language) => void;
  t: Translations;
};

const LanguageContext = createContext<LanguageContextType | null>(null);

const STORAGE_KEY = "svitrix-lang";

function getInitialLang(): Language {
  const stored = localStorage.getItem(STORAGE_KEY);
  if (stored === "en" || stored === "es") return stored;
  return "es"; // Spanish by default
}

export function LanguageProvider({ children }: { children: preact.ComponentChildren }) {
  const [lang, setLangState] = useState<Language>(getInitialLang);

  const setLang = useCallback((newLang: Language) => {
    setLangState(newLang);
    localStorage.setItem(STORAGE_KEY, newLang);
  }, []);

  const t = translations[lang];

  return (
    <LanguageContext.Provider value={{ lang, setLang, t }}>
      {children}
    </LanguageContext.Provider>
  );
}

export function useTranslation() {
  const context = useContext(LanguageContext);
  if (!context) {
    throw new Error("useTranslation must be used within LanguageProvider");
  }
  return context;
}

export function useT() {
  return useTranslation().t;
}
