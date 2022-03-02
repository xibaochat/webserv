#!/usr/bin/env python
import cgi, cgitb
cgitb.enable()
input_data = cgi.FieldStorage()
keys = input_data.keys()

if 'first_name' in keys:
  first_name = input_data["first_name"].value.strip()
else:
  first_name = ""
if not first_name:
  first_name = "empty"

if 'last_name' in keys:
  last_name = input_data["last_name"].value.strip()
else:
  last_name = ""
if not last_name:
  last_name = "empty"

print('Content-Type:text/html\r\n\r\n')
print('<html>\n')
print('<head>\n')
print('<title>Hello World - First CGI Program</title>\n')
print('</head>\n')
print('<body>\n')
print('<h2>Hello World! This is my first CGI program</h2>\n')
print('<h2>My firstname is ' + first_name + '</h2>\n')
print('<h2>My lastname is ' + last_name + '</h2>\n')
print('</body>\n')
print('</html>\n')
