#!/bin/sh

docker stop httpd-v1
docker rm httpd-v1
docker run --name httpd-v1 -d registry/httpd:v1
