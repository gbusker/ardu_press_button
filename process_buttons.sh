#!/bin/bash

url="http://lighting.dev.smartgaiacloud.com/diagCode/demo2_api.php?kitcode=944A0CE6F141&eventCode="

while true ; do
  read command value
  case "$command" in
    event)
      echo "$command: $value"
      echo "$url$value"
      ;;
  esac
done

