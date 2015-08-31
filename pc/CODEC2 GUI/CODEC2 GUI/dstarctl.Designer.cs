namespace CODEC2_GUI
{
    partial class dstarctl
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.mycallsign = new System.Windows.Forms.Label();
            this.urcallsign = new System.Windows.Forms.Label();
            this.rpt1 = new System.Windows.Forms.Label();
            this.rpt2 = new System.Windows.Forms.Label();
            this.mytxt = new System.Windows.Forms.TextBox();
            this.mynotetxt = new System.Windows.Forms.TextBox();
            this.urtxt = new System.Windows.Forms.ComboBox();
            this.rpt1txt = new System.Windows.Forms.ComboBox();
            this.rpt2txt = new System.Windows.Forms.ComboBox();
            this.rbDV = new System.Windows.Forms.RadioButton();
            this.rbDR = new System.Windows.Forms.RadioButton();
            this.mynote = new System.Windows.Forms.Label();
            this.rpt1tip = new System.Windows.Forms.ToolTip(this.components);
            this.rpt2tip = new System.Windows.Forms.ToolTip(this.components);
            this.urtip = new System.Windows.Forms.ToolTip(this.components);
            this.mytip = new System.Windows.Forms.ToolTip(this.components);
            this.mynotetip = new System.Windows.Forms.ToolTip(this.components);
            this.dvtip = new System.Windows.Forms.ToolTip(this.components);
            this.drtip = new System.Windows.Forms.ToolTip(this.components);
            this.btnReset = new System.Windows.Forms.Button();
            this.btnRef = new System.Windows.Forms.Button();
            this.btnRpt = new System.Windows.Forms.Button();
            this.msgtxt = new System.Windows.Forms.ComboBox();
            this.lblMESSAGE = new System.Windows.Forms.Label();
            this.msgtip = new System.Windows.Forms.ToolTip(this.components);
            this.SuspendLayout();
            // 
            // mycallsign
            // 
            this.mycallsign.AutoSize = true;
            this.mycallsign.Location = new System.Drawing.Point(3, 64);
            this.mycallsign.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.mycallsign.Name = "mycallsign";
            this.mycallsign.Size = new System.Drawing.Size(43, 21);
            this.mycallsign.TabIndex = 2;
            this.mycallsign.Text = "MY:";
            // 
            // urcallsign
            // 
            this.urcallsign.AutoSize = true;
            this.urcallsign.Location = new System.Drawing.Point(3, 106);
            this.urcallsign.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.urcallsign.Name = "urcallsign";
            this.urcallsign.Size = new System.Drawing.Size(43, 21);
            this.urcallsign.TabIndex = 6;
            this.urcallsign.Text = "UR:";
            // 
            // rpt1
            // 
            this.rpt1.AutoSize = true;
            this.rpt1.Location = new System.Drawing.Point(316, 64);
            this.rpt1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.rpt1.Name = "rpt1";
            this.rpt1.Size = new System.Drawing.Size(65, 21);
            this.rpt1.TabIndex = 9;
            this.rpt1.Text = "RPT1:";
            // 
            // rpt2
            // 
            this.rpt2.AutoSize = true;
            this.rpt2.Location = new System.Drawing.Point(316, 110);
            this.rpt2.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.rpt2.Name = "rpt2";
            this.rpt2.Size = new System.Drawing.Size(65, 21);
            this.rpt2.TabIndex = 12;
            this.rpt2.Text = "RPT2:";
            // 
            // mytxt
            // 
            this.mytxt.Location = new System.Drawing.Point(53, 60);
            this.mytxt.Margin = new System.Windows.Forms.Padding(4);
            this.mytxt.MaxLength = 8;
            this.mytxt.Name = "mytxt";
            this.mytxt.Size = new System.Drawing.Size(110, 28);
            this.mytxt.TabIndex = 3;
            this.mytxt.TextChanged += new System.EventHandler(this.mytxt_TextChanged);
            // 
            // mynotetxt
            // 
            this.mynotetxt.Location = new System.Drawing.Point(200, 62);
            this.mynotetxt.Margin = new System.Windows.Forms.Padding(4);
            this.mynotetxt.MaxLength = 4;
            this.mynotetxt.Name = "mynotetxt";
            this.mynotetxt.Size = new System.Drawing.Size(77, 28);
            this.mynotetxt.TabIndex = 5;
            this.mynotetxt.TextChanged += new System.EventHandler(this.mynotetxt_TextChanged);
            // 
            // urtxt
            // 
            this.urtxt.Items.AddRange(new object[] {
            "CQCQCQ"});
            this.urtxt.Location = new System.Drawing.Point(53, 102);
            this.urtxt.Margin = new System.Windows.Forms.Padding(4);
            this.urtxt.MaxLength = 8;
            this.urtxt.Name = "urtxt";
            this.urtxt.Size = new System.Drawing.Size(152, 28);
            this.urtxt.TabIndex = 7;
            this.urtxt.SelectedIndexChanged += new System.EventHandler(this.urtxt_SelectedIndexChanged);
            this.urtxt.TextChanged += new System.EventHandler(this.urtxt_TextChanged);
            // 
            // rpt1txt
            // 
            this.rpt1txt.Location = new System.Drawing.Point(387, 60);
            this.rpt1txt.Margin = new System.Windows.Forms.Padding(4);
            this.rpt1txt.Name = "rpt1txt";
            this.rpt1txt.Size = new System.Drawing.Size(231, 28);
            this.rpt1txt.TabIndex = 10;
            this.rpt1txt.SelectedIndexChanged += new System.EventHandler(this.rpt1txt_SelectedIndexChanged);
            this.rpt1txt.TextChanged += new System.EventHandler(this.rpt1txt_TextChanged);
            // 
            // rpt2txt
            // 
            this.rpt2txt.Location = new System.Drawing.Point(387, 106);
            this.rpt2txt.Margin = new System.Windows.Forms.Padding(4);
            this.rpt2txt.MaxLength = 8;
            this.rpt2txt.Name = "rpt2txt";
            this.rpt2txt.Size = new System.Drawing.Size(231, 28);
            this.rpt2txt.TabIndex = 13;
            this.rpt2txt.SelectedIndexChanged += new System.EventHandler(this.rpt2txt_SelectedIndexChanged);
            this.rpt2txt.TextChanged += new System.EventHandler(this.rpt2txt_TextChanged);
            // 
            // rbDV
            // 
            this.rbDV.AutoSize = true;
            this.rbDV.Location = new System.Drawing.Point(192, 9);
            this.rbDV.Margin = new System.Windows.Forms.Padding(4);
            this.rbDV.Name = "rbDV";
            this.rbDV.Size = new System.Drawing.Size(53, 25);
            this.rbDV.TabIndex = 0;
            this.rbDV.TabStop = true;
            this.rbDV.Text = "DV";
            this.rbDV.UseVisualStyleBackColor = true;
            this.rbDV.Click += new System.EventHandler(this.rbDV_Click);
            // 
            // rbDR
            // 
            this.rbDR.AutoSize = true;
            this.rbDR.Location = new System.Drawing.Point(267, 9);
            this.rbDR.Margin = new System.Windows.Forms.Padding(4);
            this.rbDR.Name = "rbDR";
            this.rbDR.Size = new System.Drawing.Size(53, 25);
            this.rbDR.TabIndex = 1;
            this.rbDR.TabStop = true;
            this.rbDR.Text = "DR";
            this.rbDR.UseVisualStyleBackColor = true;
            this.rbDR.Click += new System.EventHandler(this.rbDR_Click);
            // 
            // mynote
            // 
            this.mynote.AutoSize = true;
            this.mynote.Location = new System.Drawing.Point(173, 62);
            this.mynote.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.mynote.Name = "mynote";
            this.mynote.Size = new System.Drawing.Size(21, 21);
            this.mynote.TabIndex = 4;
            this.mynote.Text = "/";
            // 
            // btnReset
            // 
            this.btnReset.Location = new System.Drawing.Point(351, 7);
            this.btnReset.Margin = new System.Windows.Forms.Padding(4);
            this.btnReset.Name = "btnReset";
            this.btnReset.Size = new System.Drawing.Size(77, 29);
            this.btnReset.TabIndex = 14;
            this.btnReset.Text = "Reset";
            this.btnReset.UseVisualStyleBackColor = true;
            this.btnReset.Visible = false;
            this.btnReset.Click += new System.EventHandler(this.btnReset_Click);
            // 
            // btnRef
            // 
            this.btnRef.Location = new System.Drawing.Point(228, 102);
            this.btnRef.Name = "btnRef";
            this.btnRef.Size = new System.Drawing.Size(61, 31);
            this.btnRef.TabIndex = 8;
            this.btnRef.Text = "Ref";
            this.btnRef.UseVisualStyleBackColor = true;
            this.btnRef.Click += new System.EventHandler(this.btnRef_Click);
            // 
            // btnRpt
            // 
            this.btnRpt.Location = new System.Drawing.Point(638, 58);
            this.btnRpt.Name = "btnRpt";
            this.btnRpt.Size = new System.Drawing.Size(61, 33);
            this.btnRpt.TabIndex = 11;
            this.btnRpt.Text = "Rpt";
            this.btnRpt.UseVisualStyleBackColor = true;
            this.btnRpt.Click += new System.EventHandler(this.btnRpt_Click);
            // 
            // msgtxt
            // 
            this.msgtxt.FormattingEnabled = true;
            this.msgtxt.Location = new System.Drawing.Point(156, 157);
            this.msgtxt.MaxLength = 20;
            this.msgtxt.Name = "msgtxt";
            this.msgtxt.Size = new System.Drawing.Size(462, 28);
            this.msgtxt.TabIndex = 15;
            this.msgtxt.SelectedIndexChanged += new System.EventHandler(this.msgtxt_SelectedIndexChanged);
            this.msgtxt.TextChanged += new System.EventHandler(this.msgtxt_TextChanged);
            // 
            // lblMESSAGE
            // 
            this.lblMESSAGE.AutoSize = true;
            this.lblMESSAGE.Location = new System.Drawing.Point(34, 157);
            this.lblMESSAGE.Name = "lblMESSAGE";
            this.lblMESSAGE.Size = new System.Drawing.Size(98, 21);
            this.lblMESSAGE.TabIndex = 16;
            this.lblMESSAGE.Text = "Message:";
            // 
            // dstarctl
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.Controls.Add(this.lblMESSAGE);
            this.Controls.Add(this.msgtxt);
            this.Controls.Add(this.btnRpt);
            this.Controls.Add(this.btnRef);
            this.Controls.Add(this.btnReset);
            this.Controls.Add(this.rbDR);
            this.Controls.Add(this.rbDV);
            this.Controls.Add(this.rpt2txt);
            this.Controls.Add(this.rpt1txt);
            this.Controls.Add(this.urtxt);
            this.Controls.Add(this.mynotetxt);
            this.Controls.Add(this.mytxt);
            this.Controls.Add(this.rpt2);
            this.Controls.Add(this.rpt1);
            this.Controls.Add(this.urcallsign);
            this.Controls.Add(this.mynote);
            this.Controls.Add(this.mycallsign);
            this.Font = new System.Drawing.Font("Courier New", 11F);
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "dstarctl";
            this.Size = new System.Drawing.Size(730, 198);
            this.Load += new System.EventHandler(this.dstarctl_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label mycallsign;
        private System.Windows.Forms.Label urcallsign;
        private System.Windows.Forms.Label rpt1;
        private System.Windows.Forms.Label rpt2;
        private System.Windows.Forms.TextBox mytxt;
        private System.Windows.Forms.TextBox mynotetxt;
        private System.Windows.Forms.ComboBox urtxt;
        private System.Windows.Forms.ComboBox rpt1txt;
        private System.Windows.Forms.ComboBox rpt2txt;
        private System.Windows.Forms.RadioButton rbDV;
        private System.Windows.Forms.RadioButton rbDR;
        private System.Windows.Forms.Label mynote;
        private System.Windows.Forms.ToolTip rpt1tip;
        private System.Windows.Forms.ToolTip rpt2tip;
        private System.Windows.Forms.ToolTip urtip;
        private System.Windows.Forms.ToolTip mytip;
        private System.Windows.Forms.ToolTip mynotetip;
        private System.Windows.Forms.ToolTip dvtip;
        private System.Windows.Forms.ToolTip drtip;
        private System.Windows.Forms.Button btnReset;
        private System.Windows.Forms.Button btnRef;
        private System.Windows.Forms.Button btnRpt;
        private System.Windows.Forms.ComboBox msgtxt;
        private System.Windows.Forms.Label lblMESSAGE;
        private System.Windows.Forms.ToolTip msgtip;
    }
}
