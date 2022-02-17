/**************************************************
 * Názov projektu: Kalkulačka - časť výpočet smerodajnej odchýlky
 * Súbor: Program.cs
 * Dátum: 23.4.2018
 * Posledná zmena: 23.4.2018
 * Autor: Natália Holková xholko02(at)fit.vutbr.cz
 * 
 * Popis: Výpočet smerodajnej odchýlky
 * 
 ***************************************************
/**
 * @file Program.cs
 * 
 * @brief Program na výpočet smerodajnej odchýlky
 * @author Natália Holková (xholko02)
 * 
 * @note .NET Framework v4.0
 */

using System;
using System.Collections.Generic;
using Calculator;

namespace StandartDeviation
{
    /**
     * @class Trieda na výpočet smerodajnej odchýlky
     */
    class Program
    {
        /**
         * @brief Výpočet aritmetického priemeru
         * 
         * @param list Zoznam double, z ktorého vypočítať priemer
         * @return Aritmetický priemer
         */
        public static double GetArithmeticMean(List<double> list)
        {
            int N = list.Count;
            double res = 0, sum = 0;
            for (int i = 0; i < N; i++)
            {
                sum = MatLib.add(sum, list[i]);
            }
            try
            {
                res = MatLib.divide(sum, N);
            }
            catch (DivideByZeroException e)
            {
                Console.WriteLine("{0}", e.Message);
                Console.WriteLine("Press any key to continue . . .");
                Console.Read();
                Environment.Exit(-1);
            }
            return res;
        }

        /**
         * @brief Výpočet sumy druhých mocnín prvkov
         * 
         * @param list Zoznam double, z ktorého vypočítať sumu druhých mocnín prvkov
         * @return Suma druhých mocnín
         */
        public static double GetSumSqr(List<double> list)
        {
            int N = list.Count;
            double sum = 0;
            for (int i = 0; i < N; i++)
            {
                sum = MatLib.add(sum, MatLib.square(list[i]));
            }
            return sum;
        }

        /**
         * @brief Výpočet smerodajnej odchýlky
         * 
         * @param list Zoznam double, z ktorého vypočítať smerodajnú odchýlku
         * @return Smerodajná odchýlka
         */
        public static double GetStandartDeviation(List<double> list)
        {
            int N = list.Count;
            double tmp = MatLib.subtract(GetSumSqr(list), MatLib.multiply(N, MatLib.square(GetArithmeticMean(list))));
            try
            {
                tmp = MatLib.divide(tmp, N - 1);
            }
            catch (DivideByZeroException e)
            {
                Console.WriteLine("{0}", e.Message);
                Console.WriteLine("Press any key to continue . . .");
                Console.Read();
                Environment.Exit(-1);
            }

            tmp = MatLib.sqrt(tmp);
            return tmp;
        }

        /**
         * @brief Vygeneruje náhodné čísla
         * 
         * @param range Koľko čísel vygenerovať
         * @return Zoznam čísiel double
         */
        static List<double> GetRandomNumbers(int range)
    {
            List<double> numbers = new List<double>();
            double max = 9999.0;
            Random r = new Random();
            for (int i = 0; i < range; i++)
            {
                numbers.Add(r.NextDouble() * max);
            }
            return numbers;
        }

        /**
         * @brief Načíta po riadkoch čísla zo vstupu do zoznamu
         * 
         * @return Zoznam čísiel double
         */
        static List<double> GetInput()
        {
            List<double> input = new List<double>();
            string line;
            while ((line = Console.ReadLine()) != null && line != "" )
            {
                try
                {
                    input.Add(double.Parse(line));
                }
                catch (FormatException e)
                {
                    Console.WriteLine("Nepodarilo sa pridať {0} medzi prvky.", line);
                }
            }

            return input;
        }

        static void Main(string[] args)
        {
            Console.WriteLine("Pogram na výpočet smerodajnej odchýlky");

            List<double> list2;
            bool profiling = true;

            if (profiling)
            {
                //list2 = GetRandomNumbers(10);
                //list2 = GetRandomNumbers(100);
                //list2 = GetRandomNumbers(1000);
                list2 = GetRandomNumbers(10000);
            }
            else
            {
                list2 = GetInput();
            }
            double res = GetStandartDeviation(list2);

            Console.WriteLine("Smerodajná odchýlka: {0}", res);

            Console.WriteLine("Press any key to continue . . .");
            Console.Read();
        }
    }
}
/*** Koniec súboru Program.cs ***/