/**************************************************
 * Názov projektu: Kalkulačka - mockup
 * Súbor: Form4.cs
 * Dátum: 23.4.2018
 * Posledná zmena: 23.4.2018
 * Autor: Natália Holková xholko02(at)fit.vutbr.cz
 * 
 * Popis: Mockup kalkulačky
 * 
 ***************************************************
/**
 * @file Form4.cs
 * 
 * @brief Okno štatistického módu kalkulačky
 * @author Natália Holková (xholko02)
 * 
 */

using System;
using System.Windows.Forms;

namespace CalculatorMockup
{
    public partial class Form4 : Form
    {
        /**
         * @brief Konštruktor Form4
         */
        public Form4()
        {
            InitializeComponent();
            this.FormClosing += Form4_FormClosing;
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
            this.FormClosing -= Form4_FormClosing;
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
            this.FormClosing -= Form4_FormClosing;
            this.Close();
        }

        /**
         * @brief Prepnutie na grafický mód
         * 
         * @param sender
         * @param e
         */
        private void grafickýToolStripMenuItem_Click(object sender, EventArgs e)
        {
            (new Form3()).Show();
            this.FormClosing -= Form4_FormClosing;
            this.Close();
        }

        /**
         * @brief Ukončenie okna
         * 
         * @param sender
         * @param e
         */
        private void Form4_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (e.CloseReason == CloseReason.UserClosing)
            {
                Application.Exit();
            }
        }
    }
}
/*** Koniec Form4.cs ***/
