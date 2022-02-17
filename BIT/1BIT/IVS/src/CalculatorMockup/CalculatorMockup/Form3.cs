/**************************************************
 * Názov projektu: Kalkulačka - mockup
 * Súbor: Form3.cs
 * Dátum: 23.4.2018
 * Posledná zmena: 23.4.2018
 * Autor: Natália Holková xholko02(at)fit.vutbr.cz
 * 
 * Popis: Mockup kalkulačky
 * 
 ***************************************************
/**
 * @file Form3.cs
 * 
 * @brief Okna grafického módu kalkulačky
 * @author Natália Holková (xholko02)
 * 
 */

using System;
using System.Windows.Forms;

namespace CalculatorMockup
{
    public partial class Form3 : Form
    {
        /**
         * @brief Konštruktor Form3
         */
        public Form3()
        {
            InitializeComponent();
            this.FormClosing += Form3_FormClosing;
        }

        /**
         * @brief Prepnutie na štandartný mód
         * 
         * @param sender
         * @param e
         */
        private void standartToolStripMenuItem_Click(object sender, EventArgs e)
        {
            (new Form1()).Show();
            this.FormClosing -= Form3_FormClosing;
            this.Close();
        }

        /**
         * @brief Prepnutie na vedecký mód
         * 
         * @param sender
         * @param e
         */
        private void scientificToolStripMenuItem_Click(object sender, EventArgs e)
        {
            (new Form2()).Show();
            this.FormClosing -= Form3_FormClosing;
            this.Close();
        }

        /**
         * @brief Prepnutie na štatistický mód
         * 
         * @param sender
         * @param e
         */
        private void štatistickýToolStripMenuItem_Click(object sender, EventArgs e)
        {
            (new Form4()).Show();
            this.FormClosing -= Form3_FormClosing;
            this.Close();
        }

        /**
         * @brief Ukončenie okna
         * 
         * @param sender
         * @param e
         */
        private void Form3_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (e.CloseReason == CloseReason.UserClosing)
            {
                Application.Exit();
            }
        }
    }
}
/*** Koniec Form3.cs ***/
