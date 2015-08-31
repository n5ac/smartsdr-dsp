/*******************************************************************************
 * SelectForm.cs									  			
 *
 * 	DStar Repeater / Reflector Selection Dialog
 *
 *  Created on: 2015-08-23
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
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace CODEC2_GUI
{
    public partial class SelectForm : Form
    {
        public enum RMode { Repeater, Reflector, ReflectorCmd };
        public RMode Mode { get; set; }
        public bool showOnly { get; set; }
        public bool repeaterOnly { get; set; }

        public string SelectedName { get; set; }
        public string SelectedDesc { get; set; }

        private IEnumerable<ReflectorOrRepeater> ReflectorList { get; set; }
        private IEnumerable<ReflectorOrRepeater> RepeaterList { get; set; }
        private IEnumerable<ReflectorOrRepeater> ReflectorCmdList { get; set; }

        public SelectForm()
        {
            InitializeComponent();
            ReflectorCmdList = new List<ReflectorOrRepeater>() {
            { new ReflectorOrRepeater() { Name = "       U", Desc = "Unlink command"} },
            { new ReflectorOrRepeater() { Name = "       I", Desc = "Info command" } },
            { new ReflectorOrRepeater() { Name = "       E", Desc = "Echo command" } }
          };
        }

        private void SelectForm_Load(object sender, EventArgs e)
        {
            if (showOnly)
            {
                btnSelect.Visible = false;
                btnCancel.Text = "Close";
            } 


            DstarInfo di = new DstarInfo();

            if (repeaterOnly)
            {
                Mode = RMode.Repeater;
                rb1.Checked = true;
                rb2.Visible = false;
                rb3.Visible = false;

                if (di.Repeaters == null || di.Repeaters.Count == 0)
                {
                    MessageBox.Show(this, "Load / Reload Repeater List from File Menu!", "Select Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    DialogResult = DialogResult.Cancel;
                    Close();
                    return;
                }
                RepeaterList = di.Repeaters;
            }
            else
            {
                if (di.Refelectors == null || di.Refelectors.Count == 0)
                {
                    MessageBox.Show(this, "Load / Reload Reflectors List from File Menu!", "Select Error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    DialogResult = DialogResult.Cancel;
                    Close();
                    return;
                }
                ReflectorList = di.Refelectors;

                rb1.Visible = false;
                if (Mode == RMode.Repeater)
                    Mode = RMode.Reflector;

                switch (Mode)
                {
                    case RMode.Repeater:
                        rb1.Checked = true;
                        break;
                    case RMode.Reflector:
                        rb2.Checked = true;
                        break;
                    case RMode.ReflectorCmd:
                        rb3.Checked = true;
                        break;
                }
            }
            setlistitems();
        }

        private void setlistitems()
        {
            listView1.Items.Clear();
            IEnumerable<ReflectorOrRepeater> rlist;
            switch(Mode)
            {
                case RMode.Reflector:
                    rlist = ReflectorList;
                    break;
                case RMode.ReflectorCmd:
                    rlist = ReflectorCmdList;
                    break;
                case RMode.Repeater:
                default:
                    rlist = RepeaterList;
                    break;
            }
            if (rlist != null)
            {
                foreach (ReflectorOrRepeater rr in rlist)
                {
                    listView1.Items.Add(new ListViewItem(new string[] { rr.Name, rr.Desc }));
                }
            }
            if (listView1.Items.Count > 0)
                listView1.SelectedIndices.Add(0);

            listView1.Select();
        }

        private void btnSelect_Click(object sender, EventArgs e)
        {
            if (listView1.SelectedItems.Count > 0)
            {
                SelectedName = listView1.SelectedItems[0].Text;
                StringBuilder sb = new StringBuilder();
                foreach(ListViewItem.ListViewSubItem lvi in listView1.SelectedItems[0].SubItems)
                {
                    sb.AppendFormat("{0}{1}", sb.Length > 0 ? "|" : string.Empty, lvi.Text);
                }
                SelectedDesc = sb.ToString();
                DialogResult = DialogResult.OK;
                Close();
            }
            else
                MessageBox.Show(this, "Nothing selected!", "Information"); 
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            Close();
        }


        private void rb1_Click(object sender, EventArgs e)
        {
            if (rb1.Checked)
            {
                Mode = RMode.Repeater;
                setlistitems();
            }
        }

        private void rb2_CheckedChanged(object sender, EventArgs e)
        {
            if (rb2.Checked)
            {
                Mode = RMode.Reflector;
                setlistitems();
            }
        }

        private void rb3_CheckedChanged(object sender, EventArgs e)
        {
            if (rb3.Checked)
            {
                Mode = RMode.ReflectorCmd;
                setlistitems();
            }
        }

        private int sortColumn;

        private void listView1_ColumnClick(object sender, ColumnClickEventArgs e)
        {
            if (e.Column != sortColumn)
            {
                // Set the sort column to the new column.
                sortColumn = e.Column;
                // Set the sort order to ascending by default.
                listView1.Sorting = SortOrder.Ascending;
            }
            else
            {
                // Determine what the last sort order was and change it.
                if (listView1.Sorting == SortOrder.Ascending)
                    listView1.Sorting = SortOrder.Descending;
                else
                    listView1.Sorting = SortOrder.Ascending;
            }

            ColumnHeader ch = listView1.Columns[e.Column];
            listView1.ListViewItemSorter = new ListViewSorter(e.Column, listView1.Sorting);
            listView1.Sort();            
        }
    }

    class ListViewSorter : IComparer
    {
        int col;
        SortOrder sortOrder;

        public ListViewSorter()
        {
            col = 0;
            sortOrder = SortOrder.Ascending;
        }
        public ListViewSorter(int scol, SortOrder sorder)
        {
            col = scol;
            sortOrder = sorder;
        }
        public int Compare(object x, Object y)
        {
            int returnVal = String.Compare(((ListViewItem)x).SubItems[col].Text,
                ((ListViewItem)y).SubItems[col].Text, true);
            if (sortOrder == SortOrder.Descending)
                returnVal *= -1;
            return returnVal;
        }
    }
}
