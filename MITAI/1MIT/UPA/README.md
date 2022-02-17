# UPA - Covid-19

## Členovia tímu

- Filip Bali (xbalif00)
- Natália Holková (xholko02)
- Roland Žitný (xzitny01)
<br/><br/>
 
***

## Štruktúra repozitára
```
UPA-project
│   README.md
│   doc.pdf - dokumentácia    
│   .gitignore
│   requirements.txt - zoznam požadovaných knižníc
│   data_loader.py - implementácia prvej časti projektu
│   vizualizer.py - implementácia druhej časti projektu
│
└───queries
│   │   __init__.py
│   │   A1.py - dotaz A1
│   │   A3.py - dotaz A3
│   │   B1.py - dotaz B1
│   │   C1.py - dolovacia úloha C1
│   │   VL1.py - prvý vlastný dotaz
│   │   VL2.py - druhý vlastný dotaz
│   
└───GUI
    │   __init__.py
    │   main.py - spúštanie GUI
    │   MainWindow.py
    |   MainWindow.ui
    │   unnamed.jpg - logo VUT
```

## Inštalácia:

Inštalácia MongoDB - https://docs.mongodb.com/manual/administration/install-community/

```
python3 -m venv ./env-upa
source ./env-upa/bin/activate
pip3 install -r requirements.txt
```
## Spustenie 

### Spustenie prvej časti:

```
python3 data_loader.py --mongo mongodb://localhost:27017/ -f download_data_folder -d UPA-db
```
### Spustenie druhej časti:
Vyžaduje najskôr vytvoriť databázu v prvej časti.

Vygenerovanie všetkého naraz:
```
python3 vizualizer.py --mongo mongodb://localhost:27017/ -d UPA-db
```

Vygenerovanie výsledkov jednotlivých dotazov:
```
python3 queries/A1.py --mongo mongodb://localhost:27017/ -d UPA-db    # pre dotaz A1
python3 queries/A3.py --mongo mongodb://localhost:27017/ -d UPA-db    # pre dotaz A3
python3 queries/B1.py --mongo mongodb://localhost:27017/ -d UPA-db    # pre dotaz B1
python3 queries/C1.py --mongo mongodb://localhost:27017/ -d UPA-db    # pre dotaz C1
python3 queries/VL1.py --mongo mongodb://localhost:27017/ -d UPA-db   # pre dotaz VL1
python3 queries/VL2.py --mongo mongodb://localhost:27017/ -d UPA-db   # pre dotaz VL2
```
Spustenie GUI (je potrebné najprv spustiť *vizualizer.py*):
```
python3 GUI/main.py
```

