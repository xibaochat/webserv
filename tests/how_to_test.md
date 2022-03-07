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

1. Create the destination directory
```
mkdir -p /tmp/upload_file/
```
2. Go to http://localhost:9000/html/multi_fields_input_static_upload.html
3. Fill the all the field
4. Select a file from `./tests/files/`
5. Click upload
6. Validate the file was upload in `/tmp/upload_file/`

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
