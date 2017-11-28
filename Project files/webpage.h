

#include "stm32f10x.h"

#ifndef _WEBPAGE
#define _WEBPAGE

const uint8_t webTemplate[1108+1] = "<!DOCTYPE html> <html> <head> <meta http-equiv=\"refresh\" content=\"15\"> <style> table, th, td {  border: 1px solid black; border-collapse: collapse; } th, td { padding: 5px; text-align: left; } </style> </head> <body> <table style=\"width:60%\"> <tr> <th colspan=\"2\">Senzor 1 - Temperature [°C]</th> <th colspan=\"2\">Senzor 2 - Humidity [%]</th> <th colspan=\"2\">Senzor 3 - Temperature [°C]</th> <th colspan=\"2\">Senzor 4 - Temperature [°C]</th> </tr> <tr> <td>12</td> <td>10/1/2017 10:10</td> <td>70</td> <td>10/1/2017 10:10</td> <td>12</td> <td>10/1/2017 10:10</td> <td>12</td> <td>10/1/2017 10:10</td> </tr> <tr> <td>12</td> <td>10/1/2017 10:10</td> <td>70</td> <td>10/1/2017 10:10</td> <td>12</td> <td>10/1/2017 10:10</td> <td>12</td> <td>10/1/2017 10:10</td> </tr> <tr> <td>12</td> <td>10/1/2017 10:10</td> <td>70</td> <td>10/1/2017 10:10</td> <td>12</td> <td>10/1/2017 10:10</td> <td>12</td> <td>10/1/2017 10:10</td> </tr> </table><p>Current Time: 16:10</p> <form> <button type=\"submit\" formmethod=\"post\">Set time</button> Time (dd.mm.yyyy hh:mm): <input type=\"text\" name=\"datetime\"> </form> </body> </html>";

#endif