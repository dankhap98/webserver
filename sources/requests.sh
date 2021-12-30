#!/bin/sh

#GET Method Host = mdeep
curl -X GET -H "User-Agent: custom-client
Host: mdeep
Accept-Language: fr-CH, fr;q=0.9, en;q=0.8, de;q=0.7, *;q=0.5
Connection: Keep-Alive" "http://localhost:8000"

curl -X GET -H "User-Agent: custom-client
Host: mdeep2
Accept-Language: fr-CH, fr;q=0.9, en;q=0.8, de;q=0.7, *;q=0.5
Connection: Keep-Alive" "http://localhost:8000"

#DELETE Method
curl -X DELETE -H "User-Agent: custom-client
Host: mdeep
Accept-Language: en-US
Connection: Keep-Alive" "http://localhost:8000/delfile"

#Chunks
curl -X POST -H "User-Agent: custom-client
Host: mdeep
Accept-Language: fr-CH, fr;q=0.9, en;q=0.8, de;q=0.7, *;q=0.5
Connection: Keep-Alive
Transfer-Encoding: chunked" -d "5
Hello
8
, World!
0
" "http://localhost:8000"