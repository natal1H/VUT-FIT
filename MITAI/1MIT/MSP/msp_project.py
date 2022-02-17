import numpy as np
import scipy.stats as stats
import pandas as pd
import pingouin as pg
import statsmodels.api as sm

"""
MSP projekt
Autor: Natália Holková (xholko02)
"""

def task1():
    print("MSP - 1. úloha\n--------------")

    alpha = 0.05

    # Dáta o starom prevádzkovateľovi pripojenia (odozva v [ms])
    X = np.array([22.43, 22.39, 24.18, 22.22, 25.52, 20.27, 24.88, 22.26, 24.97, 19.27,
         27.77, 20.52, 23.25, 21.37, 25.30, 19.03, 23.41, 18.64, 23.78, 18.74,
         24.94, 19.34, 23.58, 20.42, 24.98, 23.06, 25.54, 21.18, 25.82, 20.02,
         26.95, 21.73, 24.47, 21.36, 25.90, 22.38, 23.55, 20.01, 25.98, 19.69,
         24.96, 21.91, 24.89, 17.81, 24.72, 21.17, 24.72, 19.32, 25.97, 19.10])

    # Dáta o novom prevádzkovateľovi pripojenia (odozva v [ms])
    Y = np.array([23.13, 24.81, 28.78, 25.42, 25.51, 24.10, 23.00, 22.42, 20.86, 23.73,
         20.88, 24.75, 25.43, 22.81, 23.40, 24.14, 25.63, 22.34, 24.41, 23.33,
         20.06, 20.87, 20.88, 24.17, 25.91, 22.28, 22.29, 24.78, 25.53, 23.47,
         24.39, 24.05, 22.30, 23.62, 23.91, 26.13, 24.85, 23.71, 22.23, 24.75,
         21.50, 20.73, 25.28, 22.25, 24.90, 24.77, 25.61, 23.38, 25.02, 24.55])

    # Treba otestovať, či je má X a Y normálne rozdelenie
    # Využijeme Kolmogorov-Smirnov test
    print("Lilliefors test")
    print("\tH0: Má normálne rozdelenie")
    print("\tHA: Nemá normálne rozdelenie")
    print("\t----------------------------")

    # ak pval <= alpha --> zamietame H0
    # ak pval > alpha --> nezamietame H0

    # Test normality X
    D, pval = sm.stats.lilliefors(X)
    print("\tp-value:", pval.round(3))
    if pval <= alpha:
        print("\tH0 zamietam, X nemá normálne rozdelenie.")
    else:
        print("\tH0 nezamietam, X má normálne rozdelenie")

    # Test normality Y
    D, pval = sm.stats.lilliefors(Y)
    print("\tp-value:", pval.round(3))
    if pval <= alpha:
        print("\tH0 zamietam, Y nemá normálne rozdelenie.")
    else:
        print("\tH0 nezamietam, Y má normálne rozdelenie")

    # Zistili sme, že ani X aj Y majú normálne rozdelenie
    print("Studentov test")
    print("\tPriemer dát z X:", X.mean().round(3))
    print("\tPriemer dát z Y:", Y.mean().round(3))

    # Použijeme studentov test
    print("\tH0: X >= Y")
    print("\tHA: Y > X") # Väčšia odozva -> horšie pripojenie
    print("\t---------------------------------") 

    D, pval = stats.ttest_ind(X, Y)
    
    print("\tp-value:", pval.round(3))
    if pval / 2 <= alpha:
        print("\tH0 zamietame vzhľadom k HA")
    else:
        print("\tH0 nezamietame vzhľadom k HA")

def task2():
    print("MSP - 2. úloha\n--------------")
    # Nevyvážená dvojfaktorová ANOVA

    alpha = 0.05

    # Načítať vstupné hodnoty
    df = pd.DataFrame({'Ss': pd.Series(dtype='int'),
                       'Faktor 1': pd.Series(dtype='str'),
                       'Faktor 2': pd.Series(dtype='str'),
                       'Cas': pd.Series(dtype='int')})
    # Hodnoty z rána
    # ticho
    df = df.append({"Ss": 1, "Faktor 1":  "rano", "Faktor 2": "ticho", "Cas": 6}, ignore_index=True)
    df = df.append({"Ss": 2, "Faktor 1":  "rano", "Faktor 2": "ticho", "Cas": 8}, ignore_index=True)
    df = df.append({"Ss": 3, "Faktor 1":  "rano", "Faktor 2": "ticho", "Cas": 11}, ignore_index=True)
    df = df.append({"Ss": 4, "Faktor 1":  "rano", "Faktor 2": "ticho", "Cas": 9}, ignore_index=True) # Moja pridaná hodnota
    # hudba
    df = df.append({"Ss": 5, "Faktor 1":  "rano", "Faktor 2": "hudba", "Cas": 7}, ignore_index=True)
    df = df.append({"Ss": 6, "Faktor 1":  "rano", "Faktor 2": "hudba", "Cas": 8}, ignore_index=True)
    df = df.append({"Ss": 7, "Faktor 1":  "rano", "Faktor 2": "hudba", "Cas": 12}, ignore_index=True)
    df = df.append({"Ss": 8, "Faktor 1":  "rano", "Faktor 2": "hudba", "Cas": 10}, ignore_index=True)
    # hluk
    df = df.append({"Ss": 9, "Faktor 1":  "rano", "Faktor 2": "hluk", "Cas": 8}, ignore_index=True)
    df = df.append({"Ss": 10, "Faktor 1":  "rano", "Faktor 2": "hluk", "Cas": 7}, ignore_index=True)
    df = df.append({"Ss": 11, "Faktor 1":  "rano", "Faktor 2": "hluk", "Cas": 20}, ignore_index=True)
    # krik
    df = df.append({"Ss": 12, "Faktor 1":  "rano", "Faktor 2": "krik", "Cas": 13}, ignore_index=True)
    df = df.append({"Ss": 13, "Faktor 1":  "rano", "Faktor 2": "krik", "Cas": 21}, ignore_index=True)

    # Hodnoty z popoludnia
    # ticho
    df = df.append({"Ss": 14, "Faktor 1":  "popoludnie", "Faktor 2": "ticho", "Cas": 8}, ignore_index=True)
    df = df.append({"Ss": 15, "Faktor 1":  "popoludnie", "Faktor 2": "ticho", "Cas": 13}, ignore_index=True)
    df = df.append({"Ss": 16, "Faktor 1":  "popoludnie", "Faktor 2": "ticho", "Cas": 7}, ignore_index=True)
    # hudba
    df = df.append({"Ss": 17, "Faktor 1":  "popoludnie", "Faktor 2": "hudba", "Cas": 5}, ignore_index=True)
    df = df.append({"Ss": 18, "Faktor 1":  "popoludnie", "Faktor 2": "hudba", "Cas": 11}, ignore_index=True)
    df = df.append({"Ss": 19, "Faktor 1":  "popoludnie", "Faktor 2": "hudba", "Cas": 7}, ignore_index=True)
    df = df.append({"Ss": 20, "Faktor 1":  "popoludnie", "Faktor 2": "hudba", "Cas": 10}, ignore_index=True) # Moja pridaná hodnota
    # hluk
    df = df.append({"Ss": 21, "Faktor 1":  "popoludnie", "Faktor 2": "hluk", "Cas": 10}, ignore_index=True)
    df = df.append({"Ss": 22, "Faktor 1":  "popoludnie", "Faktor 2": "hluk", "Cas": 17}, ignore_index=True)
    df = df.append({"Ss": 23, "Faktor 1":  "popoludnie", "Faktor 2": "hluk", "Cas": 11}, ignore_index=True)
    df = df.append({"Ss": 24, "Faktor 1":  "popoludnie", "Faktor 2": "hluk", "Cas": 13}, ignore_index=True)
    # krik
    df = df.append({"Ss": 25, "Faktor 1":  "popoludnie", "Faktor 2": "krik", "Cas": 14}, ignore_index=True)

    # Hodnoty z večera
    # ticho
    df = df.append({"Ss": 26, "Faktor 1":  "vecer", "Faktor 2": "ticho", "Cas": 7}, ignore_index=True)
    df = df.append({"Ss": 27, "Faktor 1":  "vecer", "Faktor 2": "ticho", "Cas": 8}, ignore_index=True)
    df = df.append({"Ss": 28, "Faktor 1":  "vecer", "Faktor 2": "ticho", "Cas": 6}, ignore_index=True)
    # hluk
    df = df.append({"Ss": 29, "Faktor 1":  "vecer", "Faktor 2": "hudba", "Cas": 6}, ignore_index=True)
    df = df.append({"Ss": 30, "Faktor 1":  "vecer", "Faktor 2": "hudba", "Cas": 8}, ignore_index=True)
    df = df.append({"Ss": 31, "Faktor 1":  "vecer", "Faktor 2": "hudba", "Cas": 16}, ignore_index=True)
    df = df.append({"Ss": 32, "Faktor 1":  "vecer", "Faktor 2": "hudba", "Cas": 15}, ignore_index=True)
    # hluk
    df = df.append({"Ss": 33, "Faktor 1":  "vecer", "Faktor 2": "hluk", "Cas": 12}, ignore_index=True)
    df = df.append({"Ss": 34, "Faktor 1":  "vecer", "Faktor 2": "hluk", "Cas": 17}, ignore_index=True)
    df = df.append({"Ss": 35, "Faktor 1":  "vecer", "Faktor 2": "hluk", "Cas": 18}, ignore_index=True) # Moja pridaná hodnota
    # krik
    df = df.append({"Ss": 36, "Faktor 1":  "vecer", "Faktor 2": "krik", "Cas": 13}, ignore_index=True)
    df = df.append({"Ss": 37, "Faktor 1":  "vecer", "Faktor 2": "krik", "Cas": 17}, ignore_index=True)
    df = df.append({"Ss": 38, "Faktor 1":  "vecer", "Faktor 2": "krik", "Cas": 15}, ignore_index=True)
    df = df.append({"Ss": 39, "Faktor 1":  "vecer", "Faktor 2": "krik", "Cas": 22}, ignore_index=True)
    df = df.append({"Ss": 40, "Faktor 1":  "vecer", "Faktor 2": "krik", "Cas": 18}, ignore_index=True)

    aov = df.anova(dv="Cas", between=["Faktor 1", "Faktor 2"], effsize="n2").round(3)
    print(aov)

    # Interpretácia výsledkov
    # Závisí doba vyriešenia úlohy na dennej dobe?
    print("\tH0: doba výpočtu závisí na dennej dobe")
    pval_fakt1 = aov[aov["Source"] == "Faktor 1"]["p-unc"].values[0]
    if pval_fakt1 <= alpha:
        print("\tH0 zamietame")
    else:
        print("\tH0 nezamietame")

    # Závisí doba vyriešenia úlohy na hlučnosti okolia?
    print("\t------------------\n\tH0: doba výpočtu závisí na hlučnosti okolia")
    pval_fakt2 = aov[aov["Source"] == "Faktor 2"]["p-unc"].values[0]
    if pval_fakt2 <= alpha:
        print("\tH0 zamietame")
    else:
        print("\tH0 nezamietame")

    # Závisí doba vyriešenia úlohy na kombinácii oboch faktorov?
    print("\t------------------\n\tH0: doba výpočtu závisí na kombinácii oboch faktorov")
    pval_komb = aov[aov["Source"] == "Faktor 1 * Faktor 2"]["p-unc"].values[0]
    if pval_komb <= alpha:
        print("\tH0 zamietame")
    else:
        print("\tH0 nezamietame")

def task3():
    print("MSP - 3. úloha\n--------------")
    # Dotazník:
    #   1.) Akú známku ste mali na štátniciach? (A/B/C/D/E)
    #   2.) Koľko jazykov ovládate (1/2/3/4/5 a viac)

    # Dáta z dotazníku    
    data = [['A', '1'] for i in range(7)] + \
            [['A', '2'] for i in range(5)] + \
            [['A', '3'] for i in range(7)] + \
            [['A', '4'] for i in range(10)] + \
            [['A', '5+'] for i in range(9)] + \
            [['B', '1'] for i in range(8)] + \
            [['B', '2'] for i in range(12)] + \
            [['B', '3'] for i in range(15)] + \
            [['B', '4'] for i in range(5)] + \
            [['B', '5+'] for i in range(5)] + \
            [['C', '1'] for i in range(9)] + \
            [['C', '2'] for i in range(14)] + \
            [['C', '3'] for i in range(10)] + \
            [['C', '4'] for i in range(8)] + \
            [['C', '5+'] for i in range(8)] + \
            [['D', '1'] for i in range(6)] + \
            [['D', '2'] for i in range(7)] + \
            [['D', '3'] for i in range(6)] + \
            [['D', '4'] for i in range(7)] + \
            [['D', '5+'] for i in range(5)] + \
            [['E', '1'] for i in range(7)] + \
            [['E', '2'] for i in range(7)] + \
            [['E', '3'] for i in range(7)] + \
            [['E', '4'] for i in range(5)] + \
            [['E', '5+'] for i in range(5)]

    df = pd.DataFrame(data, columns = ['Štátnice známka', 'Počet jazykov']) 


    # Vytvorenie kontingenčnej tabuľky
    data_crosstab = pd.crosstab(df['Štátnice známka'], df['Počet jazykov'], margins=True, margins_name="Total")
    print(data_crosstab)

    alpha = 0.05

    # Test dobre zhody
    chi_square = 0
    rows = df['Štátnice známka'].unique()
    columns = df['Počet jazykov'].unique()
    for i in columns:
        for j in rows:
            O = data_crosstab[i][j]
            E = data_crosstab[i]['Total'] * data_crosstab['Total'][j] / data_crosstab['Total']['Total']
            chi_square += (O - E)**2 / E
    print(E)

    print("H0: známka zo štátnic a počet jazykov sú nezávislé")
    # Výpočet p hodnoty
    p_val = 1 - stats.norm.cdf(chi_square, (len(rows)-1)*(len(columns)-1))
    print("chisquare-score:", chi_square, ", p_val:", p_val)
    if p_val <= alpha:
        print("H0 sa zamieta")
    else:
        print("H0 sa nezamieta, známka zo štátnica a počet jazykov sú nezávislé.")            

if __name__ == "__main__":
    print("MSP projekt")
    task1()
    task2()
    task3()
 