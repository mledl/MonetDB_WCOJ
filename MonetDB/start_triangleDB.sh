#!/bin/bash

monetdbd start ~/dbfarm
monetdb start triangleDB
mclient -d triangleDB