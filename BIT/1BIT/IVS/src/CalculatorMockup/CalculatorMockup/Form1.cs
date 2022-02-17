/**************************************************
 * Názov projektu: Kalkulačka - mockup
 * Súbor: Form1.cs
 * Dátum: 23.4.2018
 * Posledná zmena: 23.4.2018
 * Autor: Natália Holková xholko02(at)fit.vutbr.cz
 * 
 * Popis: Mockup kalkulačky
 * 
 ***************************************************
/**
 * @file Form1.cs
 * 
 * @brief Okno štandartného módu kalkulačky
 * @author Natália Holková (xholko02)
 * 
 */

using System;
using System.Windows.Forms;

namespace CalculatorMockup
{
    public partial class Form1 : Form
    {
        /**
         * @brief Konštruktor Form1
         */
        public Form1()
        {
            InitializeComponent();
            this.FormClosing += Form1_FormClosing;
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
            this.FormClosing -= Form1_FormClosing;
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
            this.FormClosing -= Form1_FormClosing;
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
            this.FormClosing -= Form1_FormClosing;
            this.Close();
        }

        /**
         * @brief Ukončenie okna
         * 
         * @param sender
         * @param e
         */
        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (e.CloseReason == CloseReason.UserClosing)
            {
                Application.Exit();
            }
        }
    }
}
/*** Koniec súboru Form1.cs ***/
