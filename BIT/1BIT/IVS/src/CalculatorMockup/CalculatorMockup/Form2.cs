/**************************************************
 * Názov projektu: Kalkulačka - mockup
 * Súbor: Form2.cs
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
 * @brief Okno vedeckého módu kalkulačky
 * @author Natália Holková (xholko02)
 * 
 */

using System;
using System.Windows.Forms;

namespace CalculatorMockup
{
    public partial class Form2 : Form
    {
        private bool shift_pressed;

        /**
         * @brief Konštruktor Form2
         */
        public Form2()
        {
            InitializeComponent();
            this.FormClosing += Form2_FormClosing;
            button33.Hide();
            button34.Hide();
            button35.Hide();
            button36.Hide();
            shift_pressed = false;
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
            this.FormClosing -= Form2_FormClosing;
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
            this.FormClosing -= Form2_FormClosing;
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
            this.FormClosing -= Form2_FormClosing;
            this.Close();
        }

        /**
         * @brief Ukončenie okna
         * 
         * @param sender
         * @param e
         */
        private void Form2_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (e.CloseReason == CloseReason.UserClosing)
            {
                Application.Exit();
            }
        }

        /**
         * @brief Prepnutie tlačítok pomocou shift
         * 
         * @param sender
         * @param e
         */
        private void button31_Click(object sender, EventArgs e)
        {
            if (shift_pressed == true)
            {
                button27.Show();
                button28.Show();
                button29.Show();
                button30.Show();

                button33.Hide();
                button34.Hide();
                button35.Hide();
                button36.Hide();
            }
            else
            {
                button27.Hide();
                button28.Hide();
                button29.Hide();
                button30.Hide();

                button33.Show();
                button34.Show();
                button35.Show();
                button36.Show();
            }
            shift_pressed = !shift_pressed;
        }
    }
}
