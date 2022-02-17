/**************************************************
 * Názov projektu: Kalkulačka - mockup
 * Súbor: Program.cs
 * Dátum: 23.4.2018
 * Posledná zmena: 23.4.2018
 * Autor: Natália Holková xholko02(at)fit.vutbr.cz
 * 
 * Popis: Mockup kalkulačky
 * 
 ***************************************************
/**
 * @file Program.cs
 * 
 * @brief Mockup - spustenie
 * @author Natália Holková (xholko02)
 * 
 */

using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace CalculatorMockup
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            //Application.EnableVisualStyles();
            //Application.SetCompatibleTextRenderingDefault(false);
            //Application.Run(new Form1());

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            (new Form1()).Show();
            Application.Run();
        }
    }
}
