/*******************************************************************************
 * MainForm.cs									  			
 *
 * 	An example GUI showing how to interface to the FlexRadio Modem API
 *
 *  Created on: 2014-08-28
 *      Author: Eric / KE5DTO
 *
 *      Enhancements: Mark Hanson / AA3RK / MKCM Software, LLC.
 *
 *******************************************************************************
 *
 *	Copyright (C) 2014 FlexRadio Systems.
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *	GNU General Public License for more details.
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
 *	
 *  Contact: gpl<AT>flexradio<DOT>com or 
 *  
 *  GPL C/O FlexRadio Systems
 *  4616 W. Howard Lane
 *  Suite 1-150
 *  Austin, TX USA 78728
 *
 ******************************************************************************/

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using Flex.Smoothlake.FlexLib;
using System.IO;

namespace CODEC2_GUI
{
    public partial class MainForm : Form
    {
        private bool _radioConnected = false;
        private Radio _radio = null;
        public BindingList<string> LogQueue { get; set; }

        public MainForm()
        {
            InitializeComponent();



        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (this.WindowState == FormWindowState.Normal)
                Properties.Settings.Default.MainPosition =
                    string.Format("{0},{1},{2},{3},{4}", 
                        this.Left, this.Top, this.Width, this.Height,
                        splitContainer1.SplitterDistance);

            Properties.Settings.Default.Save();
        }


        private void Form1_Load(object sender, EventArgs e)
        {
            if (!string.IsNullOrEmpty(Properties.Settings.Default.MainPosition))
            {
                string[] cords = Properties.Settings.Default.MainPosition.Split(',');
                if (cords.Length == 5)
                {
                    this.Left = Convert.ToInt32(cords[0]);
                    this.Top = Convert.ToInt32(cords[1]);
                    this.Width = Convert.ToInt32(cords[2]);
                    this.Height = Convert.ToInt32(cords[3]);
                    splitContainer1.SplitterDistance = Convert.ToInt32(cords[4]);
                }
            }

            API.RadioAdded += new API.RadioAddedEventHandler(API_RadioAdded);
            API.RadioRemoved += new API.RadioRemovedEventHandler(API_RadioRemoved);

            API.ProgramName = "ThumbDV_DSTAR";
            API.Init();


            LogQueue = new BindingList<string>();
            logList.DataSource = LogQueue;

        }


        //*************************************
        //         API Event Handlers
        //*************************************

        void API_RadioAdded(Radio radio)
        {
            // if we have already connected to another radio, ignore this one
            if(_radioConnected) return;

            // set the flag so that we don't try to connect to other radios
            _radioConnected = true;

            // save a reference to the radio
            _radio = radio;

            // attach Slice event handlers
            _radio.SliceAdded += new Radio.SliceAddedEventHandler(_radio_SliceAdded);
            _radio.SliceRemoved += new Radio.SliceRemovedEventHandler(_radio_SliceRemoved);

            // connect to the radio
            _radio.Connect();
        }

        void API_RadioRemoved(Radio radio)
        {
            _radio.SliceAdded -= _radio_SliceAdded;
            _radio.SliceRemoved -= _radio_SliceRemoved;

            _radio.Disconnect();

            _radio = null;
            _radioConnected = false;
        }

        //*************************************
        //         Radio Event Handlers
        //*************************************

        void _radio_SliceAdded(Slice slc)
        {
            if (InvokeRequired)
            {
                Invoke(new MethodInvoker(delegate
                {
                    _radio_SliceAdded(slc);
                }));
                return;
            }

            // get existing DStar list items
            List<dstarlistitem> lst = new List<dstarlistitem>(sliceFlow.Controls.Cast<dstarlistitem>());
            sliceFlow.Controls.Clear();
            // create new item
            dstarlistitem dli = new dstarlistitem();
            dli.slice = slc;
            dli.LogEvent += Dli_LogEvent;
            lst.Add(dli);
            // sort items by slice index
            lst.Sort((dstarlistitem x, dstarlistitem y) => { return x.slice.Index - y.slice.Index; });
            // update flow control
            sliceFlow.Controls.AddRange(lst.ToArray());
            // set no slice indicator
            lblNoDstar.Visible = sliceFlow.Controls.Count == 0;
        }

        private void Dli_LogEvent(object sender, LogEventArgs e)
        {
            // current selection
            int sel = logList.SelectedIndex;
            int maxlogsentry = Properties.Settings.Default.LogEntryMax;
            while (LogQueue.Count > maxlogsentry)
            {
                LogQueue.RemoveAt(maxlogsentry);
            }
            if (LogQueue.Count == 0)
                LogQueue.Add(e.LogEntry);
            else 
                LogQueue.Insert(0, e.LogEntry);
            // maintain top selected item if was selected or no selection
            if (logList.SelectedIndex == -1 || sel == 0)
                logList.SelectedIndex = 0;
        }

        void _radio_SliceRemoved(Slice slc)
        {
            if (InvokeRequired)
            {
                Invoke(new MethodInvoker(delegate
                {
                    _radio_SliceRemoved(slc);
                }));
                return;
            }

            foreach (dstarlistitem dli in sliceFlow.Controls)
            {
                if (dli.slice == slc)
                {
                    dli.LogEvent -= Dli_LogEvent;
                    dli.slice = null;
                    sliceFlow.Controls.Remove(dli);
                    break;
                }
            }
            // set no slice indicator
            lblNoDstar.Visible = sliceFlow.Controls.Count == 0;
        }


        private void closeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void clearURListToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Properties.Settings.Default.URList = string.Empty;
            Properties.Settings.Default.RPT1List = string.Empty;
            Properties.Settings.Default.RPT2List = string.Empty;
            Properties.Settings.Default.Save();

            foreach(dstarlistitem dli in sliceFlow.Controls)
            {
                dli.ClearMYList();
                dli.ClearRPT1List();
                dli.ClearRPT2List();
                dli.ClearMESSAGEList();
            }

        }


        private void reloadDStarInfoRelectorListToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                DstarInfo di = new DstarInfo();
                di.FetchAndSaveReflectorList();
                SelectForm sf = new SelectForm();
                sf.showOnly = true;
                sf.Mode = SelectForm.RMode.Reflector;
                sf.Owner = this;
                sf.Show();
            }
            catch(Exception ex)
            {
                StringBuilder sb = new StringBuilder();
                Exception ex1 = ex;
                while(ex1 != null)
                {
                    sb.AppendLine(ex1.Message);
                    ex1 = ex1.InnerException;
                }
                MessageBox.Show(sb.ToString(), "Load DSTARINFO Error!", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            }
        }

        private void reloadDStarRepeaterListToolStripMenuItemList(string area)
        {
            try
            {
                DstarInfo di = new DstarInfo();
                di.FetchAndSaveRepeaterList(area);
                SelectForm sf = new SelectForm();
                sf.showOnly = true;
                sf.repeaterOnly = true;
                sf.Mode = SelectForm.RMode.Repeater;
                sf.Owner = this;
                sf.Show();
            }
            catch (Exception ex)
            {
                StringBuilder sb = new StringBuilder();
                Exception ex1 = ex;
                while (ex1 != null)
                {
                    sb.AppendLine(ex1.Message);
                    ex1 = ex1.InnerException;
                }
                MessageBox.Show(sb.ToString(), "Load DSTARINFO Error!", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            }
        }

        private void saveLogToFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (LogQueue.Count() == 0)
            {
                MessageBox.Show(this, "There are no log entries!", "Save Log Warning", MessageBoxButtons.OK, MessageBoxIcon.Warning);
                return;
            }
            SaveFileDialog sfd = new SaveFileDialog();
            sfd.OverwritePrompt = true;
            sfd.DefaultExt = ".TXT";
            sfd.Filter = "Text (*.TXT)|.TXT\0";
            sfd.Title = "Save Log File";
            sfd.InitialDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
            if (sfd.ShowDialog() == DialogResult.OK)
            {
                using (StreamWriter sw = File.CreateText(sfd.FileName))
                {
                    foreach (string str in LogQueue)
                    {
                        sw.WriteLine(str);
                    }
                    sw.Close();
                }
            }
        }

        private void reloadDStarRepeaterListToolStripMenuItem_DropDownOpening(object sender, EventArgs e)
        {
            for (int idx = reloadDStarRepeaterListToolStripMenuItem.DropDownItems.Count - 1; idx > 0; idx--)
            {
                reloadDStarRepeaterListToolStripMenuItem.DropDownItems.RemoveAt(idx);
            }
            try
            {
                DstarInfo di = new DstarInfo();
                List<string> areas = di.GetDStarRepeaterAreas();
                if (areas != null && areas.Count > 0)
                {
                    foreach (string area in areas)
                    {
                        ToolStripMenuItem ti = new ToolStripMenuItem(area);
                        ti.Click += RepeaterAreaSelection_Click;
                        reloadDStarRepeaterListToolStripMenuItem.DropDownItems.Add(ti);
                    }
                }
            }
            catch (Exception ex)
            {
                StringBuilder sb = new StringBuilder();
                Exception ex1 = ex;
                while (ex1 != null)
                {
                    sb.AppendLine(ex1.Message);
                    ex1 = ex1.InnerException;
                }
                MessageBox.Show(sb.ToString(), "Load DSTARINFO Repeater Area Info Error!", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            }
        }

        private void RepeaterAreaSelection_Click(object sender, EventArgs e)
        {
            ToolStripMenuItem ti = sender as ToolStripMenuItem;
            reloadDStarRepeaterListToolStripMenuItemList(ti.Text);
        }
    }



    public class VerticalProgressBar : ProgressBar
    {
        protected override CreateParams CreateParams
        {
            get
            {
                CreateParams cp = base.CreateParams;
                cp.Style |= 0x04;
                return cp;
            }
        }
    }
}
