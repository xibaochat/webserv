import cgi
import cgitb
# This activates a special handler
# that will display detailed reports
# in the web browser if any errors occur
cgitb.enable()

data = cgi.FieldStorage()
print(data)

print('Content-Type:text/html\r\n\r\n')
print('<html>\n')
print('<head>\n')
print('<title>Hello World - First CGI Program</title>\n')
print('</head>\n')
print('<body>\n')
print('<h2>haha' + data["abc"].value + ' This is my first CGI program</h2>\n')
print('</body>\n')
print('</html>\n')

