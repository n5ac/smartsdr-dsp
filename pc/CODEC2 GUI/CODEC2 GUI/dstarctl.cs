/*******************************************************************************
 * dstarctl.cs									  			
 *
 * 	UI Control for DSTAR Settings
 *
 *  Created on: 2015-08-20
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
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace CODEC2_GUI
{
    public partial class dstarctl : UserControl, INotifyPropertyChanged
    {
        [Flags]
        public enum ModifyFlags : byte
        {
            NOFLAGS = 0,
            MYFLAG = 1,
            NOTEFLAG = 2,
            URFLAG = 4,
            RPT1FLAG = 8,
            RPT2FLAG = 16,
            DRFLAG = 32,
            MESSAGE = 64
        }
        public ModifyFlags Modified { get; set; }

        private bool DRModeOrig;
        private string MYOrig = string.Empty;
        private string NOTEOrig = string.Empty;
        private string UROrig = string.Empty;
        private string RPT1Orig = string.Empty;
        private string RPT2Orig = string.Empty;
        private string MESSAGEOrig = string.Empty;

        private bool inreset;

        public bool DRMode
        {
            get
            {
                return rbDR.Checked;
            }
            set
            {
                if ((rbDV.Checked && value) || (rbDR.Checked && !value) || (!rbDV.Checked && !rbDR.Checked))
                {
                    rbDV.Checked = !value;
                    rbDR.Checked = value;
                    Modified = Modified & ~ModifyFlags.DRFLAG;
                    rpt1txt.Enabled =
                    rpt1.Enabled =
                    rpt2.Enabled =
                    btnRpt.Visible =
                    rpt2txt.Enabled =
                    value;
                    OnPropertyChanged("DRMode");
                }
                DRModeOrig = value;
            }
        }
        public string MY
        {
            get
            {
                return mytxt.Text.ToUpper();
            }
            set
            {
                if (value == null)
                    value = string.Empty;
                if (string.Compare(value, mytxt.Text, true) != 0)
                {
                    mytxt.Text = value.ToUpper();
                    Modified = Modified & ~ModifyFlags.MYFLAG;
                    OnPropertyChanged("MY");
                }
                MYOrig = value;
                updateReset();
            }
        }
        public string NOTE
        {
            get
            {
                return mynotetxt.Text;
            }
            set
            {
                if (value == null)
                    value = string.Empty;
                if (string.Compare(value, mynotetxt.Text, true) != 0)
                {
                    mynotetxt.Text = value;
                    Modified = Modified & ~ModifyFlags.NOTEFLAG;
                    OnPropertyChanged("NOTE");
                }
                NOTEOrig = value;
                updateReset();
            }
        }
        public string UR
        {
            get
            {
                return urtxt.Text.ToUpper();
            }
            set
            {
                if (value == null)
                    value = string.Empty;
                if (string.Compare(value, urtxt.Text, true) != 0)
                {
                    urtxt.Text = value.ToUpper();
                    Modified = Modified & ~ModifyFlags.URFLAG;
                    OnPropertyChanged("UR");
                }
                UROrig = value;
                updateReset();
            }
        }
        public string RPT1
        {
            get
            {
                return rpt1txt.Text.ToUpper();
            }
            set
            {
                if (value == null)
                    value = string.Empty;
                if (rpt1txt.Text.StartsWith(value) == false)
                {
                    inreset = true;
                    value = value.ToUpper();
                    int idx = rpt1txt.FindString(value);
                    if (idx >= 0)
                    {
                        rpt1txt.SelectedIndex = idx;
                        value = rpt1txt.Text;
                    }
                    else
                        rpt1txt.Text = value;
                    inreset = false;
                    Modified = Modified & ~ModifyFlags.RPT1FLAG;
                    OnPropertyChanged("RPT1");
                }
                RPT1Orig = value;
                updateReset();
            }
        }
        public string RPT2
        {
            get
            {
                return DRMode ? rpt2txt.Text.ToUpper() : string.Empty;
            }
            set
            {
                if (value == null)
                    value = string.Empty;
                if (string.Compare(value, rpt2txt.Text, true) != 0)
                {
                    rpt2txt.Text = value.ToUpper();
                    Modified = Modified & ~ModifyFlags.RPT2FLAG;
                    OnPropertyChanged("RPT2");
                }
                RPT2Orig = value;
                updateReset();
            }
        }

        public string MESSAGE
        {
            get
            {
                return msgtxt.Text;
            }
            set
            {
                if (value == null)
                    value = string.Empty;
                if (string.Compare(value, msgtxt.Text, true) != 0)
                {
                    msgtxt.Text = value.ToUpper();
                    Modified = Modified & ~ModifyFlags.MESSAGE;
                    OnPropertyChanged("MESSAGE");
                }
                MESSAGEOrig = value;
                updateReset();
            }
        }

        public List<string> URList
        {
            get
            {
                List<string> lst = new List<string>(urtxt.Items.Cast<string>());
                int i = lst.IndexOf("CQCQCQ");
                if (i >= 0)
                    lst.RemoveAt(i);
                return lst;
            }
            set
            {
                urtxt.Items.Clear();
                if (value != null && value.Count > 0)
                {
                    if (!value.Contains("CQCQCQ"))
                        urtxt.Items.Add("CQCQCQ");
                    urtxt.Items.AddRange(value.ToArray());
                }
                else 
                    urtxt.Items.Add("CQCQCQ");
            }
        }

        public List<string> RPT1List
        {
            get
            {
                List<string> lst = new List<string>(rpt1txt.Items.Cast<string>());
                return lst;
            }
            set
            {
                rpt1txt.Items.Clear();
                if (value != null && value.Count > 0)
                {
                    rpt1txt.Items.AddRange(value.ToArray());
                }
            }
        }

        public List<string> RPT2List
        {
            get
            {
                List<string> lst = new List<string>(rpt2txt.Items.Cast<string>());
                return lst;
            }
            set
            {
                rpt2txt.Items.Clear();
                if (value != null && value.Count > 0)
                {
                    rpt2txt.Items.AddRange(value.ToArray());
                }
            }
        }

        public List<string> MESSAGEList
        {
            get
            {
                List<string> lst = new List<string>(msgtxt.Items.Cast<string>());
                return lst;
            }
            set
            {
                msgtxt.Items.Clear();
                if (value != null && value.Count > 0)
                {
                    msgtxt.Items.AddRange(value.ToArray());
                }
            }
        }

        public dstarctl()
        {
            InitializeComponent();
        }

        public event PropertyChangedEventHandler PropertyChanged;

        private void OnPropertyChanged(string prop)
        {
            if (inreset)
                return;

            if (prop == "Modified")
            {
                btnReset.Visible = Modified != ModifyFlags.NOFLAGS;
            }
            if (PropertyChanged != null)
                PropertyChanged(this, new PropertyChangedEventArgs(prop));
        }

        private void rbDV_Click(object sender, EventArgs e)
        {
            if (rbDR.Checked == true)
                rbDR.Checked = false;

            Modified = Modified & ~ModifyFlags.DRFLAG;
            rpt1txt.Enabled = false;
            rpt1txt.Text = string.Empty;
            rpt2txt.Text = string.Empty;
            rpt1.Enabled = false;
            rpt2.Enabled = false;
            rpt2txt.Enabled = false;
            btnRpt.Visible = false;
            OnPropertyChanged("DRMode");
            OnPropertyChanged("Modified");

        }

        private void rbDR_Click(object sender, EventArgs e)
        {
            if (rbDV.Checked == true)
                rbDV.Checked = false;
            Modified = Modified | ModifyFlags.DRFLAG;
            rpt1txt.Enabled = true;
            rpt1.Enabled = true;
            rpt2.Enabled = true;
            rpt2txt.Enabled = true;
            btnRpt.Visible = true;
            OnPropertyChanged("DRMode");
            OnPropertyChanged("Modified");
        }

        private void dstarctl_Load(object sender, EventArgs e)
        {
            Modified = ModifyFlags.NOFLAGS;
            btnReset.Visible = false;
            rpt1tip.SetToolTip(rpt1txt, "Access / Area Callsign");
            rpt2tip.SetToolTip(rpt2txt, "Link / Gateway Callsign");
            urtip.SetToolTip(urtxt, "Destination Callsign (CQ, Individual Callsign or Reflector Command)");
            mytip.SetToolTip(mytxt, "MY Callsign (8 chars max)");
            mynotetip.SetToolTip(mynotetxt, "Note (4 chars max)");
            dvtip.SetToolTip(rbDV, "DStar Simplex Mode");
            drtip.SetToolTip(rbDR, "DStar Repeater Mode");
            msgtip.SetToolTip(msgtxt, "Slow Data Message (20 chars max)");
        }


        private void mytxt_TextChanged(object sender, EventArgs e)
        {
            if (inreset)
                return;
            Modified = Modified | ModifyFlags.MYFLAG;
            OnPropertyChanged("MY");
            OnPropertyChanged("Modified");
        }

        private void mynotetxt_TextChanged(object sender, EventArgs e)
        {
            if (inreset)
                return;
            Modified = Modified | ModifyFlags.NOTEFLAG;
            OnPropertyChanged("NOTE");
            OnPropertyChanged("Modified");
        }

        private void urtxt_TextChanged(object sender, EventArgs e)
        {
            if (inreset)
                return;
            Modified = Modified | ModifyFlags.URFLAG;
            OnPropertyChanged("UR");
            OnPropertyChanged("Modified");
        }

        private void rpt1txt_TextChanged(object sender, EventArgs e)
        {
            if (inreset)
                return;
            
            string rtxt = rpt1txt.Text;
            if (!string.IsNullOrWhiteSpace(rtxt))
            {
                string[] srtxt = rtxt.Split('~');
                if (srtxt.Length > 0 && srtxt[0].Length > 0)
                {
                    if (srtxt[0].Length > 8)
                    {
                        srtxt[0] = srtxt[0].Substring(0, 8);
                        rtxt = string.Join("~", srtxt);
                        rpt1txt.Text = rtxt;
                        rpt1txt.SelectionStart = 7;
                        rpt1txt.SelectionLength = 0;
                    }
                }
            }

            Modified = Modified | ModifyFlags.RPT1FLAG;
            updateRpt2();
            OnPropertyChanged("RPT1");
            OnPropertyChanged("Modified");
        }

        private void rpt2txt_TextChanged(object sender, EventArgs e)
        {
            if (inreset)
                return;
            Modified = Modified | ModifyFlags.RPT2FLAG;
            OnPropertyChanged("RPT2");
            OnPropertyChanged("Modified");
        }

        private void msgtxt_TextChanged(object sender, EventArgs e)
        {
            if (inreset)
                return;
            Modified = Modified | ModifyFlags.MESSAGE;
            OnPropertyChanged("MESSAGE");
            OnPropertyChanged("Modified");
        }


        private void rpt1txt_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (inreset)
                return;
            Modified = Modified | ModifyFlags.RPT1FLAG;
            updateRpt2();
            OnPropertyChanged("RPT1");
            OnPropertyChanged("Modified");
        }

        private void rpt2txt_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (inreset)
                return;
            Modified = Modified | ModifyFlags.RPT2FLAG;
            OnPropertyChanged("RPT2");
            OnPropertyChanged("Modified");
        }

        private void urtxt_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (inreset)
                return;
            Modified = Modified | ModifyFlags.URFLAG;
            OnPropertyChanged("UR");
            OnPropertyChanged("Modified");
        }

        private void msgtxt_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (inreset)
                return;
            Modified = Modified | ModifyFlags.MESSAGE;
            OnPropertyChanged("MESSAGE");
            OnPropertyChanged("Modified");
        }



        private void updateReset()
        {
            bool isorig = (MYOrig == MY && NOTEOrig == NOTE && UROrig == UR && 
                RPT1Orig == RPT1 && RPT2Orig == RPT2 && MESSAGEOrig == MESSAGE);

            if (btnReset.Visible == isorig)
            {
                btnReset.Visible = !isorig;
                if (isorig)
                    Modified = ModifyFlags.NOFLAGS;
            }
            OnPropertyChanged("Modified");
        }

        private void btnReset_Click(object sender, EventArgs e)
        {
            inreset = true;

            rbDV.Checked = !DRModeOrig;
            rbDR.Checked = DRModeOrig;
            mytxt.Text = MYOrig;
            mynotetxt.Text = NOTEOrig;
            urtxt.Text = UROrig;
            rpt1txt.Text = RPT1Orig;
            rpt2txt.Text = RPT2Orig;
            rpt1txt.Enabled =
            rpt1.Enabled =
            rpt2.Enabled =
            btnRpt.Visible = 
            rpt2txt.Enabled =
            DRModeOrig;
            msgtxt.Text = MESSAGEOrig;

            btnReset.Visible = false;
            inreset = false;
            Modified = ModifyFlags.NOFLAGS;

            OnPropertyChanged("MY");
            OnPropertyChanged("NOTE");
            OnPropertyChanged("UR");
            OnPropertyChanged("RPT1");
            OnPropertyChanged("RPT2");
            OnPropertyChanged("MESSAGE");
            OnPropertyChanged("Modified");
        }

        private void btnRef_Click(object sender, EventArgs e)
        {
            try
            {
                SelectForm sf = new SelectForm();
                sf.Owner = this.TopLevelControl as Form;
                sf.repeaterOnly = false;

                if (sf.ShowDialog() == DialogResult.OK)
                {
                    //if (sf.Mode == SelectForm.RMode.Repeater)
                    //    urtxt.Text = "/" + sf.SelectedName.Split(' ')[0];
                    //else 
                    if (sf.Mode == SelectForm.RMode.Reflector)
                        urtxt.Text = string.Format("{0,-7}L", sf.SelectedName);
                    else
                        urtxt.Text = sf.SelectedName;

                    Modified = Modified | ModifyFlags.URFLAG;
                    OnPropertyChanged("UR");
                    OnPropertyChanged("Modified");
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
                MessageBox.Show(sb.ToString(), "Select DSTARINFO Error!", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            }
        }

        private void btnRpt_Click(object sender, EventArgs e)
        {
            try
            {
                SelectForm sf = new SelectForm();
                sf.repeaterOnly = true;
                sf.Owner = this.TopLevelControl as Form;
                if (sf.ShowDialog() == DialogResult.OK)
                {
                    RepeaterModule rpm = new RepeaterModule();
                    rpm.Owner = this.TopLevelControl as Form;
                    rpm.RptName = sf.SelectedName;
                    rpm.RptDesc = sf.SelectedDesc;
                    if (rpm.ShowDialog() == DialogResult.OK)
                    {
                        rpt1txt.Text = rpm.SelectedRpt.RepeaterName;
                        Modified = Modified | ModifyFlags.RPT1FLAG;
                        updateRpt2();
                        OnPropertyChanged("RPT1");
                        OnPropertyChanged("Modified");
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
                MessageBox.Show(sb.ToString(), "Select DSTARINFO Error!", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            }
        }

        private void updateRpt2()
        {
            string[] srpt1 = rpt1txt.Text.Split('~');
            if (srpt1.Length > 0 && srpt1[0].Length > 0 && !srpt1[0].StartsWith("DIRECT"))
            {
                char last = 'G';
                if (rpt2txt.Text.Length > 0)
                    last = rpt2txt.Text.Last();
                string rcs = srpt1[0].ToUpper().Trim();
                int idx = rcs.IndexOf(' ');
                if (idx > 0)
                    rcs = rcs.Substring(0, idx);
                else if (rcs.Length > 7)
                    rcs = rcs.Substring(0, 7); 
                string rpts = string.Format("{0,-7}S", rcs);
                string rptg = string.Format("{0,-7}G", rcs);
                RPT2List = new List<string>() { rptg, rpts };
                if (rpt2txt.Text != rptg && rpt2txt.Text != rpts)
                {
                    if (last == 'S')
                        rpt2txt.SelectedIndex = 1;
                    else 
                        rpt2txt.SelectedIndex = 0;
                }
                OnPropertyChanged("RPT2");
            }
        }
    }

}
