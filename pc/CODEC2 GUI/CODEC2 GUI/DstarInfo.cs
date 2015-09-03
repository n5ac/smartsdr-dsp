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
using System.Xml;

namespace CODEC2_GUI
{
    public class DstarInfo
    {
        public DstarInfo()
        {

        }

        private static HttpClient _client;
        private static HttpClient client
        {
            get
            {
                if (_client == null)
                    _client = new HttpClient() { BaseAddress = new Uri("http://apps.dstarinfo.com") };
                return _client;
            }
        }
        private static string eventvalidation;
        private static string viewstate;
        private static string viewstategenerator;


        private List<ReflectorOrRepeater> Repeaters_;
        public List<ReflectorOrRepeater> Repeaters
        {
            get
            {
                if (Repeaters_ == null)
                {
                    LoadRepeaters();
                    if (Repeaters_ == null)
                        Repeaters_ = new List<ReflectorOrRepeater>();

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

        public bool FetchAndSaveRepeaterList(string area)
        {
            bool result = false;
            try
            {
                List<ReflectorOrRepeater> rptrs = GetDStarRepeaterByArea(area);
                if (rptrs != null && rptrs.Count > 0)
                {
                    SaveRepeaters(rptrs);
                    Repeaters_ = rptrs;
                    result = true;
                }
            }
            catch (Exception ex)
            {
                string msg = "Fetch and save Repeater list failed!";
                System.Diagnostics.Debug.WriteLine(string.Format("{0} {1}", msg, ex.Message));
                throw new ApplicationException(msg, ex);
            }
            return result;
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

        public List<string> GetDStarRepeaterAreas()
        {
            List<string> rptareas = new List<string>();
            string htmlAreas = string.Empty;

            try
            {
                if (client != null)
                {
                    HttpRequestMessage request = new HttpRequestMessage(HttpMethod.Get, "D-STAR_Repeater_List.aspx");
                    Task<HttpResponseMessage> resp = client.SendAsync(request);
                    resp.Wait();
                    resp.Result.EnsureSuccessStatusCode();

                    Task<Stream> strm = resp.Result.Content.ReadAsStreamAsync();
                    strm.Wait();
                    using (StreamReader sr = new StreamReader(strm.Result, System.Text.Encoding.UTF8))
                    {
                        htmlAreas = sr.ReadToEnd();
                    }
                }

                viewstate = getHiddenValues(htmlAreas, "id=\"__VIEWSTATE\" value=\"");
                viewstategenerator = getHiddenValues(htmlAreas, "id=\"__VIEWSTATEGENERATOR\" value=\"");
                eventvalidation = getHiddenValues(htmlAreas, "id=\"__EVENTVALIDATION\" value=\"");

                // parsing HTML select of areas
                // start by finding "<select name=\"Countries1\""

                int ofs = htmlAreas.IndexOf("<select name=\"Countries1\"");
                if (ofs > 0)
                {
                    // find end of select
                    int ofs2 = htmlAreas.IndexOf("</select>", ofs + 23);
                    if (ofs2 > 0)
                    {
                        string selVals = htmlAreas.Substring(ofs, ofs2 - ofs);
                        int ofs3 = 0;
                        int ofs4 = 0;
                        while (ofs4 < selVals.Length)
                        {
                            ofs4 = selVals.IndexOf("<option ", ofs3);
                            if (ofs4 > 0)
                            {
                                ofs4 += 8;
                                ofs3 = selVals.IndexOf("value=\"", ofs4);
                                if (ofs3 > 0)
                                {
                                    ofs3 += 7;
                                    ofs4 = selVals.IndexOf("\"", ofs3);
                                    string val = selVals.Substring(ofs3, ofs4 - ofs3);
                                    val = System.Net.WebUtility.HtmlDecode(val);
                                    ofs3 = ofs4;
                                    rptareas.Add(val);
                                }
                            }
                            else
                                break;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                string msg = "Fetch Repeater Areas list failed!";
                System.Diagnostics.Debug.WriteLine(string.Format("{0} {1}", msg, ex.Message));
                throw new ApplicationException(msg, ex);
            }

            return rptareas;
        }

        private string getHiddenValues(string str, string prefix)
        {
            int ofs = str.IndexOf(prefix);
            if (ofs > 0)
            {
                ofs += prefix.Length;
                int ofs2 = str.IndexOf("\"", ofs);
                if (ofs2 > ofs)
                {
                    return str.Substring(ofs, ofs2 - ofs);
                }
            }
            return null;
        }


        public List<ReflectorOrRepeater> GetDStarRepeaterByArea(string area)
        {
            string htmlRptrs = string.Empty;

            if (client != null)
            {
                HttpRequestMessage request = new HttpRequestMessage(HttpMethod.Post, "D-STAR_Repeater_List.aspx");
                request.Content = new FormUrlEncodedContent(new [] {
                    new KeyValuePair<string, string>("__EVENTARGUMENT", string.Empty),
                    new KeyValuePair<string, string>("__EVENTTARGET", "Countries1"),
                    new KeyValuePair<string, string>("__EVENTVALIDATION", eventvalidation ?? string.Empty),
                    new KeyValuePair<string, string>("__LASTFOCUS", string.Empty),
                    new KeyValuePair<string, string>("__VIEWSTATE", viewstate ?? string.Empty),
                    new KeyValuePair<string, string>("__VIEWSTATEENCRYPTED", string.Empty),
                    new KeyValuePair<string, string>("__VIEWSTATEGENERATOR", viewstategenerator ?? string.Empty),
                    new KeyValuePair<string, string>("Countries1", System.Net.WebUtility.HtmlEncode(area)),
                });

                Task<HttpResponseMessage> resp = client.SendAsync(request);
                resp.Wait();
                resp.Result.EnsureSuccessStatusCode();
                Task<Stream> strm = resp.Result.Content.ReadAsStreamAsync();
                strm.Wait();
                using (StreamReader sr = new StreamReader(strm.Result, System.Text.Encoding.UTF8))
                {
                    htmlRptrs = sr.ReadToEnd();
                }
            }

            List<ReflectorOrRepeater> rptr = new List<ReflectorOrRepeater>();

            // parsing HTML table of repeaters


            // find the start of table of repeaters
            int ofs = htmlRptrs.IndexOf("<table id=\"ListView1_itemPlaceholderContainer\"");
            if (ofs > 0)
            {
                // find end of table
                int ofs2 = htmlRptrs.IndexOf("</table>", ofs + 46);
                if (ofs2 > 0)
                {
                    string selVals = htmlRptrs.Substring(ofs, ofs2 - ofs + 8);

                    // fix improper ampersand html encoding
                    string[] doms = System.Text.RegularExpressions.Regex.Split(selVals, @"&(?!\w{1,3};)");
                    selVals = string.Join("&amp;", doms);

                    XmlDocument xdoc = new XmlDocument();
                    xdoc.LoadXml("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n" + selVals);

                    XmlNodeList xnl = xdoc.SelectNodes("//tr");
                    foreach (XmlNode xn in xnl)
                    {
                        ReflectorOrRepeater rr = new ReflectorOrRepeater();
                        string city = null;
                        string cty_state = null;
                        string modC = null;
                        string modB = null;
                        string modA = null;
                        XmlNodeList dnl = xn.SelectNodes("descendant::span");
                        foreach (XmlNode dn in dnl)
                        {
                            foreach (XmlAttribute xa in dn.Attributes)
                            {
                                if (xa.Name == "id")
                                {
                                    if (xa.Value.StartsWith("ListView1_CityLabel"))
                                        city = dn.InnerText;
                                    else if (xa.Value.StartsWith("ListView1_CountryStateLabel"))
                                        cty_state = dn.InnerText;
                                    else if (xa.Value.StartsWith("ListView1_C_ModLabel"))
                                        modC = dn.InnerText;
                                    else if (xa.Value.StartsWith("ListView1_B_ModLabel"))
                                        modB = dn.InnerText;
                                    else if (xa.Value.StartsWith("ListView1_A_ModLabel"))
                                        modA = dn.InnerText;
                                }
                            }
                        }

                        XmlNode fn = xn.FirstChild; // td
                        if (fn != null)
                            fn = fn.FirstChild; // a
                        if (fn != null && fn.Attributes != null)
                        {
                            XmlAttribute xlnk = fn.Attributes["href"];
                            if (xlnk != null)
                            {
                                string lnk = xlnk.Value;
                                if (!string.IsNullOrEmpty(lnk))
                                {
                                    int idx = lnk.IndexOf("?Repeater=");
                                    if (idx > 0)
                                    {
                                        string cs = System.Net.WebUtility.UrlDecode(lnk.Substring(idx + 10));
                                        rr.Name = cs;
                                        rr.Desc = string.Format("{0}|{1}|C:{2}|B:{3}|A:{4}", city ?? string.Empty, cty_state ?? string.Empty, modC ?? string.Empty, modB ?? string.Empty, modA ?? string.Empty);
                                        rptr.Add(rr);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            return rptr;
        }

        private List<ReflectorOrRepeater> GetDStarInfoReflectors()
        {
            string htmlReflector = string.Empty;

            if (client != null)
            {
                HttpRequestMessage request = new HttpRequestMessage(HttpMethod.Get, "reflectors.aspx");
                Task<HttpResponseMessage> resp = client.SendAsync(request);
                resp.Wait();
                resp.Result.EnsureSuccessStatusCode();
                Task<Stream> strm = resp.Result.Content.ReadAsStreamAsync();
                strm.Wait();
                using (StreamReader sr = new StreamReader(strm.Result, System.Text.Encoding.UTF8))
                {
                    htmlReflector = sr.ReadToEnd();
                }
            }

            List<ReflectorOrRepeater> refs = new List<ReflectorOrRepeater>();


            int ofs = htmlReflector.IndexOf("<table>");
            if (ofs > 0)
                ofs = htmlReflector.IndexOf("<table", ofs + 7);
            if (ofs > 0)
            {
                // find end of name span
                int ofs2 = htmlReflector.IndexOf("</table>", ofs + 7);
                if (ofs2 > 0)
                {
                    string dom = htmlReflector.Substring(ofs, ofs2 - ofs + 8)
                        .Replace("target=_blank", string.Empty); // format issue for xml parse

                    // fix improper ampersand html encoding
                    string[] doms = System.Text.RegularExpressions.Regex.Split(dom, @"&(?!\w{1,3};)");
                    dom = string.Join("&amp;", doms);

                    XmlDocument xdoc = new XmlDocument();
                    xdoc.LoadXml("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n" + dom);

                    XmlNodeList xnl = xdoc.SelectNodes("//tr");
                    foreach (XmlNode xn in xnl)
                    {
                        ReflectorOrRepeater rr = new ReflectorOrRepeater();
                        string refl = null;
                        string loc = null;
                        XmlNodeList dnl = xn.SelectNodes("descendant::span");
                        foreach (XmlNode dn in dnl)
                        {
                            foreach (XmlAttribute xa in dn.Attributes)
                            {
                                if (xa.Name == "id")
                                {
                                    if (xa.Value.StartsWith("ListView1_ReflectorLabel"))
                                        refl = dn.InnerText;
                                    else if (xa.Value.StartsWith("ListView1_LocationLabel"))
                                        loc = dn.InnerText;
                                }
                            }
                        }
                        if (!string.IsNullOrEmpty(refl))
                        {
                            rr.Name = refl;
                            rr.Desc = loc ?? string.Empty;
                            refs.Add(rr);
                        }
                    }
                }
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
                        if (vals.Length >= 2)
                        {
                            refs.Add(new ReflectorOrRepeater() { Name = vals[0], Desc = string.Join("|", vals, 1, vals.Length - 1) });
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

                string mybasepath = System.IO.Path.Combine(basepath, ai.Company, ai.ProductTitle);
                if (!System.IO.Directory.Exists(mybasepath))
                    System.IO.Directory.CreateDirectory(mybasepath);

                rootappdata = mybasepath;
            }
            return System.IO.Path.Combine(rootappdata, fname);
        }
    }



}
