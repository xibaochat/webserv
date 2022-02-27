#!/usr/bin/env python
import cgi, os
import cgitb
# This activates a special handler
# that will display detailed reports
# in the web browser if any errors occur
cgitb.enable()

data = cgi.FieldStorage()
keys = data.keys()

if 'description' in keys:
  description = data["description"].value.strip()
else:
  description = ""
if not description:
  description = "empty"

if (len(data) == 0):
	print('Content-Type:text/html\r\n\r\n')
	print('<html>\n')
	print('<head>\n')
	print('<title>ERROR - First CGI Program</title>\n')
	print('</head>\n')
	print('<body>\n')
	print('<h2>Error' + ' This is my first CGI program</h2>\n')
	print('</body>\n')
	print('</html>\n')
else:
	fileitem = data['upload_file']
	upload_dir = os.getenv("UPLOAD_DIR")

	if not os.path.exists(upload_dir):
		os.makedirs(upload_dir)

	if fileitem.filename:
		fn = os.path.basename(fileitem.filename.replace("\\", "/" ))
		open(upload_dir + fn, 'wb').write(fileitem.file.read())
		message = 'The file "' + upload_dir + fn + '" was uploaded successfully'
	else:
		message = 'No file was uploaded'

	print('Content-Type:text/html\r\n\r\n')
	print('<html>\n')
	print('<head>\n')
	print('<title>Hello World - First CGI Program</title>\n')
	print('</head>\n')
	print('<body>\n')
	print('<h2>This is my first CGI program</h2>\n')
	print('<h2>' + message + '</h2>\n')
	print('<h2>Description of the file is ' + description + '</h2>\n')
	print('</body>\n')
	print('</html>\n')
