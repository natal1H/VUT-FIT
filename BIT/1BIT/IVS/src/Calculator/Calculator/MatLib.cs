/**************************************************
 * Názov projektu: Kalkulačka
 * Súbor: MatLib.cs
 * Dátum: 6.4.2018
 * Posledná zmena: 20.4.2018
 * Autor: Natália Holková xholko02(at)fit.vutbr.cz
 * 
 * Popis: Matematická knižnica
 * 
 ***************************************************
/**
 * @file MatLib.cs
 * 
 * @brief Trieda MatLib - knižnica s matematickými operáciami
 * @author Natália Holková (xholko02)
 * 
 * @note .NET Framework v4.0
 */

using System;

namespace Calculator
{
    /**
     * @class Trieda s matematickými operáciami
     */
    public class MatLib
    {
        /**
         * @brief Vypočíta súčet dvoch čísiel
         * 
         * @param number1 Prvý sčítanec
         * @param number2 Druhý sčítanec
         * @return Súčet čísiel
         */
        public static double add(double number1, double number2)
        {
            return number1 + number2;
        }

        /**
         * @brief Vypočíta rozdiel dvoch čísiel
         * 
         * @param number1 Menšenec
         * @param number2 Menšiteľ
         * @return Rozdiel čísiel
         */
        public static double subtract(double number1, double number2)
        {
            return number1 - number2;
        }

        /**
         * @brief Vypočíta násobok dvoch čísiel
         * 
         * @param number1 Prvý činiteľ
         * @param number2 Druhý činiteľ
         * @return Násobok čísiel
         */
        public static double multiply(double number1, double number2)
        {
            return number1 * number2;
        }

        /**
         * @brief Vypočíta podiel dvoch čísiel
         * 
         * @param number1 Delenec
         * @param number2 Deliteľ
         * @return Podiel čísiel
         */
        public static double divide(double number1, double number2)
        {
            if (number2 == 0)
            {
                throw new DivideByZeroException("Chyba! Nie je povolené deliť 0.");
            }
            return number1 / number2;
        }

        /**
         * @brief Vypočíta faktoriál čísla
         * 
         * @param number Kladné celé číslo
         * @return Faktoriál čísla
         */
        public static long factorial(int number)
        {
            if (number < 0)
            {
                throw new ArgumentOutOfRangeException("Chyba! Nesmie byť záporné číslo.");
            }
            long res = 1;
            for (int i = 2; i <= number; i++)
                res = res * i;
            return res;
        }

        /**
         * @brief Vypočíta druhú mocninu čísla
         * 
         * @param number Základ mocniny
         * @return Druhá mocnina čísla
         */
        public static double square(double number)
        {
            return number * number;
        }

        /**
         * @brief Vypočíta n-tú mocninu zadaného čísla
         * 
         * @param number Základ mocniny
         * @param exp Exponent mocniny
         * @return N-tá mocnina čísla
         */
        public static double power(double number, int exp)
        {
            if (exp < 0)
            {
                throw new ArgumentOutOfRangeException("Chyba! Exponent musí byť prirodzené číslo.");
            }
            return Math.Pow(number, exp);
        }

        /**
         * @brief Vypočíta druhú odmocninu čísla
         * 
         * @param number Základ odmocniny
         * @return Druhá odmocnina
         */
        public static double sqrt(double number)
        {
            if (number < 0)
            {
                throw new ArgumentOutOfRangeException("Chyba! Nesmie byť záporné číslo pod odmocninou.");
            }
            return Math.Sqrt(number);
        }

        /**
         * @brief Vypočíta n-tú odmocninu čísla
         * 
         * @param number Základ odmocniny
         * @param root Stupeň odmocniny
         * @return N-tá odmocnina čísla
         */
        public static double genroot(double number, int root)
        {
            if (number < 0 && (root % 2 == 0 ))
            {
                throw new ArgumentOutOfRangeException("Chyba! Nesmie byť záporné číslo pod párnou odmocninou.");
            }
            if (number < 0 && (root % 2 == 1))
                return -(Math.Pow(-number, 1.0 / root));
            else
                return Math.Pow(number, 1.0 / root);
        }

        /**
         * @brief Vypočíta logaritmus reálneho čísla pri základe 10
         * 
         * @param number Argument logaritmu
         * @return Logaritmus čísla pri základe 10
         */
        public static double log10(double number)
        {
            if (number <= 0)
            {
                throw new ArgumentOutOfRangeException("Chyba! Argument musí byť kladné číslo.");
            }
            return Math.Log10(number);
        }
    }
}

/*** Koniec súboru MatLib.cs ***/
