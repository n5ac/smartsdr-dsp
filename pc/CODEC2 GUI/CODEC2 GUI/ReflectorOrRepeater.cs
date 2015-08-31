/*******************************************************************************
 * ReflectorOrRepeater.cs									  			
 *
 * 	DStar Repeater / Reflector Model
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

namespace CODEC2_GUI
{
    public class ReflectorOrRepeater
    {
        public string Name { get; set; }
        public string Desc { get; set; }
        public double Frequency { get; set; }
        public double Offset { get; set; }
        public string RepeaterName
        {
            get
            {
                if (Frequency == 0)
                    return Name;
                return string.Format("{0}~{1}~{2}", Name ?? string.Empty, Frequency, Offset);
            }
        }
    }
}
