 /**
 * This file is part of nmealib.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "nmealib.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


/**
 * Calculate crc control sum of a string.
 * If the string starts with a '$' then that character is skipped as per
 * the NMEA spec.
 *
 * @param s the string
 * @param len the length of the string
 * @return the crc
 */
int nmea_calc_crc(const char *s, const int len) {
  int chksum = 0;
  int it = 0;

  if (s[it] == '$')
    it++;

  for (; it < len; it++)
    chksum ^= (int) s[it];

  return chksum;
}




bool attr_GGA_handler_double(u8 *attr, unsigned int len, u8 index, u8 **data){
  if(len == 0)
    return true;
  double *res = (double *)*data;
  *res = strtod((char const *)attr, NULL);
  res ++;
  *data = (u8 *)res;
  return true;
}
/**
 * Validate north/south or east/west and uppercase it.
 * Expects:
 * <pre>
 * 3. N or S (North or South)
 * 4. Longitude
 * 5. E or W (East or West) 
 *   c in { n, N, s, S } (for north/south)
 *   c in { e, E, w, W } (for east/west)
 * </pre>
 *
 * @param c a pointer to the character. The character will be converted to uppercase.
 * @param ns true: evaluate north/south, false: evaluate east/west
 * @return true when valid, false otherwise
 */
bool attr_GGA_handler_NSEW(u8 *attr, unsigned int len, u8 index, u8 **data){
  u8 *ret = *data;
  u8 dir = toupper(*attr);
  if(len == 0)
    return true;

  else if (index == 3) {
    if ((dir == 'N') || (dir == 'S')) {
      *ret=dir;
      ret ++;
      *data = ret;
    }
      return true;
  } 
  else  if(index == 5){
    if ((dir == 'E') || (dir == 'W')) {
      *ret=dir;
      ret ++;
      *data = ret;
    }
    return true;
  }

  return true;
}


bool attr_GGA_handler_header(u8 *attr, unsigned int len, u8 index, u8 **data){
  u8 header[] = "$GPGGA";
  if(0 != memcmp(attr, header, strlen((char const *)header)))
    return false;
  return true;
}
bool attr_GGA_handler_none(u8 *attr, unsigned int len, u8 index, u8 **data){
  return true;
}





/**
* 
* === GGA - Global Positioning System Fix Data ===
* 
* Time, Position and fix related data for a GPS receiver.
* 
* ------------------------------------------------------------------------------
*                                                       11
*         1         2       3 4        5 6 7  8   9  10 |  12 13  14   15
*         |         |       | |        | | |  |   |   | |   | |   |    |
*  $--GGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh<CR><LF>
* ------------------------------------------------------------------------------
* 
* Field Number: 
* 
* 1. Universal Time Coordinated (UTC)
* 2. Latitude
* 3. N or S (North or South)
* 4. Longitude
* 5. E or W (East or West)
* 6. GPS Quality Indicator,
*      - 0 - fix not available,
*      - 1 - GPS fix,
*      - 2 - Differential GPS fix
*            (values above 2 are 2.3 features)
*      - 3 = PPS fix
*      - 4 = Real Time Kinematic
*      - 5 = Float RTK
*      - 6 = estimated (dead reckoning)
*      - 7 = Manual input mode
*      - 8 = Simulation mode
* 7. Number of satellites in view, 00 - 12
* 8. Horizontal Dilution of precision (meters)
* 9. Antenna Altitude above/below mean-sea-level (geoid) (in meters)
* 10. Units of antenna altitude, meters
* 11. Geoidal separation, the difference between the WGS-84 earth
*      ellipsoid and mean-sea-level (geoid), "-" means mean-sea-level
*      below ellipsoid
* 12. Units of geoidal separation, meters
* 13. Age of differential GPS data, time in seconds since last SC104
*      type 1 or 9 update, null field when DGPS is not used
* 14. Differential reference station ID, 0000-1023
* 15. Checksum
**/

attr_GGA_handler attr_GGA_handler_list[]={
  attr_GGA_handler_header,    //0 header
  attr_GGA_handler_double,      //1 utc
  attr_GGA_handler_double,      //2 Lat
  attr_GGA_handler_NSEW,      //3 N S
  attr_GGA_handler_double,      //4 LON
  attr_GGA_handler_NSEW,      //5 E W
  attr_GGA_handler_none,      //6 available
  attr_GGA_handler_none,      //7 satellite number
  attr_GGA_handler_none,      //8 precision of hori 
  attr_GGA_handler_none,      //9
  attr_GGA_handler_none,      //10

};
bool parse_attr_GGA (u8 *attr, unsigned int len, u8 index, u8 **data){
  if(index >= sizeof(attr_GGA_handler_list)/sizeof(attr_GGA_handler))
    return false;
  return attr_GGA_handler_list[index](attr, len, index, data);
}


bool nmea_parse_GGA(u8 *s, const int len, bool has_checksum, u8 *pack){

  u8 attrIndex = 0;
  u8 * p = (u8 *)s;
  u8 * attr = (u8 *)s;
  u8 * data = pack;

  ((resolved_data_t *)data)->type = RESOLVED_GGA_DATA_TYPE; 
  ((resolved_data_t *)data)->len = 0;     // length
  data =  ((resolved_data_t *)data)->data;

  while (p && (p-(u8 *)s) < len 
    && p[0] != '\0' && p[0] != '\r' && p[0] != '\n'
    && attrIndex < sizeof(attr_GGA_handler_list)
    ){
        
    if(p[0] == ','){
      if (!parse_attr_GGA(attr, p-attr, attrIndex, &data))
        break;
      p[0] = '\0';
      p++;
      attr=p;    
      attrIndex ++;
    }else
      p++;
  }

  if(data <= ((resolved_data_t *)pack)->data){ 
      return false; // <4 haven't get datas
  }else{
    ((resolved_data_t *)pack)->len = data -  ((resolved_data_t *)pack)->data;
    return true;
  }
  
}


