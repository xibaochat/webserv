# Subject

<!-- * define `browser` of the team -->
* (Maobe & Olano) add chunk management:
    -> (Olana) chunked request (when client upload a file)
		-> will probably need to use `test_this/client.cpp` to emulate chunked request
    -> (Maobe) chunked response (when client download a file)
	-> (Maobe) add `max_chunk_size` in config file
<!-- * You must provide some configuration files and default basic files to test/demonstrate -->
<!-- every feature is working during evaluation. -->


# Correction

<!-- * make sure we always remove request fd from epoll list when an error occured: -->
<!-- 	add `this->Close(request_fd)` inside `send_error_page` function() -->
<!-- * add error management when opening HTTP error HTML templates -->
<!-- * manage return value for `send` in `send_content_to_request` -->
<!-- * add  `this->Close(request_fd)` in `send_content_to_request` -->
<!-- * what are `compilation re-link issues` ? -->
* (Olano) validate `HTTP status code` for:
  - file permission error
  - file does not exist (CGI & static html)
<!-- * (Maobe) add `multiple server management` in `code` & `config file`: -->
  <!-- - manage same port being used in multiple servers -->
  <!-- - manage multiple `hostname` with multiple servers: -->
  <!-- 	  - what happen when `server_names` is left empty ? -->
* (Olano & Ting) check `client_max_body_size` (=`MBS`) beahvior with `POST`
  -> does `MBS` apply to headers or only body ?
	  -> if apply to headers : adapt `handle_client_event` function
	  -> if does not apply to headers : what is the beahvior with 1 billion headers
* (Maobe) manage routes configuration:
  - define an HTTP redirection
  - add `PATH_INFO` to define where to look for the CGI file (ex `/usr/bin/python3`)
  - add default html file when none are provided in url
  <!-- - add alterntaives directories for specific routes -->
  <!-- - limit request type (`POST`, `GET`...) -->
  <!-- - turn on or off directory listing -->
  <!-- - default file to answer if the request is a directory -->
<!-- * see behavior with invalid type requests (ex: `not POST`) -->
* (Ting) POST :
  - upload files with c++ only, not using external files or CGI
  - make the route (see `location` in config file) able to accept or not uploaded files
  - add new param in the route (see `location` in config file) to configure where uploaded files should be saved
  <!-- - data passed to CGI -->
  - do we need to manage chunk upload with CGI ?
<!-- * DELETE : -->
<!--   - delete files -->
<!-- * (Olano) validate browser headers with different request type (`GET`, `POST`...), action (`upload`, `delete`...) and status codes (`200`, `404`, `500`...) -->
<!-- * (Olano) use `Siege` (or similar tool) to stress tests the server -->
