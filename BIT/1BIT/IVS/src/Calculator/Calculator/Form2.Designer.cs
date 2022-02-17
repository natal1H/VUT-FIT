namespace Calculator
{
    partial class Form2
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Arial", 10.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(103, 22);
            this.label1.TabIndex = 0;
            this.label1.Text = "Nápoveda";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Arial", 10.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label2.Location = new System.Drawing.Point(12, 57);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(223, 22);
            this.label2.TabIndex = 1;
            this.label2.Text = "Používanie kalkulačky:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Arial", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label3.Location = new System.Drawing.Point(12, 89);
            this.label3.MaximumSize = new System.Drawing.Size(400, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(391, 38);
            this.label3.TabIndex = 2;
            this.label3.Text = "- pri binárnych operáciách zadajte prvé číslo, zvoľte operáciu, zadajte druhý ope" +
    "rand a stlačte =";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Arial", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label4.Location = new System.Drawing.Point(12, 140);
            this.label4.MaximumSize = new System.Drawing.Size(400, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(376, 38);
            this.label4.TabIndex = 3;
            this.label4.Text = "- pri unárnych operáciách zadajte prvý operand a stlačte požadovanú operáciu";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Arial", 10.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label5.Location = new System.Drawing.Point(12, 241);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(133, 22);
            this.label5.TabIndex = 4;
            this.label5.Text = "Obmedzenia:";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Arial", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label6.Location = new System.Drawing.Point(12, 275);
            this.label6.MaximumSize = new System.Drawing.Size(400, 0);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(390, 38);
            this.label6.TabIndex = 5;
            this.label6.Text = "- faktoriál: celé číslo, nesmie byť záporné, musí byť <= 17 ( z dôvodu zachovania" +
    " presnosti)";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("Arial", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label7.Location = new System.Drawing.Point(12, 328);
            this.label7.MaximumSize = new System.Drawing.Size(400, 0);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(400, 38);
            this.label7.TabIndex = 6;
            this.label7.Text = "- odmocniny: operandy nesmú byť záporné (výnimka pri nepárnej odmocnine)";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Font = new System.Drawing.Font("Arial", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label8.Location = new System.Drawing.Point(12, 190);
            this.label8.MaximumSize = new System.Drawing.Size(400, 0);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(364, 38);
            this.label8.TabIndex = 7;
            this.label8.Text = "- CE vymaže celú operáciu, C vymaže iba zatiaľ zadaný operand";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Font = new System.Drawing.Font("Arial", 10.2F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(238)));
            this.label9.Location = new System.Drawing.Point(13, 382);
            this.label9.MaximumSize = new System.Drawing.Size(400, 0);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(266, 19);
            this.label9.TabIndex = 8;
            this.label9.Text = "- operand log musí byť kladné číslo";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(149, 458);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(117, 17);
            this.label10.TabIndex = 9;
            this.label10.Text = "Team Baseballka";
            // 
            // Form2
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.WhiteSmoke;
            this.ClientSize = new System.Drawing.Size(427, 484);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Name = "Form2";
            this.Text = "Kalkulačka - nápoveda";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label10;
    }
}