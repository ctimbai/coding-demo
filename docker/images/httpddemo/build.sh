#!/bin/sh

docker rmi registry/httpd:v1
docker build -t registry/httpd:v1 -f ./Dockerfile .
