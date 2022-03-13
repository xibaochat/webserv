#!/usr/bin/env python
import cgi
import os
import cgitb

# This activates a special handler
# that will display detailed reports
# in the web browser if any errors occur
cgitb.enable(display=0)

data = cgi.FieldStorage()
keys = data.keys()

if 'description' in keys:
  description = data["description"].value.strip()
else:
  description = ""
if not description:
  description = "empty"

if (len(data) == 0):
  raise Exception('No data provided')
else:
	fileitem = data['upload_file']
	upload_dir = os.getenv("UPLOAD_DIR")
	acceptable_upload = os.getenv("ACCEPTABLE_UPLOAD")

	if fileitem.filename:
		if acceptable_upload == "on":
			if not os.path.exists(upload_dir):
				os.makedirs(upload_dir)
			fn = os.path.basename(fileitem.filename.replace("\\", "/" ))
			open(upload_dir + fn, 'wb').write(fileitem.file.read())
			message = 'The file "' + upload_dir + fn + '" was uploaded successfully'
		else:
			message = 'Not allow to upload file'
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
