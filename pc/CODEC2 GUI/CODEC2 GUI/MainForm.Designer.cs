namespace CODEC2_GUI
{
    partial class MainForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.lblNoDstar = new System.Windows.Forms.Label();
            this.sliceFlow = new System.Windows.Forms.FlowLayoutPanel();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.reloadDStarRepeaterListToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.selectAreaToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.reloadDStarInfoRelectorListToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.saveLogToFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.closeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.editToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.clearURListToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.logList = new System.Windows.Forms.ListBox();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.toolStripContainer1 = new System.Windows.Forms.ToolStripContainer();
            this.menuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.toolStripContainer1.ContentPanel.SuspendLayout();
            this.toolStripContainer1.TopToolStripPanel.SuspendLayout();
            this.toolStripContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // lblNoDstar
            // 
            this.lblNoDstar.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.lblNoDstar.AutoSize = true;
            this.lblNoDstar.Location = new System.Drawing.Point(236, 24);
            this.lblNoDstar.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.lblNoDstar.Name = "lblNoDstar";
            this.lblNoDstar.Size = new System.Drawing.Size(241, 21);
            this.lblNoDstar.TabIndex = 4;
            this.lblNoDstar.Text = "No DSTAR (DSTR) Slice";
            // 
            // sliceFlow
            // 
            this.sliceFlow.AutoScroll = true;
            this.sliceFlow.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.sliceFlow.Dock = System.Windows.Forms.DockStyle.Fill;
            this.sliceFlow.Location = new System.Drawing.Point(0, 0);
            this.sliceFlow.Name = "sliceFlow";
            this.sliceFlow.Size = new System.Drawing.Size(762, 200);
            this.sliceFlow.TabIndex = 1;
            this.sliceFlow.TabStop = true;
            // 
            // menuStrip1
            // 
            this.menuStrip1.Dock = System.Windows.Forms.DockStyle.None;
            this.menuStrip1.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem,
            this.editToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(762, 28);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.reloadDStarRepeaterListToolStripMenuItem,
            this.reloadDStarInfoRelectorListToolStripMenuItem,
            this.toolStripSeparator1,
            this.saveLogToFileToolStripMenuItem,
            this.toolStripSeparator2,
            this.closeToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(44, 24);
            this.fileToolStripMenuItem.Text = "&File";
            // 
            // reloadDStarRepeaterListToolStripMenuItem
            // 
            this.reloadDStarRepeaterListToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.selectAreaToolStripMenuItem});
            this.reloadDStarRepeaterListToolStripMenuItem.Name = "reloadDStarRepeaterListToolStripMenuItem";
            this.reloadDStarRepeaterListToolStripMenuItem.Size = new System.Drawing.Size(262, 26);
            this.reloadDStarRepeaterListToolStripMenuItem.Text = "Reload DStar Repeater List";
            this.reloadDStarRepeaterListToolStripMenuItem.DropDownOpening += new System.EventHandler(this.reloadDStarRepeaterListToolStripMenuItem_DropDownOpening);
            // 
            // selectAreaToolStripMenuItem
            // 
            this.selectAreaToolStripMenuItem.Enabled = false;
            this.selectAreaToolStripMenuItem.Name = "selectAreaToolStripMenuItem";
            this.selectAreaToolStripMenuItem.Size = new System.Drawing.Size(159, 26);
            this.selectAreaToolStripMenuItem.Text = "Select Area";
            // 
            // reloadDStarInfoRelectorListToolStripMenuItem
            // 
            this.reloadDStarInfoRelectorListToolStripMenuItem.Name = "reloadDStarInfoRelectorListToolStripMenuItem";
            this.reloadDStarInfoRelectorListToolStripMenuItem.Size = new System.Drawing.Size(262, 26);
            this.reloadDStarInfoRelectorListToolStripMenuItem.Text = "Reload DStar Relector List";
            this.reloadDStarInfoRelectorListToolStripMenuItem.Click += new System.EventHandler(this.reloadDStarInfoRelectorListToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(259, 6);
            // 
            // saveLogToFileToolStripMenuItem
            // 
            this.saveLogToFileToolStripMenuItem.Name = "saveLogToFileToolStripMenuItem";
            this.saveLogToFileToolStripMenuItem.Size = new System.Drawing.Size(262, 26);
            this.saveLogToFileToolStripMenuItem.Text = "&Save Log to File...";
            this.saveLogToFileToolStripMenuItem.Click += new System.EventHandler(this.saveLogToFileToolStripMenuItem_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(259, 6);
            // 
            // closeToolStripMenuItem
            // 
            this.closeToolStripMenuItem.Name = "closeToolStripMenuItem";
            this.closeToolStripMenuItem.Size = new System.Drawing.Size(262, 26);
            this.closeToolStripMenuItem.Text = "&Close";
            this.closeToolStripMenuItem.Click += new System.EventHandler(this.closeToolStripMenuItem_Click);
            // 
            // editToolStripMenuItem
            // 
            this.editToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.clearURListToolStripMenuItem});
            this.editToolStripMenuItem.Name = "editToolStripMenuItem";
            this.editToolStripMenuItem.Size = new System.Drawing.Size(47, 24);
            this.editToolStripMenuItem.Text = "&Edit";
            // 
            // clearURListToolStripMenuItem
            // 
            this.clearURListToolStripMenuItem.Name = "clearURListToolStripMenuItem";
            this.clearURListToolStripMenuItem.Size = new System.Drawing.Size(186, 26);
            this.clearURListToolStripMenuItem.Text = "Clear MRU Lists";
            this.clearURListToolStripMenuItem.Click += new System.EventHandler(this.clearURListToolStripMenuItem_Click);
            // 
            // logList
            // 
            this.logList.Dock = System.Windows.Forms.DockStyle.Fill;
            this.logList.FormattingEnabled = true;
            this.logList.IntegralHeight = false;
            this.logList.ItemHeight = 20;
            this.logList.Location = new System.Drawing.Point(0, 0);
            this.logList.Name = "logList";
            this.logList.ScrollAlwaysVisible = true;
            this.logList.Size = new System.Drawing.Size(762, 121);
            this.logList.TabIndex = 2;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.lblNoDstar);
            this.splitContainer1.Panel1.Controls.Add(this.sliceFlow);
            this.splitContainer1.Panel1MinSize = 200;
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.logList);
            this.splitContainer1.Panel2MinSize = 50;
            this.splitContainer1.Size = new System.Drawing.Size(762, 325);
            this.splitContainer1.SplitterDistance = 200;
            this.splitContainer1.TabIndex = 0;
            // 
            // toolStripContainer1
            // 
            // 
            // toolStripContainer1.ContentPanel
            // 
            this.toolStripContainer1.ContentPanel.Controls.Add(this.splitContainer1);
            this.toolStripContainer1.ContentPanel.Size = new System.Drawing.Size(762, 325);
            this.toolStripContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.toolStripContainer1.Location = new System.Drawing.Point(0, 0);
            this.toolStripContainer1.Name = "toolStripContainer1";
            this.toolStripContainer1.Size = new System.Drawing.Size(762, 353);
            this.toolStripContainer1.TabIndex = 1;
            this.toolStripContainer1.Text = "toolStripContainer1";
            // 
            // toolStripContainer1.TopToolStripPanel
            // 
            this.toolStripContainer1.TopToolStripPanel.Controls.Add(this.menuStrip1);
            // 
            // MainForm
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.ClientSize = new System.Drawing.Size(762, 353);
            this.Controls.Add(this.toolStripContainer1);
            this.Font = new System.Drawing.Font("Courier New", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            //this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Margin = new System.Windows.Forms.Padding(4);
            this.MinimumSize = new System.Drawing.Size(200, 200);
            this.Name = "MainForm";
            this.Text = "SmartSDR DSTAR Waveform";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel1.PerformLayout();
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.toolStripContainer1.ContentPanel.ResumeLayout(false);
            this.toolStripContainer1.TopToolStripPanel.ResumeLayout(false);
            this.toolStripContainer1.TopToolStripPanel.PerformLayout();
            this.toolStripContainer1.ResumeLayout(false);
            this.toolStripContainer1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label lblNoDstar;
        private System.Windows.Forms.FlowLayoutPanel sliceFlow;
        private System.Windows.Forms.ListBox logList;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem closeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem editToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem clearURListToolStripMenuItem;
        private System.Windows.Forms.ToolStripContainer toolStripContainer1;
        private System.Windows.Forms.ToolStripMenuItem reloadDStarInfoRelectorListToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem reloadDStarRepeaterListToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem saveLogToFileToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem selectAreaToolStripMenuItem;
    }
}

