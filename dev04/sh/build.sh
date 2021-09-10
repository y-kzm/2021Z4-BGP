#!/bin/bash
docker build -t test .
tinet reconf | sudo sh -x

