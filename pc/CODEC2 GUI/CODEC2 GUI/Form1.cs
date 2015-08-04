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
        private List<Slice> _waveformSlices;

        public Form1()
        {
            InitializeComponent();

            _waveformSlices = new List<Slice>();
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
            if (_waveformSlices.Contains(slc))
            {
                // yes -- tear down any GUI components
                if (InvokeRequired)
                {
                    Invoke(new MethodInvoker(delegate
                    {
                        RemoveWaveformSliceControls(slc);
                    }));
                }
                else
                {
                    RemoveWaveformSliceControls(slc);
                }                

                // now remove it
                _waveformSlices.Remove(slc);
            }

            slc.PropertyChanged -= slc_PropertyChanged;
            slc.WaveformStatusReceived -= slc_WaveformStatusReceived;                      
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
            if (slc.DemodMode == "D*FM")
            {
                // yes -- is this Slice already in the list?
                if (!_waveformSlices.Contains(slc))
                {
                    // no -- lets go add it
                    _waveformSlices.Add(slc);

                    // now create GUI components to go with it
                    AddFDVSliceControls(slc);
                }
            }
            else
            {
                // was this Slice in FreeDV mode before?
                if (_waveformSlices.Contains(slc))
                {
                    // yes -- tear down any GUI components
                    RemoveWaveformSliceControls(slc);

                    // now remove it
                    _waveformSlices.Remove(slc);
                }
            }
        }


        void slc_WaveformStatusReceived(Slice slc, string status)
        {
            if(status.StartsWith("string"))
            {
                Debug.WriteLine(status);
                string x = "ASDLKFJASLDKJ";

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
                int index = _waveformSlices.IndexOf(slc);
                label1.Visible = false;

                int vertical_offset = 30;

                this.SuspendLayout();

                Label myInfo = new Label();
                myInfo.Text = "MY INFO";
                Label rxInfo = new Label();
                rxInfo.Text = "RX INFO";

                Label destinationRptrInLabel = new Label();
                destinationRptrInLabel.Text = "Destination Repeater:";
                destinationRptrInLabel.Anchor = AnchorStyles.Left;
                Label departureRptrInLabel = new Label();
                departureRptrInLabel.Text = "Departure Repeater:";
                Label companionCallInLabel = new Label();
                companionCallInLabel.Text = "Companion Call:";
                Label ownCall1InLabel = new Label();
                ownCall1InLabel.Text = "Own Call 1:";
                Label ownCall2InLabel = new Label();
                ownCall2InLabel.Text = "Own Call 2:";

                TextBox destinationRptrIn = new TextBox();
                TextBox departureRptrIn = new TextBox();
                TextBox companionCallIn = new TextBox();
                TextBox ownCall1In = new TextBox();
                TextBox ownCall2In = new TextBox();

                Label destinationRptrOutLabel = new Label();
                destinationRptrOutLabel.Text = "Destination Repeater:";
                Label departureRptrOutLabel = new Label();
                departureRptrOutLabel.Text = "Departure Repeater:";
                Label companionCallOutLabel = new Label();
                companionCallOutLabel.Text = "Companion Call:";
                Label ownCall1OutLabel = new Label();
                ownCall1OutLabel.Text = "Own Call 1:";
                Label ownCall2OutLabel = new Label();
                ownCall2OutLabel.Text = "Own Call 2:";

                TextBox destinationRptrOut = new TextBox();
                TextBox departureRptrOut = new TextBox();
                TextBox companionCallOut = new TextBox();
                TextBox ownCall1Out = new TextBox();
                TextBox ownCall2Out = new TextBox();

                GroupBox groupbox = new GroupBox();
                groupbox.SuspendLayout();
                //
                // groupBox
                //

                groupbox.Controls.Add(myInfo);
                groupbox.Controls.Add(rxInfo);

                groupbox.Controls.Add(destinationRptrInLabel);
                groupbox.Controls.Add(departureRptrInLabel);
                groupbox.Controls.Add(companionCallInLabel);
                groupbox.Controls.Add(ownCall1InLabel);
                groupbox.Controls.Add(ownCall2InLabel);
                groupbox.Controls.Add(destinationRptrIn);
                groupbox.Controls.Add(departureRptrIn);
                groupbox.Controls.Add(companionCallIn);
                groupbox.Controls.Add(ownCall1In);
                groupbox.Controls.Add(ownCall2In);

                groupbox.Controls.Add(destinationRptrOutLabel);
                groupbox.Controls.Add(departureRptrOutLabel);
                groupbox.Controls.Add(companionCallOutLabel);
                groupbox.Controls.Add(ownCall1OutLabel);
                groupbox.Controls.Add(ownCall2OutLabel);
                groupbox.Controls.Add(destinationRptrOut);
                groupbox.Controls.Add(departureRptrOut);
                groupbox.Controls.Add(companionCallOut);
                groupbox.Controls.Add(ownCall1Out);
                groupbox.Controls.Add(ownCall2Out);

                groupbox.Location = new Point(10, 10 + index * 220);
                groupbox.Name = "grpSlice" + index;
                groupbox.Size = new Size(500, 220);
                groupbox.Text = "Slice " + SliceIndexToLetter(slc.Index);

            

                myInfo.Location = new Point(75, 30);
                myInfo.Size = new Size(100, 20);
                myInfo.TextAlign = ContentAlignment.MiddleCenter;


                rxInfo.Location = new Point(325, 30);
                rxInfo.Size = new Size(100, 20);
                rxInfo.TextAlign = ContentAlignment.MiddleCenter;

                //
                // destinationRptrIn
                //
                destinationRptrInLabel.Location = new Point(10, 30 + vertical_offset);
                destinationRptrInLabel.Size = new Size(120, 20);
                destinationRptrIn.Location = new Point(130, 30 + vertical_offset);
                destinationRptrIn.Name = "destinationRptrIn" + slc.Index;
                destinationRptrIn.Size = new Size(100, 20);
                destinationRptrIn.PreviewKeyDown += new PreviewKeyDownEventHandler(txtIn_PreviewKeyDown);
                //
                // departureRptrIn

                departureRptrInLabel.Location = new Point(10, 60 + vertical_offset);
                departureRptrInLabel.Size = new Size(120, 20);
                departureRptrIn.Location = new Point(130, 60 + vertical_offset);
                departureRptrIn.Name = "departureRptrIn" + slc.Index;
                departureRptrIn.Size = new Size(100, 20);
                departureRptrIn.PreviewKeyDown += new PreviewKeyDownEventHandler(txtIn_PreviewKeyDown);

                companionCallInLabel.Location = new Point(10, 90 + vertical_offset);
                companionCallInLabel.Size = new Size(120, 20);
                companionCallIn.Location = new Point(130, 90 + vertical_offset);
                companionCallIn.Name = "companionCallIn" + slc.Index;
                companionCallIn.Size = new Size(100, 20);
                companionCallIn.PreviewKeyDown += new PreviewKeyDownEventHandler(txtIn_PreviewKeyDown);

                ownCall1InLabel.Location = new Point(10, 120 + vertical_offset);
                ownCall1InLabel.Size = new Size(120, 20);
                ownCall1In.Location = new Point(130, 120 + vertical_offset);
                ownCall1In.Name = "ownCall1In" + slc.Index;
                ownCall1In.Size = new Size(100, 20);
                ownCall1In.PreviewKeyDown += new PreviewKeyDownEventHandler(txtIn_PreviewKeyDown);

                ownCall2InLabel.Location = new Point(10, 150 + vertical_offset);
                ownCall2InLabel.Size = new Size(120, 20);
                ownCall2In.Location = new Point(130, 150 + vertical_offset);
                ownCall2In.Name = "ownCall2In" + slc.Index;
                ownCall2In.Size = new Size(100, 20);
                ownCall2In.PreviewKeyDown += new PreviewKeyDownEventHandler(txtIn_PreviewKeyDown);

                /* OUT indicates it get's retrieved by the radio */

                destinationRptrOutLabel.Location = new Point(250, 30 + vertical_offset);
                destinationRptrOutLabel.Size = new Size(120, 20);
                destinationRptrOut.Location = new Point(250 + 120, 30 + vertical_offset);
                destinationRptrOut.Name = "destinationRptrOut" + slc.Index;
                destinationRptrOut.Size = new Size(100, 20);
                destinationRptrOut.ReadOnly = true;

                departureRptrOutLabel.Location = new Point(250, 60 + vertical_offset);
                departureRptrOutLabel.Size = new Size(120, 20);
                departureRptrOut.Location = new Point(250 + 120, 60 + vertical_offset);
                departureRptrOut.Name = "departureRptrIn" + slc.Index;
                departureRptrOut.Size = new Size(100, 20);
                departureRptrOut.ReadOnly = true;

                companionCallOutLabel.Location = new Point(250, 90 + vertical_offset);
                companionCallOutLabel.Size = new Size(120, 20);
                companionCallOut.Location = new Point(250 + 120, 90 + vertical_offset);
                companionCallOut.Name = "companionCallOut" + slc.Index;
                companionCallOut.Size = new Size(100, 20);
                companionCallOut.ReadOnly = true;

                ownCall1OutLabel.Location = new Point(250, 120 + vertical_offset);
                ownCall1OutLabel.Size = new Size(120, 20);
                ownCall1Out.Location = new Point(250 + 120, 120 + vertical_offset);
                ownCall1Out.Name = "ownCall1Out" + slc.Index;
                ownCall1Out.Size = new Size(100, 20);
                ownCall1Out.ReadOnly = true;

                ownCall2OutLabel.Location = new Point(250, 150 + vertical_offset);
                ownCall2OutLabel.Size = new Size(120, 20);
                ownCall2Out.Location = new Point(250 + 120, 150 + vertical_offset);
                ownCall2Out.Name = "ownCall2Out" + slc.Index;
                ownCall2Out.Size = new Size(100, 20);
                ownCall2Out.ReadOnly = true;
                
                //
                // Form1
                //
                if (groupbox.Bottom > this.Height)
                    this.Height += 180;
                this.Controls.Add(groupbox);
                groupbox.ResumeLayout();
                this.ResumeLayout(false);
                this.PerformLayout();
            }
        }

        void txtIn_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
        {
            if (e.KeyCode != Keys.Enter) return;

            TextBox txt = sender as TextBox;
            if (txt == null) return;

            // get the index of the control
            int index;
            bool b = int.TryParse(txt.Name.Substring(6), out index);
            if(!b) return;

            // first we need to find the slice that goes with this control
            foreach(Slice slc in _waveformSlices)
            {
                if (slc.Index == index)
                {
                    // now that we have found the slice, we need to send a waveform command to set the string
                    slc.SendWaveformCommand("string=" + txt.Text);
                    return;
                }
            }
        }

        private void RemoveWaveformSliceControls(Slice slc)
        {
            lock (this)
            {
                this.SuspendLayout();

                int index = _waveformSlices.IndexOf(slc);

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
                                txt.PreviewKeyDown -= txtIn_PreviewKeyDown;
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

                if (_waveformSlices.Count == 1)
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
