/*******************************************************************************
 * DstarListItem.cs									  			
 *
 * 	DStar Flow List Item control 
 *
 *  Created on: 2015-08-22
 *      Author: Mark Hanson / AA3RK / MKCM Software, LLC.
 *
 *
 *******************************************************************************
 *
 *	Copyright (C) 2015 FlexRadio Systems.
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
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Flex.Smoothlake.FlexLib;

namespace CODEC2_GUI
{
    public partial class dstarlistitem : UserControl
    {
        public event EventHandler<LogEventArgs> LogEvent;

        private Slice _slice;
        public Slice slice
        {
            get { return _slice; }
            set
            {
                if (_slice == value)
                    return;

                if (_slice != null && value == null)
                {
                    _slice.PropertyChanged -= slc_PropertyChanged;
                    _slice.WaveformStatusReceived -= slc_WaveformStatusReceived;
                    this.Enabled = false;
                    this.Visible = false;
                    return;
                }
                _slice = value;
                _slice.PropertyChanged += new PropertyChangedEventHandler(slc_PropertyChanged);
                _slice.WaveformStatusReceived += new Slice.WaveformStatusReceivedEventHandler(slc_WaveformStatusReceived);
                lblName.Text = "Slice " + Convert.ToChar(0x41 + _slice.Index).ToString();
                this.Enabled = true;
                this.Visible = true;
                slc_PropertyChanged(_slice, new PropertyChangedEventArgs("DemodMode"));
            }
        }

        //*************************************
        //         Slice Event Handlers
        //*************************************

        void slc_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (InvokeRequired)
            {
                Invoke(new MethodInvoker(delegate
                {
                    slc_PropertyChanged(sender, e.PropertyName);
                }));
            }
            else
            {
                slc_PropertyChanged(sender, e.PropertyName);
            }
        }

        void slc_PropertyChanged(object sender, string property_name)
        {
            Slice slc = sender as Slice;
            if (slc == null) return;

            switch (property_name)
            {
                case "DemodMode":
                    // Is the mode set to FreeDV?
                    CheckDemodMode(slc);
                    break;
            }
        }

        private void CheckDemodMode(Slice slc)
        {
            if (slc.DemodMode == "DSTR" || slc.DemodMode == "D*FM")
            {
                this.Enabled = true;
                dstarctl1.Visible = true;
                slc.SendWaveformCommand("status");
            }
            else
            {
                this.Enabled = false;
                dstarctl1.Visible = false;
            }
            lblMode.Text = slc.DemodMode;
        }


        void slc_WaveformStatusReceived(Slice slc, string status)
        {
            if (InvokeRequired)
            {
                Invoke(new MethodInvoker(delegate
                {
                    slc_WaveformStatusReceived(slc, status);
                }));
                return;
            }

            string logMY = null;
            string logNote = null;
            string logUR = null;
            string logRPT1 = null;
            string logRPT2 = null;
            bool setDRMode = false;

            string[] words = status.Split(' ');

            foreach (string kv in words)
            {
                string[] tokens = kv.Split('=');

                if (tokens.Length != 2)
                {
                    continue;
                }

                string key = tokens[0];
                string value = tokens[1];

                value = value.Replace('\u007f', ' ').Trim();
                switch (key.ToLower())
                {
                    case "destination_rptr_tx":
                        dstarctl1.RPT2 = value == "DIRECT" ? string.Empty : value; 
                        setDRMode = true;
                        break;
                    case "departure_rptr_tx":
                        dstarctl1.RPT1 = value == "DIRECT" ? string.Empty : value;
                        setDRMode = true;
                        break;
                    case "companion_call_tx":
                        dstarctl1.UR = value;
                        break;
                    case "own_call1_tx":
                        if (string.IsNullOrEmpty(value) || value == "CALLSIGN")
                        {
                            if (!string.IsNullOrEmpty(Properties.Settings.Default.MYCallSign))
                                dstarctl1.MY = Properties.Settings.Default.MYCallSign;
                            else
                                dstarctl1.MY = string.Empty;
                        }
                        else
                            dstarctl1.MY = value;
                        break;
                    case "own_call2_tx":
                        dstarctl1.NOTE = value;
                        break;

                    case "destination_rptr_rx":
                        logRPT2 = value == "DIRECT" ? string.Empty : value;
                        break;
                    case "departure_rptr_rx":
                        logRPT1 = value == "DIRECT" ? string.Empty : value;
                        break;
                    case "companion_call_rx":
                        logUR = value;
                        break;
                    case "own_call1_rx":
                        logMY = value;
                        break;
                    case "own_call2_rx":
                        logNote = value;
                        break;
                }
            }

            if (setDRMode)
            {
                dstarctl1.DRMode = string.IsNullOrEmpty(dstarctl1.RPT1) ? false : true;
            }

            if (!string.IsNullOrEmpty(logMY) && !string.IsNullOrEmpty(logUR))
            {
                if (LogEvent != null)
                    LogEvent(this, new LogEventArgs(string.Format("{0} UR: {1,-8} MY: {2,-13}{3}",
                    DateTime.UtcNow.ToString("yyyy-MM-dd HH:mm:ss"),
                    logUR,
                    logMY + (string.IsNullOrEmpty(logNote) ? string.Empty : "/" + logNote),
                    string.IsNullOrEmpty(logRPT1) ? string.Empty :
                        string.Format(" RPT1: {0,-8} RPT2: {1,-8}", logRPT1, logRPT2))));
            }
            else
            {
                System.Diagnostics.Debug.WriteLine("log information not set");
            }

            btnCommit.Enabled = dstarctl1.Modified != dstarctl.ModifyFlags.NOFLAGS;
        }



        public dstarlistitem()
        {
            InitializeComponent();
        }

        private void dstarlistitem_Load(object sender, EventArgs e)
        {
            dstarctl1.PropertyChanged += Dstarctl1_PropertyChanged;

            if (!string.IsNullOrEmpty(Properties.Settings.Default.URList))
            {
                dstarctl1.URList = Properties.Settings.Default.URList.Split('|').ToList();
                dstarctl1.RPT1List = Properties.Settings.Default.RPT1List.Split('|').ToList();
                dstarctl1.RPT2List = Properties.Settings.Default.RPT2List.Split('|').ToList();
            }

        }

        private void Dstarctl1_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "Modified")
            {
                btnCommit.Enabled = dstarctl1.Modified != dstarctl.ModifyFlags.NOFLAGS;
            }
        }

        private void btnCommit_Click(object sender, EventArgs e)
        {
            if (dstarctl1.DRMode && string.IsNullOrEmpty(dstarctl1.RPT1))
            {
                MessageBox.Show(this, "DR Mode requires RPT1 value!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return;
            }
            string cmd;
            string rpt2 = string.IsNullOrEmpty(dstarctl1.RPT2) ? "DIRECT" : dstarctl1.RPT2;
            cmd = "set destination_rptr=" + rpt2.Replace(" ", "\u007f");
            _slice.SendWaveformCommand(cmd);
            string rpt1 = string.IsNullOrEmpty(dstarctl1.RPT1) ? "DIRECT" : dstarctl1.RPT1;
            cmd = "set departure_rptr=" + rpt1.Replace(" ", "\u007f");
            _slice.SendWaveformCommand(cmd);
            string ur = dstarctl1.UR;
            cmd = "set companion_call=" + ur.Replace(" ", "\u007f");
            _slice.SendWaveformCommand(cmd);
            string my = dstarctl1.MY;
            cmd = "set own_call1=" + my.Replace(" ", "\u007f");
            _slice.SendWaveformCommand(cmd);
            string note = dstarctl1.NOTE;
            cmd = "set own_call2=" + note.Replace(" ", "\u007f");
            _slice.SendWaveformCommand(cmd);

            dstarctl1.Modified = dstarctl.ModifyFlags.NOFLAGS;

            btnCommit.Enabled = false;

            dstarctl1.MY = my;
            dstarctl1.NOTE = note;
            dstarctl1.UR = ur;
            dstarctl1.RPT1 = rpt1 == "DIRECT" ? string.Empty : rpt1;
            dstarctl1.RPT2 = rpt2 == "DIRECT" ? string.Empty : rpt2;
            dstarctl1.DRMode = !string.IsNullOrEmpty(dstarctl1.RPT1);


            // add new UR entry to dropdown list
            List<string> lst = new List<string>(dstarctl1.URList);
            ur = ur.ToUpper();
            if (!lst.Contains(ur))
            {
                lst.Add(ur);
                dstarctl1.URList = lst;
                Properties.Settings.Default.URList = string.Join("|", lst);
            }

            if (dstarctl1.DRMode)
            {
                if (rpt1.Length > 0 && rpt1 != "DIRECT")
                {
                    lst = new List<string>(dstarctl1.RPT1List);
                    rpt1 = rpt1.ToUpper();
                    if (!lst.Contains(rpt1))
                    {
                        lst.Add(rpt1);
                        dstarctl1.RPT1List = lst;
                        Properties.Settings.Default.RPT1List = string.Join("|", lst);
                    }
                }
                if (rpt2.Length > 0 && rpt2 != "DIRECT")
                {
                    lst = new List<string>(dstarctl1.RPT2List);
                    rpt2 = rpt2.ToUpper();
                    if (!lst.Contains(rpt2))
                    {
                        lst.Add(rpt2);
                        dstarctl1.RPT2List = lst;
                        Properties.Settings.Default.RPT2List = string.Join("|", lst);
                    }
                }
            }

            if (!string.IsNullOrEmpty(my))
                Properties.Settings.Default.MYCallSign = my;

        }

        public void ClearMYList()
        {
            dstarctl1.URList = new List<string>();
        }
        public void ClearRPT1List()
        {
            dstarctl1.RPT1List = new List<string>();
        }
        public void ClearRPT2List()
        {
            dstarctl1.RPT2List = new List<string>();
        }

    }

    public class LogEventArgs : EventArgs
    {
        public string LogEntry { get; set; }
        public LogEventArgs(string logentry)
        {
            LogEntry = logentry;
        }
    }
}
