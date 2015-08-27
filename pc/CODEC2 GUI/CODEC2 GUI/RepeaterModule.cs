/*******************************************************************************
 * RepeaterModule.cs									  			
 *
 * 	DStar Repeater Selection Dialog
 *
 *  Created on: 2015-08-27
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
using System.Windows.Forms;


namespace CODEC2_GUI
{
    public partial class RepeaterModule : Form
    {
        public string RptName
        {
            get; set;
        }
        public string RptDesc
        {
            get; set;
        }

        public ReflectorOrRepeater SelectedRpt
        {
            get; set;
        }

        private double[] Frequency = new double[3];
        private double[] Offset = new double[3];

        public RepeaterModule()
        {
            InitializeComponent();
        }

        private void RepeaterModule_Load(object sender, EventArgs e)
        {
            button1.Visible = false;
            button2.Visible = false;
            button3.Visible = false;

            string[] vals = RptDesc.Split('|');

            for (int idx = 3; idx < vals.Length; idx++)
            {
                string[] mod = vals[idx].Split(':');
                if (mod.Length != 2)
                    continue;
                string[] fr = mod[1].Split(' ');
                if (fr.Length == 0 || string.IsNullOrEmpty(fr[0]))
                    continue;
                if (mod[0] == "A")
                {
                    button1.Visible = true;
                    Frequency[0] = fr.Length > 0 ? Convert.ToDouble(fr[0]) : 0;
                    Offset[0] = fr.Length > 1 ? Convert.ToDouble(fr[1]) : 0;
                    button1.Text = string.Format("Mod A {0:#.000} MHz; {1:#.0} MHz", Frequency[0], Offset[0]);
                }
                else if (mod[0] == "B")
                {
                    button2.Visible = true;
                    Frequency[1] = fr.Length > 0 ? Convert.ToDouble(fr[0]) : 0;
                    Offset[1] = fr.Length > 1 ? Convert.ToDouble(fr[1]) : 0;
                    button2.Text = string.Format("Mod B {0:#.000} MHz; {1:#.0} MHz", Frequency[1], Offset[1]);
                }
                else if (mod[0] == "C")
                {
                    button3.Visible = true;
                    Frequency[2] = fr.Length > 0 ? Convert.ToDouble(fr[0]) : 0;
                    Offset[2] = fr.Length > 1 ? Convert.ToDouble(fr[1]) : 0;
                    button3.Text = string.Format("Mod C {0:#.000} MHz; {1:#.0} MHz", Frequency[2], Offset[2]);
                }
            }
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            SelectedRpt = new ReflectorOrRepeater();
            SelectedRpt.Name = string.Format("{0,-7}A", RptName.Trim());
            SelectedRpt.Desc = RptDesc;
            SelectedRpt.Frequency = Frequency[0];
            SelectedRpt.Offset = Offset[0];
            DialogResult = DialogResult.OK;
            Close();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            SelectedRpt = new ReflectorOrRepeater();
            SelectedRpt.Name = string.Format("{0,-7}B", RptName.Trim());
            SelectedRpt.Desc = RptDesc;
            SelectedRpt.Frequency = Frequency[1];
            SelectedRpt.Offset = Offset[1];
            DialogResult = DialogResult.OK;
            Close();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            SelectedRpt = new ReflectorOrRepeater();
            SelectedRpt.Name = string.Format("{0,-7}C", RptName.Trim());
            SelectedRpt.Desc = RptDesc;
            SelectedRpt.Frequency = Frequency[2];
            SelectedRpt.Offset = Offset[2];
            DialogResult = DialogResult.OK;
            Close();
        }
    }
}
