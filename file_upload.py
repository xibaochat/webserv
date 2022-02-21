#!/usr/bin/env python
import cgi, os
import cgitb
# This activates a special handler
# that will display detailed reports
# in the web browser if any errors occur
cgitb.enable()

data = cgi.FieldStorage()

fileitem = data['upload_file']
print(fileitem.filename)

if fileitem.filename:
	fn = os.path.basename(fileitem.filename.replace("\\", "/" ))
	open('/tmp/' + fn, 'wb').write(fileitem.file.read())
	message = 'The file "' + fn + '" was uploaded successfully'

else:
	message = 'No file was uploaded'

print('Content-Type:text/html\r\n\r\n')
print('<html>\n')
print('<head>\n')
print('<title>Hello World - First CGI Program</title>\n')
print('</head>\n')
print('<body>\n')
print('<h2>haha' + message + ' This is my first CGI program</h2>\n')
print('</body>\n')
print('</html>\n')
