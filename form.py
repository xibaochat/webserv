#!/usr/bin/env python
import cgi, cgitb
cgitb.enable()
input_data = cgi.FieldStorage()
# name = input_data.getvalue('fname')
# print("Name of the user is:",name)

first_name = input_data["first_name"].value
last_name = input_data["last_name"].value

#print(first_name)
#print(last_name)

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