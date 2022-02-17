VUT FIT IJA 2018/2019

Vypracovali:
  - Natália Holková (xholko02)
  - Matej Novák     (xnovak2f)

Zadanie:
  Navrhnite a implementujte aplikáciu pre prehrávanie šachových partií

Specifikácia požiadavkov:
  1. Základné požiadavky
       - aplikace umožňuje přehrávat šachové partie zaznamenané v této notaci (nepoužívejte ani nevymýšlejte jinou) a zaznamenávat tahy provedené uživatelem
       - aplikace obsahuje grafické uživatelské rozhraní
           - šachovnice je zobrazena včetně popisů řádků (1-8) a sloupců (a-h)
           - figurky jsou reprezentovány obrázky
           - vedle šachovnice je zobrazen zápis partie
           - je možné otevřít více partií ve více tabech (nikoliv oknech); tyto partie jsou nezávislé
       - vždy se provádí kontrola, zda jsou tahy přípustné podle pravidel (není třeba uvažovat všechny detaily, postačí základní tahy figur)
  2. Základné funkcie
       - po spuštění je možné načíst partii, provede se ověření formátu zápisu
       - přehrávání je buď manuální nebo automatizované, je možné kdykoliv přepínat mezi těmito režimy; je možné přehrávat (krokovat) partii dopředu i zpět
           - manuální: ovládá uživatel prostřednictvím tlačítek
           - automatizované: je možné nastavit prodlevu mezi jednotlivými kroky; kdykoliv je možné zastavit a pokračovat dopředu či zpět
       - kdykoliv je možné restartovat přehrávání na začátek partie
       - při přehrávání se vždy zobrazuje celý zápis partie a zvýrazňuje pozice aktuálního tahu
       - kliknutím na pozici v zápisu lze okamžitě zobrazit stav partie na této pozici; další krokování pokračuje z této pozice
  3. Vlastné ťahy užívateľa
       - pokud uživatel provede vlastní tah při zastaveném přehrávání, zruší se zbytek zápisu od aktuální pozice a další vývoj partie již provádí uživatel
       - nové tahy uživatele se zaznamenávají do zápisu
       - tuto novou partii je opět možné přehrávat
       - uvedený postup je možné opakovat - pokud je rozehrána a krokována vlastní partie a je právě zobrazen některý ze starších tahů, může uživatel opět provést vlastní tah; v tom případě se opět zruší zbytek zápisu od zobrazeného tahu
       - možnost operace undo a redo: tahy provedené uživatelem lze vracet zpět (undo) a opět obnovit (redo) až do okamžiku před prvním vlastním tahem, tj. až do návratu k původní načtené partii (undo a redo při přehrávání partie nedává smysl)
       - nově vytvořený zápis partie lze uložit

Podmienky vypracovania:
  - tímový projekt
  - pre realizáciu využiť Java SE 8
  - pre realizáciu GUI využiť JavaFX
  - pre preklad použiť ant

Preklad:
  ant compile
    - zkompiluje zdrojové texty, classy súbory budú v adresári build
    - vytvorí jar archív ija-app.jar v adresári dest

Spustenie:
  ant run
    - spustiť v koreňovom adresári pre spustenie

Dokumentácia:
  ant doc
    - spustiť v koreňovom adresári pre vygenerovanie javadoc