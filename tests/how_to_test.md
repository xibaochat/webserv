# CORRECTION

## HTTP REDIRECTION

1. Go to redirectable url:
http://localhost:8080/images/d5v.gif

## Static upload

1. Create the destination directory
```
mkdir -p /tmp/upload_file/
```
2. Go to http://localhost:9000/html/static_upload.html
3. Select from file in `./tests/files/`
4. Click upload
5. Validate the file was upload in `/tmp/upload_file/`

## Static upload with multiple fields

1. Go to http://localhost:9000/html/multi_fields_input_static_upload.html
2. Fill the all the field
3. Select a file from `./tests/files/`
4. Click upload
5. Validate the file was upload in `/tmp/upload_file/`

## CGI Upload

1. Go to http://localhost:9000/html/cgi_upload.html
2. Fill the `File Description` field
3. Select a file from `./tests/files/`
4. Click upload
5. Validate the response is dynamically generated based on your inputs
6. Validate the file was upload in `/tmp/upload_file/`

## CGI Upload with multiple fields

1. Go to http://localhost:9000/html/cgi_upload_with_multi_fields.html
2. Fill all the fields
3. Select a file from `./tests/files/`
4. Click upload
5. Validate the response is dynamically generated based on your inputs
6. Validate the file was upload in `/tmp/upload_file/`

## Server Name

1. `Curl` the webserver using a domain name
```sh
curl --resolve bao.com:7000:127.0.0.1 http://bao.com:7000/html/server_name.html
```
2. Validate the reponse is the content of the file `./html/server_name.html`

## AllowMethods

1. Check the config file and validate the server `bao.com:7000` only accept `GET` requests
2. `Curl` the webserver with a `POST` request
```sh
curl --resolve bao.com:7000:127.0.0.1 http://bao.com:7000/html/allowMethods.html -X POST
```
3. Validate the answer being
```
Error 405 Method Not Allowed
```
4. Validate a `GET` request is valid
```sh
curl --resolve bao.com:7000:127.0.0.1 http://bao.com:7000/html/allowMethods.html -X POST
```

## Ports

1. Check all ports in the config file
2. `Curl` all ports and validate receiving a valid answer from the open ports
```sh
curl http://localhost:8000/html/ports.html
curl http://localhost:8080/html/ports.html
curl http://localhost:8088/html/ports.html
curl http://localhost:7000/html/ports.html
curl http://localhost:9000/html/ports.html
curl http://localhost:9090/html/ports.html
curl http://localhost:9092/html/ports.html
curl http://localhost:9093/html/ports.html
```
```html
<html>
  <body>
	<div id="awesome_test">
	  You successfuly tested ports.
	</div>
  </body>
</html>
```
3. `Curl` ports non-present in the config file and validate they aren't open
```
curl http://localhost:90/html/ports.html
curl http://localhost:5000/html/ports.html
curl http://localhost:9999/html/ports.html
curl http://localhost:9898/html/ports.html
```

```
curl: (7) Failed to connect to localhost port XXXX: Connection refused
```

## Root

1. `Curl` the following url and validate the answer
```sh
curl http://localhost:6670/root/root_test.html
```
2. Validate the response
```html
<html>
  <body>
	<div id="awesome_test">
	  You successfuly tested the root feature.
	</div>
  </body>
</html>
```

## Error Page

1. Go to http://localhost:9000/file_does_not_exist.html
2. Validate you can see a cute and bored little cat with a 404 status code.
3. Try to pet the cat.
4. `Curl` the server without error page define while using a forbidden method
```
curl -X POST http://localhost:6670
```
5. Validate the response is an `error 403` as follow
```
Error 403 Forbidden
```

## AutoIndexing

1. Check that the `autoindex` is **enabled** for the server `localhost:6670` on the location `/`
2. Go to http://localhost:6670
3. Validate see current directory is displayed like such
```
Index of .
../
src/                                                8-Mar-2022 20:09:26                   -
conf/                                               8-Mar-2022 20:15:05                   -
python_files/                                       8-Mar-2022 17:17:57                   -
includes/                                           8-Mar-2022 19:53:25                   -
tests/                                              8-Mar-2022 20:15:29                   -
html/                                               8-Mar-2022 20:04:56                   -
todo.md                                             8-Mar-2022 17:17:57                3384
index.html                                          8-Mar-2022 17:17:57                5452
Makefile                                            8-Mar-2022 17:17:57                1032
webserv                                             8-Mar-2022 20:08:53             1481048
```
4. Check that the `autoindex` is **disabled** for the server `localhost:6670` on the location `/init.d`
5. Go to http://localhost:6670/init.d/
6. Validate the response is as such
```
Error 403 Forbidden
```
