# CORRECTION

## Welcome

http://localhost:9000

## Configuration path

1. Launch the server without argument
```
./webserv
```
2. Validate the default file `conf/default.conf` is used
3. Launch the server with a conf file as argument and validate the use of it
```
cp conf/default.conf new_conf.conf
./webserv new_conf.conf
```
4. Launch the server with a non existant file as argument
```
./webserv nope
```
5. Validate that the server is not launching
```
Cannot open config file
```

## HTTP REDIRECTION

1. Go to redirectable url:
http://localhost:8080/images/d5v.gif
http://localhost:8080//life/pets/a34774573/girl-cat-names/
## Default HTML index file

1. Go to http://localhost:4670/html/
2. Donc get caught by the ghost!

## Static upload

1. Go to http://localhost:9000/html/static_upload.html
2. Select from file in `./tests/files/`
3. Click upload
4. Validate the file was upload in `/tmp/upload_file/`

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

## Delete file/dir

```
mkdir -p a
curl -X DELETE http://localhost:8000/a
```
```
[...]
< HTTP/1.1 200 OK
[...]
```
```
ls ./a
ls: cannot access './a': No such file or directory
```

## Stress tests

### Installation

```sh
curl -O http://download.joedog.org/siege/siege-latest.tar.gz
tar xzvpf siege-latest.tar.gz
cd `ls -1d */ | grep siege- | sort -r | head -1`
./configure
make
sudo make install
```

### Test

1. Try 'siege' on an empty page: `siege -b http://localhost:8080/html/empty.html`
2. Try 'siege' on a page with content: `siege -b http://localhost:8080/html/cute_cat.html`



## Server Name

1. `Curl` the webserver using a domain name
```sh
curl --resolve bao.com:7000:127.0.0.1 http://bao.com:7000/html/server_name.html
	```
2. Validate the reponse is the content of the file `./html/server_name.html`

```
curl --resolve localhost:7066:127.0.0.1 http://localhost:7066/html/server_name.html
```
should show status code 405, because we define only post
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
5. Validate the response is an `error 405` as follow
```
Error 405 Method Not Allowed
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

## Client Max Body Size

1. Validate for the server `localhost:7066` that the `client_max_body_size` is set to 1
2. `Curl` the server
```
curl -X POST http://localhost:7066/html/max_body_size.html --data "{\"kitten\": 66666}"
```
3. Validate the reponse is an error 413
```
Error 413 Request Entity Too Large%
```
4. Validate for the server `localhost:7067` that the `client_max_body_size` is set to 100
5. `Curl` the server
```
curl -X POST http://localhost:7067/html/max_body_size.html --data "{\"kitten\": 66666}"
```
6. Validate the reponse is a valid html
```html
<html>
  <body>
	<div id="awesome_test">
	  You successfuly tested the max_client_body_size.
	</div>
  </body>
</html>
```

## Status code

### Using Curl

1. Basic `Curl` -> 200
```
curl -v http://localhost:8080/html/permission_tests.html
```
```
[...]
< HTTP/1.1 200 OK
[...]
```
2. No `READ` permission
```
chmod -r ./html/permission_tests.html
curl -v http://localhost:8080/html/permission_tests.html
```
```
[...]
< HTTP/1.1 403 Forbidden
[...]
```
3. Empty response
```
curl -v http://localhost:5566/html/empty.html
```
```
[...]
< HTTP/1.1 204 No Content
[...]
```
4. Not implemented
```
curl -v http://localhost:5566/html/not_implemented.php
```
```
[...]
< HTTP/1.1 501 Not Implemented
[...]
```
4. Method not allowed
```
curl -X POST http://localhost:5566
```
```
[...]
< HTTP/1.1 405 Method Not Allowed
[...]
```
5. Not Found
```
curl -v http://localhost:5566/html/does_not_exist.html
```
```
[...]
< HTTP/1.1 404 Not Found
[...]
```
6. Internal Server Error (CGI)
```
curl -v http://localhost:5566/python_files/file_upload.py
```
```
[...]
< HTTP/1.1 500 Internal Server Error
[...]
etant donne dans le fichier , on a fait:  raise Exception('No data provided')

```
