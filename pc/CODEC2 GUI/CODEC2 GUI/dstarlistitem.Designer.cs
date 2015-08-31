namespace CODEC2_GUI
{
    partial class dstarlistitem
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(dstarlistitem));
            this.btnCommit = new System.Windows.Forms.Button();
            this.lblName = new System.Windows.Forms.Label();
            this.lblMode = new System.Windows.Forms.Label();
            this.dstarctl1 = new CODEC2_GUI.dstarctl();
            this.SuspendLayout();
            // 
            // btnCommit
            // 
            this.btnCommit.Location = new System.Drawing.Point(565, 5);
            this.btnCommit.Margin = new System.Windows.Forms.Padding(4);
            this.btnCommit.Name = "btnCommit";
            this.btnCommit.Size = new System.Drawing.Size(103, 29);
            this.btnCommit.TabIndex = 1;
            this.btnCommit.Text = "Commit";
            this.btnCommit.UseVisualStyleBackColor = true;
            this.btnCommit.Click += new System.EventHandler(this.btnCommit_Click);
            // 
            // lblName
            // 
            this.lblName.AutoSize = true;
            this.lblName.Location = new System.Drawing.Point(6, 9);
            this.lblName.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.lblName.Name = "lblName";
            this.lblName.Size = new System.Drawing.Size(65, 21);
            this.lblName.TabIndex = 2;
            this.lblName.Text = "Slice";
            // 
            // lblMode
            // 
            this.lblMode.AutoSize = true;
            this.lblMode.Location = new System.Drawing.Point(98, 9);
            this.lblMode.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.lblMode.Name = "lblMode";
            this.lblMode.Size = new System.Drawing.Size(54, 21);
            this.lblMode.TabIndex = 3;
            this.lblMode.Text = "Mode";
            // 
            // dstarctl1
            // 
            this.dstarctl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.dstarctl1.DRMode = false;
            this.dstarctl1.Font = new System.Drawing.Font("Courier New", 11F);
            this.dstarctl1.Location = new System.Drawing.Point(-1, -1);
            this.dstarctl1.Margin = new System.Windows.Forms.Padding(4);
            this.dstarctl1.MESSAGE = "";
            this.dstarctl1.Modified = CODEC2_GUI.dstarctl.ModifyFlags.NOFLAGS;
            this.dstarctl1.MY = "";
            this.dstarctl1.Name = "dstarctl1";
            this.dstarctl1.NOTE = "";
            this.dstarctl1.RPT1 = "";
            this.dstarctl1.RPT1List = ((System.Collections.Generic.List<string>)(resources.GetObject("dstarctl1.RPT1List")));
            this.dstarctl1.RPT2 = "";
            this.dstarctl1.RPT2List = ((System.Collections.Generic.List<string>)(resources.GetObject("dstarctl1.RPT2List")));
            this.dstarctl1.Size = new System.Drawing.Size(731, 208);
            this.dstarctl1.TabIndex = 0;
            this.dstarctl1.UR = "";
            this.dstarctl1.URList = ((System.Collections.Generic.List<string>)(resources.GetObject("dstarctl1.URList")));
            // 
            // dstarlistitem
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.Controls.Add(this.lblMode);
            this.Controls.Add(this.lblName);
            this.Controls.Add(this.btnCommit);
            this.Controls.Add(this.dstarctl1);
            this.Font = new System.Drawing.Font("Courier New", 10.8F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Margin = new System.Windows.Forms.Padding(4);
            this.Name = "dstarlistitem";
            this.Size = new System.Drawing.Size(734, 206);
            this.Load += new System.EventHandler(this.dstarlistitem_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private dstarctl dstarctl1;
        private System.Windows.Forms.Button btnCommit;
        private System.Windows.Forms.Label lblName;
        private System.Windows.Forms.Label lblMode;
    }
}
