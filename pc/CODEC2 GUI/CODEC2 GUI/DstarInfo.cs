/*******************************************************************************
 * DstarInfo.cs									  			
 *
 * 	Helper class which collects DSTAR information
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
using System.Linq;
using System.Text;
using System.Net;
using System.Net.Http;
using System.Threading.Tasks;
using System.IO;
using System.Net.Http.Headers;
using System.Reflection;

namespace CODEC2_GUI
{
    public class DstarInfo
    {
        public DstarInfo()
        {

        }

        private List<ReflectorOrRepeater> Repeaters_;
        public List<ReflectorOrRepeater> Repeaters
        {
            get
            {
                if (Repeaters_ == null)
                {
                    LoadRepeaters();
                    if (Repeaters_ == null || Repeaters_.Count == 0)
                    {
                        try
                        {
                            FetchAndSaveRepeaterList();
                        }
                        catch { }
                    }
                }
                return Repeaters_;
            }
        }

        private List<ReflectorOrRepeater> Reflectors_;
        public List<ReflectorOrRepeater> Refelectors
        {
            get
            {
                if (Reflectors_ == null)
                { 
                    LoadReflectors();
                    if (Reflectors_ == null || Reflectors_.Count == 0)
                    {
                        try
                        {
                            FetchAndSaveReflectorList();
                        }
                        catch { }
                    }
                }
                return Reflectors_;
            }
        }

        public bool FetchAndSaveRepeaterList()
        {
            return false;
        }

        public bool FetchAndSaveReflectorList()
        {
            bool result = false;
            try
            {
                List<ReflectorOrRepeater> refs = GetDStarInfoReflectors();
                if (refs != null && refs.Count > 0)
                {
                    SaveReflectors(refs);
                    Reflectors_ = refs;
                    result = true;
                }
            }
            catch(Exception ex)
            {
                string msg = "Fetch and save Relflector list failed!";
                System.Diagnostics.Debug.WriteLine(string.Format("{0} {1}", msg, ex.Message));
                throw new ApplicationException(msg, ex);
            }
            return result;
        }


        private List<ReflectorOrRepeater> GetDStarInfoReflectors()
        {
            string htmlReflector = string.Empty;

            using (HttpClient client = new HttpClient())
            {
                client.BaseAddress = new Uri("http://apps.dstarinfo.com");
                HttpRequestMessage request = new HttpRequestMessage(HttpMethod.Get, "reflectors.aspx");
                Task<HttpResponseMessage> resp = client.SendAsync(request);
                resp.Wait();
                Task<Stream> strm = resp.Result.Content.ReadAsStreamAsync();
                strm.Wait();
                using (StreamReader sr = new StreamReader(strm.Result, System.Text.Encoding.UTF8))
                {
                    htmlReflector = sr.ReadToEnd();
                }
            }

            List<ReflectorOrRepeater> refs = new List<ReflectorOrRepeater>();

            // parsing HTML table of reflector information
            // start by finding start of reflector name by prefix
            // find next span tag which is description
            // eliminate any link tag inside description span tag

            // find the start of reflector NAME
            int ofs = htmlReflector.IndexOf(">REF");
            while (ofs > 0)
            {
                // find end of name span
                int ofs2 = htmlReflector.IndexOf("</span>", ofs + 1);
                if (ofs2 > 0)
                {
                    ReflectorOrRepeater rf = new ReflectorOrRepeater();
                    string sref = htmlReflector.Substring(ofs + 1, ofs2 - ofs - 1);
                    rf.Name = sref;
                    // find desc span
                    int ofs3 = htmlReflector.IndexOf("<span", ofs2);
                    if (ofs > 0)
                    {
                        // find end of desc open span tag
                        ofs3 = htmlReflector.IndexOf(">", ofs3 + 1);
                        // find desc close span tag
                        int ofs4 = htmlReflector.IndexOf("</span>", ofs3 + 1);
                        // find possible surrounding link tag
                        int ofs5 = htmlReflector.IndexOf("<a href", ofs3 + 1);
                        // there is an link tag    
                        if (ofs5 >= 0 && ofs5 < ofs4)
                        {
                            ofs3 = htmlReflector.IndexOf(">", ofs5 + 1);
                            ofs4 = htmlReflector.IndexOf("</a>", ofs3 + 1);
                        }
                        string desc = htmlReflector.Substring(ofs3 + 1, ofs4 - ofs3 - 1);
                        rf.Desc = desc;
                        refs.Add(rf);
                        // find next
                        ofs = htmlReflector.IndexOf(">REF", ofs3);
                        continue;
                    }
                }
                break;
            }

            return refs;
        }

        private void SaveRepeaters(List<ReflectorOrRepeater> refs)
        {
            if (refs == null || refs.Count == 0)
                return;
            string fname = GetBaseAppDataPath("Repeaters.txt");
            using (StreamWriter sw = new StreamWriter(fname))
            {
                foreach (ReflectorOrRepeater rf in refs)
                {
                    sw.WriteLine("{0}|{1}", rf.Name, rf.Desc);
                }
                sw.Close();
            }
        }

        private void SaveReflectors(List<ReflectorOrRepeater> refs)
        {
            if (refs == null || refs.Count == 0)
                return;
            string fname = GetBaseAppDataPath("Reflectors.txt");
            using (StreamWriter sw = new StreamWriter(fname))
            {
                foreach (ReflectorOrRepeater rf in refs)
                {
                    sw.WriteLine("{0}|{1}", rf.Name, rf.Desc);
                }
                sw.Close();
            }
        }

        private void LoadRepeaters()
        {
            try
            {
                string fname = GetBaseAppDataPath("Repeaters.txt");
                List<ReflectorOrRepeater> refs = new List<ReflectorOrRepeater>();
                using (StreamReader sr = new StreamReader(fname))
                {
                    while (!sr.EndOfStream)
                    {
                        string line = sr.ReadLine();
                        string[] vals = line.Split('|');
                        if (vals.Length == 2)
                        {
                            refs.Add(new ReflectorOrRepeater() { Name = vals[0], Desc = vals[1] });
                        }
                    }
                    sr.Close();
                }
                Repeaters_ = refs;
            }
            catch { }
        }


        private void LoadReflectors()
        {
            try
            {
                string fname = GetBaseAppDataPath("Reflectors.txt");
                List<ReflectorOrRepeater> refs = new List<ReflectorOrRepeater>();
                using (StreamReader sr = new StreamReader(fname))
                {
                    while (!sr.EndOfStream)
                    {
                        string line = sr.ReadLine();
                        string[] vals = line.Split('|');
                        if (vals.Length == 2)
                        {
                            refs.Add(new ReflectorOrRepeater() { Name = vals[0], Desc = vals[1] });
                        }
                    }
                    sr.Close();
                }
                Reflectors_ = refs;
            }
            catch { }
        }


        private static string rootappdata;

        private static string GetBaseAppDataPath(string fname)
        {
            if (rootappdata == null)
            {
                string basepath = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);

                AssemblyInfo ai = new AssemblyInfo(Assembly.GetAssembly(typeof(MainForm)));

                string mybasepath = System.IO.Path.Combine(basepath, ai.Company.Replace(' ', '_'), ai.ProductTitle.Replace(' ', '_'));
                if (!System.IO.Directory.Exists(mybasepath))
                    System.IO.Directory.CreateDirectory(mybasepath);

                rootappdata = mybasepath;
            }
            return System.IO.Path.Combine(rootappdata, fname);
        }
    }


    public class ReflectorOrRepeater
    {
        public string Name { get; set; }
        public string Desc { get; set; }
    }

}
