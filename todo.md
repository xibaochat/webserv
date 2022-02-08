# Subject

* define `browser` of the team
* add chunk management (CGI only?)
* You must provide some configuration files and default basic files to test/demonstrate
every feature is working during evaluation.


# Correction

* OSX only ?
<!-- * make sure we always remove request fd from epoll list when an error occured: -->
<!-- 	add `this->Close(request_fd)` inside `send_error_page` function() -->
<!-- * add error management when opening HTTP error HTML templates -->
<!-- * manage return value for `send` in `send_content_to_request` -->
<!-- * add  `this->Close(request_fd)` in `send_content_to_request` -->
<!-- * what are `compilation re-link issues` ? -->
* validate `HTTP status code` for:
  - file permission error
  - file does not exist (CGI & static html)
* add `multiple server management` in `code` & `config file`:
  - manage same port being used in multiple servers
  - manage multiple `hostname` with multiple servers:
	  - what happen when `server_names` is left empty ?
* check `max_size_request` (=`MSR`) beahvior with `POST`
  -> does `MSR` apply to headers or only body ?
	  -> if apply to headers : adapt `handle_client_event` function
	  -> if does not apply to headers : what is the beahvior with 1 billion headers
* have a dynamic buffer when reading request (may be influence by previous bullet point)
* manage routes configuration:
  - add alterntaives directories for specific routes
  - limit request type (`POST`, `GET`...)
  - turn on or off directory listing
  - define an HTTP redirection
  - default file to answer if the request is a directory
  - make the route able to accept uploaded files and configure where it should
be saved
  - make the route able to accept uploaded files and configure where it should
be saved
  - CGI
* see behavior with invalid type requests (ex: `not POST`)
* POST :
  - upload files
  - data passed to CGI
* DELETE :
  - delete files
  - delete using CGI
* validate browser headers with different request type (`GET`, `POST`...), action (`upload`, `delete`...) and status codes (`200`, `404`, `500`...)
* use `Siege` (or similar tool) to stress tests the server
* (config) `Because you won’t call the CGI directly use the full path as PATH_INFO`
