/*******************************************************************************
 *	Form1.cs									  			
 *
 * 	An example GUI showing how to interface to the FlexRadio Modem API
 *
 *  Created on: 2014-08-28
 *      Author: Eric / KE5DTO
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
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using Flex.Smoothlake.FlexLib;


namespace CODEC2_GUI
{
    public partial class Form1 : Form
    {
        private bool _radioConnected = false;
        private Radio _radio = null;
        //private List<Slice> _slices;
        private List<Slice> _fdvSlices;
        private Dictionary<Meter, Slice> _SNRTable;

        public Form1()
        {
            InitializeComponent();

            //_slices = new List<Slice>();
            _fdvSlices = new List<Slice>();
            _SNRTable = new Dictionary<Meter, Slice>();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            API.RadioAdded += new API.RadioAddedEventHandler(API_RadioAdded);
            API.RadioRemoved += new API.RadioRemovedEventHandler(API_RadioRemoved);
            API.Init();
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
            
        }

        //*************************************
        //         Radio Event Handlers
        //*************************************

        void _radio_SliceAdded(Slice slc)
        {
            //if(_slices.Contains(slc)) return;

            //_slices.Add(slc);

            slc.PropertyChanged += new PropertyChangedEventHandler(slc_PropertyChanged);
            slc.WaveformStatusReceived += new Slice.WaveformStatusReceivedEventHandler(slc_WaveformStatusReceived);
            slc.MeterAdded += new Slice.MeterAddedEventHandler(slc_MeterAdded);
            slc.MeterRemoved += new Slice.MeterRemovedEventHandler(slc_MeterRemoved);
         
            Meter m = slc.FindMeterByName("SNR");
            if(m != null)
                AddSNRMeter(slc, m);

            if (InvokeRequired)
            {
                Invoke(new MethodInvoker(delegate
                {
                    CheckDemodMode(slc);
                }));
            }
            else
            {
                CheckDemodMode(slc);
            }
        }

        void _radio_SliceRemoved(Slice slc)
        {
            //if (!_slices.Contains(slc)) return;

            if (_fdvSlices.Contains(slc))
            {
                // yes -- tear down any GUI components
                if (InvokeRequired)
                {
                    Invoke(new MethodInvoker(delegate
                    {
                        RemoveFDVSliceControls(slc);
                    }));
                }
                else
                {
                    RemoveFDVSliceControls(slc);
                }                

                // now remove it
                _fdvSlices.Remove(slc);
            }

            //_slices.Remove(slc);

            slc.PropertyChanged -= slc_PropertyChanged;
            slc.WaveformStatusReceived -= slc_WaveformStatusReceived;
            slc.MeterAdded -= slc_MeterAdded;
            slc.MeterRemoved -= slc_MeterRemoved;                        
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
            if (slc.DemodMode == "FDV")
            {
                // yes -- is this Slice already in the list?
                if (!_fdvSlices.Contains(slc))
                {
                    // no -- lets go add it
                    _fdvSlices.Add(slc);

                    // now create GUI components to go with it
                    AddFDVSliceControls(slc);
                }
            }
            else
            {
                // was this Slice in FreeDV mode before?
                if (_fdvSlices.Contains(slc))
                {
                    // yes -- tear down any GUI components
                    RemoveFDVSliceControls(slc);

                    // now remove it
                    _fdvSlices.Remove(slc);
                }
            }
        }

        private void AddSNRMeter(Slice slc, Meter m)
        {
            if (_SNRTable.ContainsKey(m)) return;

            _SNRTable.Add(m, slc);
            m.DataReady += new Meter.DataReadyEventHandler(SNR_DataReady);
        }

        void slc_MeterAdded(Slice slc, Meter m)
        {
            if (m.Name == "SNR")
                AddSNRMeter(slc, m);
        }

        void SNR_DataReady(Meter meter, float data)
        {
            if (!_SNRTable.ContainsKey(meter)) return;

            Slice slc = _SNRTable[meter];

            UpdateSNR(slc, data);
        }

        private void UpdateSNR(Slice slc, float data)
        {
            // use the slice to find the progress bar control to update
            Control c = FindControlByName(this, "bar" + slc.Index);
            if (c == null) return;

            // get a good reference to the bar instead of a generic control
            VerticalProgressBar bar = c as VerticalProgressBar;
            if (bar == null) return;

            // convert the data into a percentage
            // we will map the 0-20dB SNR range onto the 0-100 bar value
            
            // check incoming data limits
            if (data < 0) data = 0.0f;
            if (data > 20) data = 20.0f;

            // do the conversion
            int new_value = (int)(data * 5);
            
            // now set the value -- careful to Invoke since this is a GUI affecting call
            if (InvokeRequired)
            {
                Invoke(new MethodInvoker(delegate
                {
                    bar.Value = new_value;
                }));
            }
            else
            {
                bar.Value = new_value;
            }
        }

        void slc_MeterRemoved(Slice slc, Meter m)
        {
            if(m.Name != "SNR") return;

            if (_SNRTable.ContainsKey(m))
                _SNRTable.Remove(m);

            m.DataReady -= SNR_DataReady;
        }

        void slc_WaveformStatusReceived(Slice slc, string status)
        {
            if(status.StartsWith("string"))
            {
                Debug.WriteLine(status);
                //string x = "ASDLKFJASLDKJ";

                int start_pos = status.IndexOf("\"");
                // did we find beginning quotes?
                if(start_pos < 0) return; // no -- return

                int end_pos = status.LastIndexOf("\"");
                // did we find ending quotes?
                if(end_pos == start_pos) return; // no -- return
                
                start_pos += 1; // ignore beginning quotes
                string value = status.Substring(start_pos,end_pos - start_pos);

                // handle the remapped spaces
                value = value.Replace("\u007f", " ");
                
                Control c = FindControlByName(this, "txtIn" + slc.Index);
                if (c == null) return;

                TextBox txtbox = c as TextBox;
                if (txtbox == null) return;

                if (InvokeRequired)
                {
                    Invoke(new MethodInvoker(delegate
                    {
                        txtbox.Text = value;
                    }));
                }
                else
                {
                    txtbox.Text = value;
                }
            }
        }

        //*************************************
        //           GUI Routines
        //*************************************

        private void AddFDVSliceControls(Slice slc)
        {
            lock (this)
            {
                int index = _fdvSlices.IndexOf(slc);
                label1.Visible = false;

                this.SuspendLayout();

                TextBox txtIn = new TextBox();
                TextBox txtOut = new TextBox();
                VerticalProgressBar bar = new VerticalProgressBar();
                GroupBox groupbox = new GroupBox();
                groupbox.SuspendLayout();
                //
                // groupBox
                //
                groupbox.Controls.Add(txtIn);
                groupbox.Controls.Add(txtOut);
                groupbox.Controls.Add(bar);
                groupbox.Location = new Point(10, 10 + index * 110);
                groupbox.Name = "grpSlice" + index;
                groupbox.Size = new Size(260, 100);
                groupbox.Text = "Slice " + SliceIndexToLetter(slc.Index);
                //
                // txtIn
                //
                txtIn.Location = new Point(40, 30);
                txtIn.Name = "txtIn" + slc.Index;
                txtIn.Size = new Size(200, 20);
                txtIn.ReadOnly = true;
                //
                // txtOut
                //
                txtOut.Location = new Point(40, 60);
                txtOut.Name = "txtOut" + slc.Index;
                txtOut.Size = new Size(200, 20);
                txtOut.PreviewKeyDown += new PreviewKeyDownEventHandler(txtOut_PreviewKeyDown);
                //
                // bar
                //
                bar.Location = new Point(10, 20);
                bar.Name = "bar" + slc.Index;
                bar.Size = new Size(20, 70);
                bar.Value = 25;
                //
                // Form1
                //
                if (groupbox.Bottom > this.Height)
                    this.Height += 110;
                this.Controls.Add(groupbox);
                groupbox.ResumeLayout();
                this.ResumeLayout(false);
                this.PerformLayout();
            }
        }

        void txtOut_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            if (e.KeyCode != Keys.Enter) return;

            TextBox txt = sender as TextBox;
            if (txt == null) return;

            // get the index of the control
            int index;
            bool b = int.TryParse(txt.Name.Substring(6), out index);
            if(!b) return;

            // first we need to find the slice that goes with this control
            foreach(Slice slc in _fdvSlices)
            {
                if (slc.Index == index)
                {
                    // now that we have found the slice, we need to send a waveform command to set the string
                    slc.SendWaveformCommand("string=" + txt.Text);
                    return;
                }
            }
        }

        private void RemoveFDVSliceControls(Slice slc)
        {
            lock (this)
            {
                this.SuspendLayout();

                int index = _fdvSlices.IndexOf(slc);

                // find the right groupbox and remove it
                for (int i = 0; i < this.Controls.Count; i++)
                {
                    Control c = this.Controls[i];
                    GroupBox groupbox = c as GroupBox;
                    if (groupbox == null) continue;

                    int temp;
                    bool b = int.TryParse(groupbox.Name.Substring("grpSlice".Length), out temp);

                    if (!b) continue;

                    // is this the one we are looking for?
                    if (temp == index)
                    {
                        // yes -- remove the whole groupbox
                        this.Controls.Remove(groupbox);
                        i--;

                        // detach the PreviewKeyDown event
                        foreach (Control c2 in groupbox.Controls)
                        {
                            if (c2.Name.StartsWith("txtOut"))
                            {
                                TextBox txt = c2 as TextBox;
                                txt.PreviewKeyDown -= txtOut_PreviewKeyDown;
                            }
                        }
                        groupbox.Controls.Clear();
                    }
                    // is this one below the one we are removing?
                    else if (temp > index)
                    {
                        // yes -- move it on up
                        groupbox.Top -= 110;
                        groupbox.Name = "grpSlice" + (temp - 1);
                    }
                }

                if (_fdvSlices.Count == 1)
                    label1.Visible = true;

                this.ResumeLayout(false);
                this.PerformLayout();
            }
        }

        private Control FindControlByName(Control start_control, string name)
        {
            foreach (Control c in start_control.Controls)
            {
                if (c.Name == name) return c;

                if (c.GetType() == typeof(GroupBox))
                {
                    Control result = FindControlByName(c, name);
                    if (result != null) return result;
                }
            }

            return null;
        }

        //*************************************
        //           GUI Routines
        //*************************************

        private string SliceIndexToLetter(int index)
        {
            return Convert.ToChar('A' + index).ToString();
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
